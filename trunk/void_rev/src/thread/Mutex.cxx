#include "Mutex.h"
#include <errno.h>
#include <pthread.h>

NormalMutex::NormalMutex()
{
    CreateMutex();
}

int NormalMutex::CreateMutex()
{
    pthread_mutexattr_t  attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

NormalMutex::~NormalMutex()
{
}

ErrChkMutex::ErrChkMutex()
{
    CreateMutex();
}

int ErrChkMutex::CreateMutex()
{
    pthread_mutexattr_t  attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

ErrChkMutex::~ErrChkMutex()
{
}


Mutex::Mutex()
{
    
}

Mutex::~Mutex()
{
    if(pthread_mutex_destroy(&m_mutex) == EBUSY)
    {
        Unlock();
    }
}




void Mutex::Lock()
{
    int res = pthread_mutex_lock(&m_mutex);

    if(!res) return;


    if(res == EDEADLK)
	throw MutexException(MutexException::WOULDDEADLOCK);
    else if (res == EINVAL)
	throw MutexException(MutexException::INVALID);
    else   throw MutexException(MutexException::UNKNOWN);
    
}

void Mutex::Unlock()
{
    int res = pthread_mutex_unlock(&m_mutex);

    if(!res) return;

    if(res == EPERM)
	throw MutexException(MutexException::PERMISSION);
    else if (res == EINVAL)
	throw MutexException(MutexException::INVALID);
    else  throw MutexException(MutexException::UNKNOWN);
   
}


bool Mutex::Try()
{

    int res = pthread_mutex_trylock(&m_mutex);

    if(!res) return true;

    if(res == EBUSY)
	return false;
    
    else if (res == EINVAL)
	throw MutexException(MutexException::INVALID);
    else   throw MutexException(MutexException::UNKNOWN);
    
}
