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
#include "void_util.h"
#include "ResourceMaster.h"
#include "libpq-fe.h"
#include "EdgeLoadThread.h"
#include "VoidThreadSpawner.h"
#include <unistd.h>

const int VOID_PORT = 5005;


using namespace std;

char  shutdownstr[] = "SHUTDOWN";

int main()
{
    int socket_num;
    int socket_num2;
    sockaddr_un local_addr;
    sockaddr_un remote_addr;
    char str[65536];
    PGconn *RMconn;

    UNIXDatagramSocket * unixsocket = new UNIXDatagramSocket("/tmp/void-server");

    VoidThreadSpawner *threadspawner = new VoidThreadSpawner(VOID_PORT);


    EdgeLoadThread   *edge_thread = new EdgeLoadThread();


    bool done = false;
    

    RMconn = PQsetdbLogin(NULL,NULL,NULL,NULL,"void","void","tiTVPok?");

    if(PQstatus(RMconn) == CONNECTION_BAD)
    {
	std::cerr << PQerrorMessage(RMconn) << std::endl;

	throw DBException(PQerrorMessage(RMconn));
    }


    ResourceMaster::GetInstance()->SetDBConn(RMconn);
//    ResourceMaster::GetInstance()->LoadEdges(); // TODO: Make this depend on a setting from the config table

//    server = new TCPServer(VOID_PORT,5,1,10,1,void_thread,false); // ipV6 support? Switch that false to a true and blammo.

    
    cout << "Void Revolution Server: Version " << VOID_VER_MAJOR << '.' << VOID_VER_MINOR << '.' << VOID_VER_INCR << endl;

    try{

	threadspawner->Start();

	ResourceMaster::GetInstance()->Log(DEBUG,"[Void Server Starting]");
    }
    catch(DBException e)
    {
	cerr  << "Logging exception:" << e.GetMessage() << endl;
	PQfinish(RMconn);
	exit(1);
    }

    unlink("/tmp/void-socket");
    unixsocket->Create();
    unixsocket->Bind(0,"/tmp/void-socket");


    edge_thread->Start();


    while(!done)
    {

	

            int  n = unixsocket->RecvFrom(str, 65536, 0);

	    str[n] = 0;
	    string command = str; 

	    try{
		ResourceMaster::GetInstance()->Log(DEBUG,"[Server Socket Got Command :" + command + "]");
	    }
	    catch(DBException e)
	    {
		cerr  << "Logging exception:" << e.GetMessage() << endl;
	    }
	    

	if(command == "shutdown" || command == "quit")
	{
	    cout << "Immediate Shutdown! Dropping connections..." << endl;
	    done = true;
	   
	    unixsocket->SendTo((void*)shutdownstr,strlen(shutdownstr),"/tmp/void-threadspawner",0);

	    threadspawner->Wait(); 
	    delete threadspawner;

	    done = true;

	}

	if(command == "list")
	{
	    // TODO: List connections
	    cout << "TCP Connections:" << endl;
	    for(vector<TCPSocket*>::iterator i = ResourceMaster::GetInstance()->GetSocketsBegin();
		i != ResourceMaster::GetInstance()->GetSocketsEnd();
		i++)
	    {
		TCPSocket *s = *i;
		cout << s->GetAddress() << endl;
	    }

	    cout << "Logged in users:" << endl;

	    for(vector<VoidServerThread*>::iterator i= ResourceMaster::GetInstance()->GetServerThreadsBegin();
		i != ResourceMaster::GetInstance()->GetServerThreadsEnd();
		i++)
	    {
		VoidServerThread *t = *i;
		LoginHandle *login = t->GetLogin();

		cout << (std::string)login->GetLogin() << endl;
		
	    }

	}


	if(command == "help" || command == "?")
	{
	    cout << "Commands: shutdown, list, quit, help" << endl;
	}

	


    }


    edge_thread->Wait();

    delete edge_thread;
   

    unixsocket->Close();

    delete unixsocket;

    try{

	ResourceMaster::GetInstance()->Log(DEBUG,"[Void Server Shutdown]");
    }
    catch(DBException e)
    {
	cerr  << "Logging exception:" << e.GetMessage() << endl;
    }

    PQfinish(RMconn);

}
