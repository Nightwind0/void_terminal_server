#include "thread.h"
#include <errno.h>

Mutex * Condition::GetMutex() const
{
    return m_pmutex;
}


Condition::Condition(Mutex *mutex) :m_pmutex(mutex)
{
    pthread_condattr_t  attr;

    pthread_condattr_init(&attr);
    pthread_cond_init(&m_condition, &attr);
    pthread_condattr_destroy(&attr);
}

Condition::~Condition()
{
    while (pthread_cond_destroy(&m_condition) == EBUSY)
    {
        Broadcast();
    }
}

void Condition::Wait()
{
    pthread_cond_wait(&m_condition, m_pmutex->GetMutex());
}

void Condition::Signal()
{
    pthread_cond_signal(&m_condition);
}


void Condition::Broadcast()
{
    pthread_cond_broadcast(&m_condition);
}
