#include "ResourceMaster.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <pqxx/pqxx>
#include "void_util.h"

#include <vector>
#include "Universe.h"
#include "PlayerHandle.h"
#include "LoginHandle.h"
#include <sys/types.h>
#include <sys/socket.h>

const std::string kGetConfigStmt {"selectConfigValue"};
const std::string kSendMailStmt {"insertMail"};
const std::string kInsertLogStmt {"insertLog"};
const std::string kLoadEdgesStmt {"selectEdges"};

ResourceMaster *ResourceMaster::m_instance;

ResourceMaster::ResourceMaster()
{
    m_instance = NULL;
}


ResourceMaster *ResourceMaster::GetInstance()
{
    if(m_instance ==NULL)
    {
	m_instance = new ResourceMaster();
    }

    return m_instance;
}

void ResourceMaster::create_prepared_statements() {
   m_dbconn->prepare(kGetConfigStmt, "SELECT svalue FROM Config WHERE sname = $1;");
   m_dbconn->prepare(kSendMailStmt, "INSERT INTO mail values(nextval('mail_id_seq'), now(),$1,NULL,TRUE,$2);");
   m_dbconn->prepare(kInsertLogStmt, "INSERT INTO Log (dstamp,nseverity,smessage) values (now(), $1, $2);");
   m_dbconn->prepare(kLoadEdgesStmt, "SELECT nsector2 from edges where nsector = $1 union select nsector from edges where nsector2 = $1;");
}

std::string ResourceMaster::GetDatabaseName() const
{
  return m_dbname;
}

void ResourceMaster::SetDatabaseConnectionString(const std::string& connection_string) {
  m_dbconn_string = connection_string;
  m_dbconn = std::make_shared<pqxx::connection>(connection_string);
  create_prepared_statements();
}
void ResourceMaster::SetDatabaseName(const std::string& str)
{
  m_dbname = str;
}
void ResourceMaster::SetInstanceName(const std::string& name) {
  m_instance_name = name;
}

std::string ResourceMaster::GetInstanceName() const {
  return m_instance_name;
}

std::string ResourceMaster::GetThreadSpawnerLocalSocketPath() const {
  return "/tmp/void-threadspawner-" + m_instance_name;
}

std::string ResourceMaster::GetConfig(const std::string &key)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    pqxx::work work { *m_dbconn }; 
    pqxx::result r = work.prepared(kGetConfigStmt)(key).exec();
    if( r.size() != 1){
      throw DBException("Must have exactly one value for config named " + key);
    }
    std::string value = r[0][0].as<std::string>();
    work.commit();
    return value;
}

void ResourceMaster::SendSystemMail(const std::string &player, const std::string &msg)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    pqxx::work work { *m_dbconn };
    pqxx::result r = work.prepared(kSendMailStmt)(player)(msg).exec();
    work.commit();
}

void ResourceMaster::SendMessageAll(DatagramSocketPtr socket, MessagePtr msg)
{

    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    std::string player;

    
    for(std::map<std::string,const VoidServerThread*>::iterator iter = m_playermap.begin();
	iter != m_playermap.end();
	iter++)
    {
	player = (*iter).first;
	
	SendMessage(socket,player,msg);
    }
}


bool ResourceMaster::SendMessage(DatagramSocketPtr socket, const std::string &player, MessagePtr msg)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);

    if(m_playermap.count(player))
    {
	const VoidServerThread *t = m_playermap.find(player)->second;
	std::string socketname = "/tmp/void-" + IntToString((long)t);
	Log(DEBUG,">Sending message: " + msg->GetString() + " to: " + player + "@" + socketname);
	
	msg->WriteToSocket(socket, socketname);


	return true;
    }
    else
    {
	return false;
    }

}

void ResourceMaster::SetThreadForPlayer(const VoidServerThread * t, const std::string &player)
{
  std::lock_guard<std::mutex> lk(m_threadmutex);
    if(m_playermap.count(player) == 0)
    {
	m_playermap[player] = t;
    }

}

void ResourceMaster::RemoveThreadForPlayer(const std::string &player)
{
    std::lock_guard<std::mutex> lk(m_threadmutex);

    if(m_playermap.count(player))
    {
	m_playermap.erase(player);
    }
}



void ResourceMaster::AddSocket(TCPSocketPtr s)
{
  std::lock_guard<std::mutex> lk(m_threadmutex);
  m_sockets.push_back(s);

}


