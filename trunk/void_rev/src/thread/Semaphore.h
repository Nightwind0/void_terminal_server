#ifndef SEMAPHORE_H
#define SEMAPHORE_H


#include <pthread.h>
#include <semaphore.h>
#include "PThreadException.h"


class SemaphoreException : public PThreadException
{
 public:

    enum Type {SEMUNKNOWN, RANGE,  UNKNOWN };
    SemaphoreException(Type type = UNKNOWN):m_type(type){}

    ~SemaphoreException(){}

    Type GetType()const{ return m_type; }
 private:
    Type m_type;
};



class Semaphore
{
    public:
    Semaphore();
    ~Semaphore();
    
    void Wait();
    void Post();
    bool Try();
    int GetCount() const;
    
    
 private:
    sem_t                   m_semaphore;
};

#endif
