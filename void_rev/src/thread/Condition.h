#ifndef CONDITION_H
#define CONDITION_H

#include <condition_variable>
#include "Mutex.h"

//class Mutex;


class Condition
{
 public:
    Condition(Mutex &pmutex);
    ~Condition();
    
    void Wait();
    void Signal();
    void Broadcast();

    Mutex& GetMutex()const;


 private:
    
    Mutex& m_mutex;
    std::condition_variable m_cv;
    bool m_ready;
};


#endif
