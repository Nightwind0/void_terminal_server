#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include "PThreadException.h"




class MutexException : public PThreadException
{
 public:

    enum Type {WOULDDEADLOCK, PERMISSION, INVALID,  UNKNOWN };
    MutexException(Type type = UNKNOWN):m_type(type){}

    ~MutexException(){}

    Type GetType()const{ return m_type; }
 private:
    Type m_type;
};

class Mutex
{
    
 public:
    
    Mutex();
    virtual ~Mutex();
    virtual int CreateMutex()=0;
    pthread_mutex_t * GetMutex() { return &m_mutex; }
    
    void Lock();
    void Unlock();
    bool Try();


 protected:    




    pthread_mutex_t         m_mutex;
};

class NormalMutex : public Mutex
{
 public:
    NormalMutex();
    ~NormalMutex();
    virtual int CreateMutex();
 protected:


};

class ErrChkMutex : public Mutex
{
 public:
    ErrChkMutex();
    ~ErrChkMutex();
    virtual int CreateMutex();
 protected:

};



#endif
