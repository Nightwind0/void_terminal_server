#ifndef THREAD_H
#define THREAD_H


#include <pthread.h>
#include <string>
#include <thread>
#include "Condition.h"


class ThreadException : public PThreadException
{
 public:
    enum ErrorType { BADTHREADCREATE, UNKNOWN };
    
    ThreadException(ErrorType type = UNKNOWN);
    
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

    bool isRunning() const { return m_bRunning; }

    void  Wait();	
    protected:
    void setRunning(bool running);
    virtual bool thread_init()=0;
    virtual void thread_destroy()=0;
    virtual bool run()=0;
    void start();  



    private:
    std::thread m_thread;
//    Condition m_condition;
    std::mutex m_mutex;
    void end();
    bool m_bRunning;



    friend void * ThreadStartRoutine(Thread * pThread);
};


void * ThreadStartRoutine(Thread *pThread);


#endif

