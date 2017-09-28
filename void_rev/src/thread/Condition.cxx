#include "thread.h"
#include <errno.h>

Mutex & Condition::GetMutex() const
{
    return m_mutex;
}


Condition::Condition(Mutex& mutex) :m_mutex(mutex), m_ready(false)
{

}

Condition::~Condition()
{
}

void Condition::Wait()
{
  std::unique_lock<std::mutex> lk(m_mutex);
  m_cv.wait(lk, [this]{return !m_ready;});
}

void Condition::Signal()
{
  std::lock_guard<std::mutex> lk(m_mutex);
  m_ready = true;

  m_cv.notify_one();
}


void Condition::Broadcast()
{
  std::lock_guard<std::mutex> lk(m_mutex);
  m_ready = true;

  m_cv.notify_all();
}
