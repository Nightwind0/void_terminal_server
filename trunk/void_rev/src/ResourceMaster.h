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

enum LOG_SEVERITY {EMERGENCY, ERROR, WARNING, DEBUG, DEBUG2, AUDIT};


class ResourceMaster
{


 public:
    static ResourceMaster * GetInstance();

    void LoadEdge(int sector);

    void LoadEdges();
    void SetDBConn(PGconn *dbconn);
    void AddSocket(TCPSocket *s);
    void RemoveSocket(TCPSocket *s);

    void AddServerThread(VoidServerThread *t);
    void RemoveServerThread(VoidServerThread *t);


//    SerialObject *GetHandle(RESOURCE_TYPE type, const PrimaryKey &key, 

    void RegisterResource(RESOURCE_TYPE type, const PrimaryKey &key);
    void ReleaseResource(RESOURCE_TYPE type, const PrimaryKey &key);

    std::vector<TCPSocket*>::iterator GetSocketsBegin(){ return m_sockets.begin();}
    std::vector<TCPSocket*>::iterator GetSocketsEnd(){ return m_sockets.end();}

    std::vector<VoidServerThread*>::iterator GetServerThreadsBegin(){ return m_threads.begin();}
    std::vector<VoidServerThread*>::iterator GetServerThreadsEnd(){ return m_threads.end();}

    // Sends message to player if he/she is currently online. Otherwise, return false
    bool SendMessage(DatagramSocket *socket,const std::string &player, Message * msg);
    void SendMessageAll(DatagramSocket *socket, Message *msg);


    void SendSystemMail(const std::string &to, const std::string &msg);

    void SetThreadForPlayer(const VoidServerThread *t, const std::string &player);
    void RemoveThreadForPlayer(const std::string &player);

    bool isPlaying(LoginHandle *login) const;


    std::vector<int>::iterator GetEdgesBegin(int sector);
    std::vector<int>::iterator GetEdgesEnd(int sector);


    void Log(LOG_SEVERITY severity, std::string message); // TODO: Would also like to know which thread, player, etc

 private:



    std::map<std::string,const VoidServerThread*> m_playermap;
    std::map<std::string,std::string> m_socketnames;
    static ResourceMaster * m_instance;

    std::map<int,std::vector<int> > m_edges;
    std::vector<TCPSocket*> m_sockets;

    std::map<std::string,Resource*> m_resources;

    std::vector<VoidServerThread*> m_threads;
    PGconn * m_dbconn;
    ResourceMaster();
    PGconn * GetDBConn();

    NormalMutex m_dbmutex;
    NormalMutex m_threadmutex;
    NormalMutex m_edgemutex;
    NormalMutex m_socketmutex;

    void LoadSector(int i);

    
};


//#define RM ResourceMaster::GetInstance()


#endif
