#ifndef VOID_RESOURCE_MASTER_H
#define VOID_RESOURCE_MASTER_H



#include <vector>
#include <map>
#include <string>
#include <pqxx/pqxx>
#include "Resource.h"


#include "VoidServerThread.h"
#include "Message.h"
#include "Socket.h"
#include "Mutex.h"
#include "void_util.h"



class MissingConfig {
 public:
 MissingConfig(std::string key):m_key(key){
 }

  std::string getKey() const { return m_key; }
 private:
  std::string m_key;
};


class ResourceMaster
{


 public:
    static ResourceMaster * GetInstance();

    void LoadEdge(Sector sector);
    void SetInstanceName(const std::string& name);
    std::string GetInstanceName() const;
    std::string GetThreadSpawnerLocalSocketPath() const;

    void LoadEdges();
    void SetDatabaseConnectionString(const std::string& connection_string);
    void AddSocket(TCPSocketPtr s);
    void RemoveSocket(const TCPSocketPtr s);

    void AddServerThread(VoidServerThread *t);
    void RemoveServerThread(VoidServerThread *t);

    std::string GetDatabaseName() const;
    void SetDatabaseName(const std::string& str);

    std::unique_ptr<pqxx::connection_base> CreateDatabaseConnection();


    void RegisterResource(ResourceType type, const PrimaryKey &key);
    void ReleaseResource(ResourceType type, const PrimaryKey &key);

    std::vector<TCPSocketPtr>::iterator GetSocketsBegin(){ return m_sockets.begin(); }
    std::vector<TCPSocketPtr>::iterator GetSocketsEnd(){ return m_sockets.end(); }

    std::vector<VoidServerThread*>::iterator GetServerThreadsBegin(){ return m_threads.begin(); }
    std::vector<VoidServerThread*>::iterator GetServerThreadsEnd(){ return m_threads.end(); }

    // Sends message to player if he/she is currently online. Otherwise, return false
    bool SendMessage(DatagramSocketPtr socket,const std::string &player, MessagePtr msg);
    void SendMessageAll(DatagramSocketPtr socket, MessagePtr msg);


    void SendSystemMail(const std::string &to, const std::string &msg);

    void SetThreadForPlayer(const VoidServerThread *t, const std::string &player);
    void RemoveThreadForPlayer(const std::string &player);

    bool isPlaying(LoginHandle *login) const;

    std::string GetConfig(const std::string &key);


    std::vector<Sector>::iterator GetEdgesBegin(int sector);
    std::vector<Sector>::iterator GetEdgesEnd(int sector);


    void Log(LOG_SEVERITY severity, const std::string& message, const std::string& filename="", int line = -1); // TODO: Would also like to know which thread, player, etc

 private:
    ResourceMaster();
    static ResourceMaster * m_instance;
    
    void create_prepared_statements();
    std::string m_instance_name;
    std::string m_dbconn_string;
    std::map<std::string,const VoidServerThread*> m_playermap;
    std::map<std::string,std::string> m_socketnames;
    std::map<Sector,std::vector<Sector> > m_edges;
    std::vector<TCPSocketPtr> m_sockets;
    std::map<std::string,ResourcePtr> m_resources;
    std::vector<VoidServerThread*> m_threads;
    std::shared_ptr<pqxx::connection> m_dbconn;

    Mutex m_threadmutex;
    Mutex m_edgemutex;
    Mutex m_socketmutex;

    std::string m_dbname;

    void LoadSector(Sector i);

};


//#define RM ResourceMaster::GetInstance()


#endif
