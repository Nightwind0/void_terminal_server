
#include "EdgeLoadThread.h"
#include "ResourceMaster.h"
#include "Universe.h"
#include <iostream>
#include "void_util.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


void EdgeLoadThread::OpenDataBaseConnection()
{
    m_dbconn = PQsetdbLogin(NULL,NULL,NULL,NULL,"void","void","tiTVPok?");


    if(PQstatus(m_dbconn) == CONNECTION_BAD)
    {
	std::cerr << PQerrorMessage(m_dbconn) << std::endl;

	throw DBException(PQerrorMessage(m_dbconn));
    }
}

void EdgeLoadThread::CloseDataBaseConnection()
{
    PQfinish(m_dbconn);
}


bool EdgeLoadThread::thread_init()
{
    OpenDataBaseConnection();

    std::string sockname = "/tmp/void-edgeloader" ;
    unlink(sockname.c_str());

    m_usocket = new UNIXDatagramSocket(sockname);

    m_usocket->Create();
    m_usocket->Bind(0,sockname);


    return true;
}
bool EdgeLoadThread::run()
{
    ResourceMaster *RM = ResourceMaster::GetInstance();

    int sectorcount = Universe::GetNumSectors(m_dbconn);

    for(int i =0; i < sectorcount; i++)
    {
	RM->LoadEdge(i);
    }




    Message message;
    message.SetType(Message::ADMIN);
    message.SetString("Flight computer reports an increase in path computation speed");

    RM->SendMessageAll(m_usocket,&message);

    return true;
}
void  EdgeLoadThread::thread_destroy()
{
    CloseDataBaseConnection();

    m_usocket->Close();
}
