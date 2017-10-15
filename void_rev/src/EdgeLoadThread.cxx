#include "EdgeLoadThread.h"
#include "ResourceMaster.h"
#include "Universe.h"
#include <iostream>
#include "void_util.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>



bool EdgeLoadThread::thread_init()
{
  std::string sockname = "/tmp/void-edgeloader-" + ResourceMaster::GetInstance()->GetInstanceName();
  unlink(sockname.c_str());

  m_usocket = std::make_shared<UNIXDatagramSocket>(sockname);
  
  m_usocket->Create();
  m_usocket->Bind(0,sockname);
  

  return true;
}
bool EdgeLoadThread::run()
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    std::unique_ptr<pqxx::connection_base> conn = RM->CreateDatabaseConnection();
    size_t sectorcount = Universe::GetNumSectors(*conn);

    for(size_t i =0; i < sectorcount; i++)
    {
	RM->LoadEdge(i);
    }


    MessagePtr message = std::make_shared<Message>();
    message->SetType(Message::ADMIN);
    message->SetString("Flight computer reports an increase in path computation speed");

    RM->SendMessageAll(m_usocket,message);

    return true;
}
void  EdgeLoadThread::thread_destroy()
{
    m_usocket->Close();
}
