#ifndef THREAD_H
#define THREAD_H


#include <pthread.h>
#include <string>
#include "Condition.h"
#include "Mutex.h"

//class Mutex;


class ThreadException : public PThreadException
{
 public:
    enum ErrorType { BADTHREADCREATE, UNKNOWN };
    
    ThreadException(ErrorType type = UNKNOWN);
    
    // Copy Constructor
    ~ThreadException();
    
    ErrorType GetType() const { return m_error; }
   

    
    
 private:
    
    ErrorType m_error;
   

	        
};






class Thread
{
    public:
    Thread();
    virtual ~Thread();
    
    void Start();
    void  Wait();

    bool isRunning() const { return m_bRunning; }
	
    protected:
    void setRunning(bool running);
    virtual bool thread_init()=0;
    virtual void thread_destroy()=0;
    virtual bool run()=0;
    
    private:
    pthread_t m_thread;
    Condition m_condition;
    ErrChkMutex m_mutex;
    void end();
    bool m_bRunning;


    friend void * ThreadStartRoutine(Thread * pThread);
};


void * ThreadStartRoutine(Thread *pThread);


#endif

