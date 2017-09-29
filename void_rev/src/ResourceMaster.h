#ifndef VOID_RESOURCE_MASTER_H
#define VOID_RESOURCE_MASTER_H



#include <vector>
#include <map>
#include <string>
#include "Resource.h"
#include "libpq-fe.h"

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

    void LoadEdge(int sector);

    void LoadEdges();
    void SetDBConn(PGconn *dbconn);
    void AddSocket(TCPSocketPtr s);
    void RemoveSocket(const TCPSocketPtr s);

    void AddServerThread(VoidServerThread *t);
    void RemoveServerThread(VoidServerThread *t);


//    SerialObject *GetHandle(RESOURCE_TYPE type, const PrimaryKey &key, 

    void RegisterResource(ResourceType type, const PrimaryKey &key);
    void ReleaseResource(ResourceType type, const PrimaryKey &key);

    std::vector<TCPSocketPtr>::iterator GetSocketsBegin(){ return m_sockets.begin();}
    std::vector<TCPSocketPtr>::iterator GetSocketsEnd(){ return m_sockets.end();}

    std::vector<VoidServerThread*>::iterator GetServerThreadsBegin(){ return m_threads.begin();}
    std::vector<VoidServerThread*>::iterator GetServerThreadsEnd(){ return m_threads.end();}

    // Sends message to player if he/she is currently online. Otherwise, return false
    bool SendMessage(DatagramSocketPtr socket,const std::string &player, MessagePtr msg);
    void SendMessageAll(DatagramSocketPtr socket, MessagePtr msg);


    void SendSystemMail(const std::string &to, const std::string &msg);

    void SetThreadForPlayer(const VoidServerThread *t, const std::string &player);
    void RemoveThreadForPlayer(const std::string &player);

    bool isPlaying(LoginHandle *login) const;

    std::string GetConfig(const std::string &key);


    std::vector<int>::iterator GetEdgesBegin(int sector);
    std::vector<int>::iterator GetEdgesEnd(int sector);


    void Log(LOG_SEVERITY severity, std::string message); // TODO: Would also like to know which thread, player, etc

 private:



    std::map<std::string,const VoidServerThread*> m_playermap;
    std::map<std::string,std::string> m_socketnames;
    static ResourceMaster * m_instance;

    std::map<int,std::vector<int> > m_edges;
    std::vector<TCPSocketPtr> m_sockets;

    std::map<std::string,Resource*> m_resources;

    std::vector<VoidServerThread*> m_threads;
    PGconn * m_dbconn;
    ResourceMaster();
    PGconn * GetDBConn();

    Mutex m_dbmutex;
    Mutex m_threadmutex;
    Mutex m_edgemutex;
    Mutex m_socketmutex;

    void LoadSector(int i);

    
};


//#define RM ResourceMaster::GetInstance()


#endif
