#ifndef VOID_THREAD_SPAWNER_H
#define VOID_THREAD_SPAWNER_H

#include "thread.h"
#include "Socket.h"
#include <list>
#include "VoidServerThread.h"

class VoidThreadSpawner : public Thread
{
 public:
    VoidThreadSpawner(int port);
    virtual ~VoidThreadSpawner();

 protected:
    virtual bool thread_init();
    virtual void thread_destroy();
    virtual bool run();

    void Stop();

 private:
    bool m_bKeepGoing;
    UNIXDatagramSocket * m_unixsocket;
    TCPSocket *m_socket;
    int m_port;
    std::list<VoidServerThread *> threadlist;
};



#endif