void ResourceMaster::AddServerThread(VoidServerThread *t)
{
  std::lock_guard<std::mutex> lk(m_threadmutex);  
  m_threads.push_back(t);
}

void ResourceMaster::RemoveSocket(const TCPSocketPtr s)
{
  std::lock_guard<std::mutex> lk(m_socketmutex);

  auto pos = std::find(std::begin(m_sockets),std::end(m_sockets),s);
  
  if(pos != m_sockets.end()) m_sockets.erase(pos);
  
}

void ResourceMaster::RemoveServerThread(VoidServerThread *t)
{
  std::lock_guard<std::mutex> lk(m_threadmutex);
  
  auto pos = find(m_threads.begin(),m_threads.end(),t);

    if(pos != m_threads.end()) m_threads.erase(pos);


    // Take the player out of the mapping that was associated with this thread.
    // This way, nobody has to explicity remove the player when the thread ends
    for(std::map<std::string,const VoidServerThread*>::iterator i = m_playermap.begin();
	i != m_playermap.end();
	i++)
    {
	if((*i).second == t)
	{
	    m_playermap.erase(i);
	    break;
	}
    }

}






void ResourceMaster::RegisterResource(ResourceType type, const PrimaryKey &key)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);

    ResourcePtr res = std::make_shared<Resource>(type,key);

    auto i = m_resources.find(res->GenerateID());

    Log(AUDIT, ">Registering Resource: " + res->GenerateID() + "<");

    if(i == m_resources.end())
    {
	// Need to add
	res->Lock();
	m_resources[res->GenerateID()] = res;
    }
    else
    {
      // Already have it
	i->second->Lock();
    }
    Log(AUDIT, ">Resource: " + res->GenerateID() + " locked.<");
}

void ResourceMaster::ReleaseResource(ResourceType type, const PrimaryKey &key)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    
    ResourcePtr res = std::make_shared<Resource>(type,key);

    std::map<std::string,ResourcePtr>::iterator i = m_resources.find(res->GenerateID());

    Log(AUDIT, ">Releasing Resource: " + res->GenerateID() + "<");

    if(i == m_resources.end())
    {
	// TODO: throw Exception

    }
    else
    {
	i->second->Unlock();

	// We're done, no other references exist, so dump it.
	if(i->second->NoCount())
	{
	    m_resources.erase(i);
	}
    }

}

void ResourceMaster::Log(LOG_SEVERITY severity, const std::string& message) 
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);

    LOG_SEVERITY debug_level = (LOG_SEVERITY)atoi(GetConfig("log_level").c_str());

    if ( debug_level < severity)
    {
	return;
    }
    pqxx::work work { *m_dbconn };
    pqxx::result r = work.prepared(kInsertLogStmt)(static_cast<int>(severity))(message).exec();
    work.commit();
}

void ResourceMaster::LoadEdge(Sector sector)
{
  std::lock_guard<std::mutex> lk(m_edgemutex);
  if(!m_edges.count(sector))
    LoadSector(sector);
}


void ResourceMaster::LoadSector(Sector i)
{
    std::vector<Sector> sectors;
    pqxx::work work { *m_dbconn };
    pqxx::result r = work.prepared(kLoadEdgesStmt)(i).exec();
    
    for(auto row : r)
    {
      sectors.push_back(static_cast<Sector>(row[0].as<int>()));
    }
    work.commit();
    
    
    m_edges[i] = sectors;
}


void ResourceMaster::LoadEdges()
{
  std::lock_guard<std::mutex> lk(m_edgemutex);
  for(int i=0;i< Universe::GetNumSectors(*m_dbconn); i++){
      LoadSector(i);
  }
}


std::vector<Sector>::iterator ResourceMaster::GetEdgesBegin(int sector)
{
    m_edgemutex.lock();

    if(!m_edges.count(sector))
	LoadSector(sector);
    m_edgemutex.unlock();

    return m_edges[sector].begin();
}

std::vector<Sector>::iterator ResourceMaster::GetEdgesEnd(int sector)
{
    m_edgemutex.lock();
    if(!m_edges.count(sector))
	LoadSector(sector);

    m_edgemutex.unlock();

    return m_edges[sector].end();
}

std::unique_ptr<pqxx::connection_base> ResourceMaster::CreateDatabaseConnection() {
  return std::make_unique<pqxx::connection>(m_dbconn_string);
}
