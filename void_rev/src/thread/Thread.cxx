#include "thread.h"


void *ThreadStartRoutine(Thread *pthread)
{


    pthread->setRunning(true);

    pthread->thread_init();

    pthread->run();

    pthread->thread_destroy();
    pthread->end();


    return NULL;
}


PThreadException::PThreadException()
{
}

PThreadException::~PThreadException()
{
}

ThreadException::ThreadException(ErrorType type)
{
    m_error = type;
}

ThreadException::~ThreadException()
{
}

Thread::Thread():m_condition(&m_mutex)
{
    m_mutex.CreateMutex();
    m_bRunning = false;
}
Thread::~Thread()
{
     Wait();
}

void Thread::setRunning(bool running)
{
    m_bRunning = running;
}

void Thread::Start()
{
    int thread_response = pthread_create(&m_thread,NULL, (void*(*)(void*))&(ThreadStartRoutine),this);

    if(thread_response)
       throw ThreadException(ThreadException::BADTHREADCREATE);
}
void Thread::Wait()
{
    m_condition.GetMutex()->Lock();
    while(m_bRunning)
    {
	m_condition.Wait();
    }

    m_condition.GetMutex()->Unlock();
}



void Thread::end()
{
    m_condition.GetMutex()->Lock();

    setRunning(false);
    m_condition.Signal();

    m_condition.GetMutex()->Unlock();
}

