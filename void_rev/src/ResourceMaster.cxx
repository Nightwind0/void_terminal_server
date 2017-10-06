#include "ResourceMaster.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "void_util.h"
#include "libpq-fe.h"
#include <vector>
#include "Universe.h"
#include "PlayerHandle.h"
#include "LoginHandle.h"
#include <sys/types.h>
#include <sys/socket.h>



ResourceMaster *ResourceMaster::m_instance;

ResourceMaster::ResourceMaster()
{
    m_instance = NULL;
    m_dbconn = NULL;
}


ResourceMaster *ResourceMaster::GetInstance()
{
    if(m_instance ==NULL)
    {
	m_instance = new ResourceMaster();
    }

    return m_instance;
}


std::string ResourceMaster::GetDatabaseName() const
{
  return m_dbname;
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
    std::lock_guard<std::mutex> dblk(m_dbmutex);

    std::string sql = "select svalue from config where sname = '" + PrepareForSQL(key) + "';";

    PGresult *dbresult = PQexec(m_dbconn, sql.c_str());

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK || PQntuples(dbresult) < 1)    
    {
	Log(ERROR,">Config for key: " + key + " not found<");
	PQclear(dbresult);
	throw MissingConfig(key);
    }
    
    return PQgetvalue(dbresult,0,0);
}

void ResourceMaster::SendSystemMail(const std::string &player, const std::string &msg)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    std::lock_guard<std::mutex> dblk(m_dbmutex);
    std::string sql = "insert into mail values(nextval('mail_id_seq'), now(),'" + player + "',NULL, TRUE,'" + PrepareForSQL(msg) + "');";

    PGresult *dbresult = PQexec(m_dbconn, sql.c_str());

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK )
    {
    }

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


PGconn* ResourceMaster::GetDBConn()
{
    if (m_dbconn != NULL)
	return m_dbconn;
    else
    {
	throw DBException("No DB Connection for ResourceMaster");
    }
}

void ResourceMaster::SetDBConn(PGconn *dbconn)
{
    m_dbconn = dbconn;
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

    Resource *res= new Resource(type,key);

    std::map<std::string,Resource*>::iterator i = m_resources.find(res->GenerateID());

    Log(DEBUG2, ">Registering Resource: " + res->GenerateID() + "<");

    if(i == m_resources.end())
    {
	// Need to add
	res->Lock();
	m_resources[res->GenerateID()] = res;
	
    }
    else
    {
	// Already have it
	delete res;
	i->second->Lock();
	
    }
    Log(DEBUG2, ">Resource: " + res->GenerateID() + " locked.<");
}

void ResourceMaster::ReleaseResource(ResourceType type, const PrimaryKey &key)
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);
    
    Resource *res = new Resource(type,key);

    std::map<std::string,Resource*>::iterator i = m_resources.find(res->GenerateID());

    Log(DEBUG2, ">Releasing Resource: " + res->GenerateID() + "<");

    delete res;



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
	    delete i->second;
	    m_resources.erase(i);
	   
	}
    }

}

void ResourceMaster::Log(LOG_SEVERITY severity, std::string message) 
{
    static Mutex mutex;
    std::lock_guard<std::mutex> lk(mutex);

    LOG_SEVERITY debug_level = (LOG_SEVERITY)atoi(GetConfig("log_level").c_str());

    if ( debug_level < severity)
    {
	return;
    }

    PGresult *dbresult;

    std::ostringstream os;
    os << "INSERT INTO Log (dstamp,nseverity,smessage) values (now()," << severity << ",\'" << PrepareForSQL(message) << '\'' << ");";

    {
      std::lock_guard<std::mutex> lk(m_dbmutex);

    dbresult = PQexec(m_dbconn, os.str().c_str());
    
    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK )
    {
	std::string err = PQresultErrorMessage(dbresult);
	std::cerr << err << " from " << os.str() << std:: endl;
	PQclear(dbresult);

	throw DBException(err);
    }

    PQclear(dbresult);
    }

}

void ResourceMaster::LoadEdge(int sector)
{
  std::lock_guard<std::mutex> lk(m_edgemutex);
  if(!m_edges.count(sector))
    LoadSector(sector);
}


void ResourceMaster::LoadSector(int i)
{

    
    std::vector<int> sectors;
    
    std::string query = "select nsector2 from edges where nsector =" + IntToString(i) + " union select nsector from edges where nsector2 =" +IntToString(i) + ';';
    
    std::lock_guard<std::mutex> dblk(m_dbmutex);
    PGresult *dbresult =  PQexec(m_dbconn, query.c_str());
    
    ExecStatusType  status = PQresultStatus(dbresult);
    
    if(status != PGRES_TUPLES_OK)
    {
	DBException e("Load sector error:" + std::string(PQerrorMessage(m_dbconn)) + std::string(" Query was: ") + query);
	PQclear(dbresult);
	throw e;
    }
    
    int tuples = PQntuples(dbresult);
    
    for(int j=0;j<tuples;j++)
    {
	sectors.push_back(atoi(PQgetvalue(dbresult,j,0)));
    }
    
    
    m_edges[i] = sectors;
    
    PQclear(dbresult);
}


void ResourceMaster::LoadEdges()
{
  std::lock_guard<std::mutex> lk(m_edgemutex);
    for(int i=0;i< Universe::GetNumSectors(m_dbconn); i++)
    {

	if((i % 10) == 0)
	    std::cout << '.' << std::flush;
	LoadSector(i);
	
    }
}


std::vector<int>::iterator ResourceMaster::GetEdgesBegin(int sector)
{
    m_edgemutex.lock();

    if(!m_edges.count(sector))
	LoadSector(sector);
    m_edgemutex.unlock();

    return m_edges[sector].begin();
}

std::vector<int>::iterator ResourceMaster::GetEdgesEnd(int sector)
{
    m_edgemutex.lock();
    if(!m_edges.count(sector))
	LoadSector(sector);

    m_edgemutex.unlock();

    return m_edges[sector].end();
}
