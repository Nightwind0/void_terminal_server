
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include "thread.h"
#include "Semaphore.h"

Semaphore::Semaphore()
{
    int sem_response = sem_init(&m_semaphore, 0, 0);

    if(!sem_response) return;

    if(sem_response == EINVAL)
	throw SemaphoreException(SemaphoreException::SEMUNKNOWN);
    else throw SemaphoreException(SemaphoreException::UNKNOWN);
}


Semaphore::~Semaphore()
{
    while (sem_destroy(&m_semaphore) == EBUSY)
    {
        Post();
    }
}
void Semaphore::Wait()
{
    sem_wait(&m_semaphore);
}

void Semaphore::Post()
{
    int sem_response = sem_post(&m_semaphore);

    if(!sem_response) return;

    if(sem_response == ERANGE)
	throw SemaphoreException(SemaphoreException::RANGE);
    else 
	throw SemaphoreException(SemaphoreException::UNKNOWN);


}

bool Semaphore::Try()
{
    int sem_response = sem_trywait(&m_semaphore);

    if(!sem_response) return true;

    if(sem_response == EAGAIN)
	return false;
    else throw SemaphoreException(SemaphoreException::UNKNOWN);
        
    return true;
}

int Semaphore::GetCount() const
{
    int sem_result;

    sem_getvalue(const_cast<sem_t*>(&m_semaphore), &sem_result);
    return sem_result;
}

