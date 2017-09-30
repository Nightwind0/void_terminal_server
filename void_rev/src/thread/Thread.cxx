#include "thread.h"
#include <iostream>

void *ThreadStartRoutine(Thread *pthread)
{


    pthread->setRunning(true);

    bool thread_init  = false;
    try {
      thread_init = pthread->thread_init();
      if(!thread_init) {
	return NULL;
      }
      
      pthread->start();
      pthread->run();
    }catch(...) {
      // Swallow it for now
    }
      
    pthread->thread_destroy();
    pthread->end();
    
    
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
    m_mutex.lock();
}
Thread::Thread()
{
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
  m_thread = std::thread(ThreadStartRoutine, this); 
}
void Thread::Wait()
{
    m_mutex.lock();
    m_mutex.unlock();
}



void Thread::end()
{
    m_mutex.unlock();
}

