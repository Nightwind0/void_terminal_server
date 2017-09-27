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
#include "SocketException.h"

const int VOID_PORT = 5005;


using namespace std;

char  shutdownstr[] = "SHUTDOWN";

int main()
{
    int socket_num;
    int socket_num2;

    char str[1024];
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


    
    cout << "Void Revolution Server";
#ifdef VERSION
    cout << " Version: "  << VERSION;
#endif
    cout << endl;

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

	int n;
	try{
	    n = unixsocket->RecvFrom(str, 1024,0);
	}
	catch(SocketException e)
	{
	    cerr << "Caught socket exception doing RecvFrom in server: " << e.GetType() << ", " << e.GetErrno() << endl;
	}
	
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

	    try{
	    unixsocket->SendTo((void*)shutdownstr,strlen(shutdownstr),"/tmp/void-threadspawner",0);
	    }
	    catch(SocketException e)
	    {
		cerr << "Caught socket exception doing SendTo threadspawner: " << e.GetType() << ", " << e.GetErrno() << endl;
	    }

	    cout << "Waiting on threadspawner.." << endl;
	    threadspawner->Wait();
	    cout << "Threadspawner done." << endl;

	    delete threadspawner;

	    done = true;

	}

	if(command == "list")
	{
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
   
    try{
    unixsocket->Close();
    }
    catch(SocketException e)
    {
	cerr << "Caught socket exception closing unixsocket." << endl;
    }

    delete unixsocket;

    try{
	// Seeing this in the log means we got a clean shutdown. yay
	ResourceMaster::GetInstance()->Log(DEBUG,"[Void Server Shutdown]");
    }
    catch(DBException e)
    {
	cerr  << "Logging exception:" << e.GetMessage() << endl;
    }

    PQfinish(RMconn);

}
