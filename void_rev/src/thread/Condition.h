#ifndef CONDITION_H
#define CONDITION_H

#include <pthread.h>
#include "Mutex.h"

//class Mutex;


class Condition
{
 public:
    Condition(Mutex *pmutex);
    ~Condition();
    
    void Wait();
    void Signal();
    void Broadcast();

    Mutex * GetMutex()const;


 private:
    
    Mutex *m_pmutex;
    pthread_cond_t m_condition;
    
};


#endif
