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
#include <unordered_map>
#include "SocketException.h"

const int DEFAULT_VOID_PORT = 5005;


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

void usage() {
  std::cout << "Void Usage" << std::endl;
  std::cout << "-p <port> use specified TCP port. Config file overrides." << std::endl;
  std::cout << "--db <database name> use specified database. Config file overrides." << std::endl;
  std::cout << "-i <instance name> give this process a specific name for control." << std::endl;
  std::cout << "-c <path to config file> use config file (default is /etc/void/void.conf)" << std::endl;
  std::cout << "-h this help" << std::endl;
}

int main(int argc, char** argv)
{
  bool daemon_mode = true;
  int port = DEFAULT_VOID_PORT;
  std::string database = "void";
  std::string instance = "default";
  
  std::string configpath = "/etc/void/void.conf";
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if(arg == "-d") {

    } else if(arg == "-c" && argc >= i+1) {
      configpath = argv[i+1];
    } else if(arg == "-p" && argc >= i+1) {
      port = atoi(argv[i+1]);
    } else if(arg == "--db" && argc >= i+1) {
      database = argv[i+1];
    } else {
      usage();
      return 0;
    }
  }

  daemon();
  
  std::unordered_map<std::string, std::string> conf_values;
  std::ifstream conf_in(configpath);
  while(conf_in.good()){
    std::string key, value;
    conf_in >> key >> value;
    conf_values[key] = value;
  }

  auto port_conf_it = conf_values.find("PORT");
  if(port_conf_it != conf_values.end()) {
    port = std::stoi(port_conf_it->second);
  }

  auto db_conf_it = conf_values.find("DATABASE");
  if(db_conf_it != conf_values.end()) {
    database = db_conf_it->second;
  }

  auto instance_conf_it = conf_values.find("INSTANCE");
  if(instance_conf_it != conf_values.end()) {
    instance = instance_conf_it->second;
  }

  if(daemon_mode)
    daemon();

    
  int socket_num;
  int socket_num2;

  srand(time(NULL));

  char str[1024];
  PGconn *RMconn;

  ResourceMaster::GetInstance()->SetInstanceName(instance);
  ResourceMaster::GetInstance()->SetDatabaseName(database);

  std::string pipe_path = "/tmp/"s + "void-socket-"s + instance;

  std::unique_ptr<UNIXDatagramSocket>  unixsocket = std::make_unique<UNIXDatagramSocket>(pipe_path);
  std::unique_ptr<VoidThreadSpawner> threadspawner = std::make_unique<VoidThreadSpawner>(port);
  std::unique_ptr<EdgeLoadThread>   edge_thread = std::make_unique<EdgeLoadThread>();


  bool done = false;
    

  RMconn = PQsetdbLogin(NULL,NULL,NULL,NULL,database.c_str(),"void","tiTVPok?");

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

    ResourceMaster::GetInstance()->Log(DEBUG,"[Void Server Starting]"s + instance);
  }
  catch(DBException e) {
    syslog(LOG_ERR, e.GetMessage().c_str());
    PQfinish(RMconn);
    exit(1);
  }



  unlink(pipe_path.c_str());
  unixsocket->Create();
  unixsocket->Bind(0,pipe_path);


  edge_thread->Start();


  while(!done) {
    int n;
    try{
      unixsocket->Select(2 /* wake up every other second */);
      n = unixsocket->RecvFrom(str, 1024,0);
    }
    catch(SocketException e) {
      cerr << "Caught socket exception doing RecvFrom in server: " << e.GetType() << ", " << e.GetErrno() << endl;
    }

    string command;
    if(n > 0) {
      str[n] = 0;
      command = str;
    }

    try{
      ResourceMaster::GetInstance()->Log(DEBUG,"[Server Socket Got Command :" + command + "]");
    }
    catch(DBException e) {
      syslog(LOG_ERR, ("Logging exception:"s + e.GetMessage()).c_str());
    }
      

    if(command == "shutdown" || command == "quit") {
      syslog(LOG_NOTICE, "Immediate Shutdown! Dropping connections...");
      done = true;

      try{
	unixsocket->SendTo((void*)shutdownstr,strlen(shutdownstr),ResourceMaster::GetInstance()->GetThreadSpawnerLocalSocketPath(),0);
      }
      catch(SocketException e) {
	cerr << "Caught socket exception doing SendTo threadspawner: " << e.GetType() << ", " << e.GetErrno() << endl;
      }

      syslog(LOG_NOTICE, "Waiting on threadspawner...");
      threadspawner->Wait();
      syslog(LOG_NOTICE, "Threadspawner done.");

      done = true;
    }

    if(command == "list") {
      // TODO: List connections
      syslog(LOG_NOTICE, "Listing connections:");
      for(vector<TCPSocketPtr>::iterator i = ResourceMaster::GetInstance()->GetSocketsBegin();
	  i != ResourceMaster::GetInstance()->GetSocketsEnd();
	  i++)
	{
	  TCPSocketPtr s = *i;
	  syslog(LOG_NOTICE, s->GetAddress().c_str());
	}


      syslog(LOG_NOTICE, "Connected players:");
      for(vector<VoidServerThread*>::iterator i= ResourceMaster::GetInstance()->GetServerThreadsBegin();
	  i != ResourceMaster::GetInstance()->GetServerThreadsEnd();
	  i++) {
	VoidServerThread *t = *i;
	LoginHandlePtr login = t->GetLogin();

	syslog(LOG_NOTICE, ((std::string)login->GetLogin()).c_str());
		
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
