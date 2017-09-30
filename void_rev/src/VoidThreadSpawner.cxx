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
using std::cout;

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
      m_socket = std::make_shared<TCPSocket>("localhost",m_port);
	m_socket->Create();
	m_socket->Bind(m_port, "localhost");
	m_socket->Listen();
	cout << "Listening on port " << m_port << endl;
    }
    catch(SocketException &ex)
    {
	cerr << "VoidThreadSpawner Socket Bind Error: " << ex.GetErrno() << endl;
	m_socket->Close();
	return false;
    }
    
    try{
	unlink("/tmp/void-threadspawner");
	m_unixsocket = std::make_shared<UNIXDatagramSocket>("/tmp/void-threadspawner",0);
	m_unixsocket->Create();
	m_unixsocket->Bind(0,"/tmp/void-threadspawner");
    }
    catch(SocketException &exs)
    {
	cerr << "VoidThreadSpanwer Unix Socket Error:" << exs.GetErrno() << endl;
	return false;
    }
	


    m_bKeepGoing = true;

    return true;
}
 
void VoidThreadSpawner::thread_destroy()
{

    m_socket->Close();

    for(std::list<VoidServerThread*>::iterator iter = threadlist.begin();
	iter != threadlist.end();
	iter++)
    {
	ResourceMaster::GetInstance()->Log(DEBUG2, "** Waiting on Threads in ThreadSpawner **");
	(*iter)->Wait();
	delete *iter;
    }
    ResourceMaster::GetInstance()->Log(DEBUG2, "** Threadspawner finished waiting on threads **");

}

void VoidThreadSpawner::Stop()
{
    std::cout << "Thread stopping." << std::endl;
    m_bKeepGoing = false;
}

bool VoidThreadSpawner::run()
{

    while(m_bKeepGoing)
    {

      Socket::eSelectResult select_result = m_socket->Select(*m_unixsocket); 
      if(select_result == Socket::eSelectResult::THIS_SOCKET)
	{

	    std::cout << "TCP Socket came in." << std::endl;
	    ResourceMaster::GetInstance()->Log(DEBUG2, "** Select Comes to TCP Socket **");
	    SocketPtr newsocket;
	     
	    newsocket = m_socket->Accept();
	    std::cout << "TCP Socket came in." << std::endl;
	    VoidServerThread *newthread = new VoidServerThread(std::dynamic_pointer_cast<TCPSocket>(newsocket));
	    
	    threadlist.push_back(newthread);
	    
	    newthread->Start();
	    ResourceMaster::GetInstance()->Log(DEBUG2, "** New Thread Started **");
	}
      else if(select_result == Socket::eSelectResult::OTHER_SOCKET)
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
	      MessagePtr msg = std::make_shared<Message>();
	      msg->SetType(Message::SYSTEM);
	      msg->SetString("SHUTDOWN");
	      msg->SetFrom("THREADSPAWNER");
	      ResourceMaster::GetInstance()->SendMessageAll(m_unixsocket, msg);
	      return true;
	    }
	}
	
    }
}



