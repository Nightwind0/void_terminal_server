#include "thread.h"
#include "VoidThreadSpawner.h"
#include "ResourceMaster.h"
#include <list>
#include <unistd.h>
#include "SocketException.h"
#include <iostream>

using std::list;
using std::cerr;
using std::endl;


VoidThreadSpawner::VoidThreadSpawner(int port):m_port(port)
{
    
}
 
VoidThreadSpawner::~VoidThreadSpawner()
{
    Stop();
}

bool VoidThreadSpawner::thread_init()
{

    try{
    m_socket = new TCPSocket("playvoid.servegame.com",m_port);
    m_socket->Create();
    m_socket->Bind(m_port, "playvoid.servegame.com");
    m_socket->Listen();
    }
    catch(SocketException &ex)
    {
	cerr << "VoidThreadSpawner Socket Bind Error: " << ex.GetErrno() << endl;
	throw ex;
    }

    try{
    unlink("/tmp/void-threadspawner");
    m_unixsocket = new UNIXDatagramSocket("/tmp/void-threadspawner",0);
    m_unixsocket->Create();
    m_unixsocket->Bind(0,"/tmp/void-threadspawner");
    }
    catch(SocketException &exs)
    {
	cerr << "VoidThreadSpanwer Unix Socket Error:" << exs.GetErrno() << endl;
	throw exs;
    }
	


    m_bKeepGoing = true;
}
 
void VoidThreadSpawner::thread_destroy()
{

    m_socket->Close();

    for(std::list<VoidServerThread*>::iterator iter = threadlist.begin();
	iter != threadlist.end();
	iter++)
    {
	(*iter)->Wait();
	delete *iter;
    }

    delete m_socket;

}

void VoidThreadSpawner::Stop()
{
    m_bKeepGoing = false;
}

bool VoidThreadSpawner::run()
{


    while(m_bKeepGoing)
    {

	if(m_socket->Select(*m_unixsocket))
	{

	       ResourceMaster::GetInstance()->Log(DEBUG2, "** Select Comes to TCP Socket **");
	    Socket * newsocket;
	     
	    newsocket = m_socket->Accept();
	    
	    VoidServerThread *newthread = new VoidServerThread((TCPSocket*)newsocket);
	    
	    threadlist.push_back(newthread);
	    
	    newthread->Start();
	    ResourceMaster::GetInstance()->Log(DEBUG2, "** New Thread Started **");
	}
	else
	{
	    // Got a message
	  ResourceMaster::GetInstance()->Log(DEBUG2, "(( Message to Threadspawner))");

	    char buffer[1024];

	    int buflen = m_unixsocket->RecvFrom((void*)buffer,1024,0);
	    buffer[buflen]  = 0;

	    std::string message = buffer;

	    ResourceMaster::GetInstance()->Log(DEBUG2, "(( Message =" + message + "))");

	    if(message == "SHUTDOWN")
	    {
		Message msg;
		msg.SetType(Message::SYSTEM);
		msg.SetString("SHUTDOWN");
		msg.SetFrom("THREADSPAWNER");
		ResourceMaster::GetInstance()->SendMessageAll(m_unixsocket, &msg);
		return true;
	    }
	}
	
    }
}



