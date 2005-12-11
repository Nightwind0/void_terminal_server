#include "thread.h"


void *ThreadStartRoutine(Thread *pthread)
{


    pthread->setRunning(true);

    if(pthread->thread_init())
    {
	pthread->start();
	pthread->run();

	pthread->thread_destroy();
	pthread->end();
    }


    return NULL;
}


bool Thread::thread_init(){}
void Thread::thread_destroy(){}
bool Thread::run(){}

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

void Thread::start()
{
    m_mutex.Lock();
}
Thread::Thread()
{
    m_mutex.CreateMutex();
    m_bRunning = false;
}
Thread::~Thread()
{
    
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
    m_mutex.Lock();
    m_mutex.Unlock();
}



void Thread::end()
{
    m_mutex.Unlock();
}

