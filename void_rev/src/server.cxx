#include "VoidServerThread.h"
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <syslog.h>
#include "void_util.h"
#include "ResourceMaster.h"
#include "libpq-fe.h"
#include "EdgeLoadThread.h"
#include "VoidThreadSpawner.h"
#include <unistd.h>
#include "SocketException.h"

const int VOID_PORT = 5005;


using namespace std;

char  shutdownstr[] = "SHUTDOWN";


static void daemon()
{
  pid_t pid;

  /* Fork off the parent process */
  pid = fork();

  /* An error occurred */
  if (pid < 0)
    exit(EXIT_FAILURE);

  /* Success: Let the parent terminate */
  if (pid > 0)
    exit(EXIT_SUCCESS);

  /* On success: The child process becomes session leader */
  if (setsid() < 0)
    exit(EXIT_FAILURE);


  /* Fork off for the second time*/
  pid = fork();

  /* An error occurred */
  if (pid < 0)
    exit(EXIT_FAILURE);

  /* Success: Let the parent terminate */
  if (pid > 0)
    exit(EXIT_SUCCESS);

  /* Set new file permissions */
  umask(0);

  /* Change the working directory to the root directory */
  /* or another appropriated directory */
  chdir("/");

  /* Close all open file descriptors */
  int x;
  for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
      close (x);
    }

  /* Open the log file */
  openlog ("void", LOG_PID, LOG_DAEMON);
}
int main(int argc, char** argv)
{
  bool daemon_mode = false;
  std::string configpath = "/etc/void/void.conf";
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if(arg == "-d") {
      daemon_mode = true;
    } else if(arg == "-c" && argc >= i + 1) {
      configpath = argv[i+1];
    }
  }

  if(daemon_mode) {
    daemon();
  }

    
  int socket_num;
  int socket_num2;

  srand(time(NULL));

  char str[1024];
  PGconn *RMconn;

  std::unique_ptr<UNIXDatagramSocket>  unixsocket = std::make_unique<UNIXDatagramSocket>("/tmp/void-server");

  std::unique_ptr<VoidThreadSpawner> threadspawner = std::make_unique<VoidThreadSpawner>(VOID_PORT);

  std::unique_ptr<EdgeLoadThread>   edge_thread = std::make_unique<EdgeLoadThread>();


  bool done = false;
    

  RMconn = PQsetdbLogin(NULL,NULL,NULL,NULL,"void","void","tiTVPok?");

  if(PQstatus(RMconn) == CONNECTION_BAD) {
    
    std::cerr << PQerrorMessage(RMconn) << std::endl;
    
    throw DBException(PQerrorMessage(RMconn));
  }
  

  ResourceMaster::GetInstance()->SetDBConn(RMconn);


    
  syslog(LOG_NOTICE, "Void Revolution Server");
#ifdef VERSION
  syslog(LOG_NOTICE," Version: "  << VERSION);
#endif


  try{

    threadspawner->Start();

    ResourceMaster::GetInstance()->Log(DEBUG,"[Void Server Starting]");
  }
  catch(DBException e) {
    syslog(LOG_ERR, e.GetMessage().c_str());
    PQfinish(RMconn);
    exit(1);
  }

  unlink("/tmp/void-socket");
  unixsocket->Create();
  unixsocket->Bind(0,"/tmp/void-socket");


  edge_thread->Start();


  while(!done) {
    int n;
    try{
      n = unixsocket->RecvFrom(str, 1024,0);
    }
    catch(SocketException e) {
      cerr << "Caught socket exception doing RecvFrom in server: " << e.GetType() << ", " << e.GetErrno() << endl;
    }
	
    str[n] = 0;
    string command = str; 

    try{
      ResourceMaster::GetInstance()->Log(DEBUG,"[Server Socket Got Command :" + command + "]");
    }
    catch(DBException e) {
      cerr  << "Logging exception:" << e.GetMessage() << endl;
    }
      

    if(command == "shutdown" || command == "quit") {
      cout << "Immediate Shutdown! Dropping connections..." << endl;
      done = true;

      try{
	unixsocket->SendTo((void*)shutdownstr,strlen(shutdownstr),"/tmp/void-threadspawner",0);
      }
      catch(SocketException e) {
	cerr << "Caught socket exception doing SendTo threadspawner: " << e.GetType() << ", " << e.GetErrno() << endl;
      }

      cout << "Waiting on threadspawner.." << endl;
      threadspawner->Wait();
      cout << "Threadspawner done." << endl;

      done = true;
    }

    if(command == "list") {
      // TODO: List connections
      cout << "TCP Connections:" << endl;
      for(vector<TCPSocketPtr>::iterator i = ResourceMaster::GetInstance()->GetSocketsBegin();
	  i != ResourceMaster::GetInstance()->GetSocketsEnd();
	  i++)
	{
	  TCPSocketPtr s = *i;
	  cout << s->GetAddress() << endl;
	}

      cout << "Logged in users:" << endl;

      for(vector<VoidServerThread*>::iterator i= ResourceMaster::GetInstance()->GetServerThreadsBegin();
	  i != ResourceMaster::GetInstance()->GetServerThreadsEnd();
	  i++) {
	VoidServerThread *t = *i;
	LoginHandlePtr login = t->GetLogin();

	cout << (std::string)login->GetLogin() << endl;
		
      }

    }


    if(command == "help" || command == "?") {
      cout << "Commands: shutdown, list, quit, help" << endl;
    }

	


  }

  edge_thread->Wait();
  
  try{
    unixsocket->Close();
  }
  catch(SocketException e) {
    cerr << "Caught socket exception closing unixsocket." << endl;
  }


  try{
    // Seeing this in the log means we got a clean shutdown. yay
    ResourceMaster::GetInstance()->Log(DEBUG,"[Void Server Shutdown]");
  }
  catch(DBException e) {
    cerr  << "Logging exception:" << e.GetMessage() << endl;
  }

  PQfinish(RMconn);

}
