#ifndef EDGE_LOAD_THREAD_H
#define EDGE_LOAD_THREAD_H

#include "thread.h"
#include <pqxx/pqxx>
#include "Socket.h"

class EdgeLoadThread : public Thread
{

 protected:

    virtual bool thread_init();
    virtual void thread_destroy();
    virtual bool run();


 private:
    UNIXDatagramSocketPtr m_usocket;


};




#endif
