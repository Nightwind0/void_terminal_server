#ifndef _PLANET_UPDATE_THREAD_H_
#define _PLANET_UPDATE_THREAD_H_
#include "thread.h"
#include "Socket.h"

class PlanetUpdateThread : public Thread {
 public:
  PlanetUpdateThread();
  virtual ~PlanetUpdateThread();
  virtual bool thread_init();
  virtual void thread_destroy();
  virtual bool run();
 private:
  UNIXDatagramSocketPtr m_usocket;
  double CalculatePopulation(double current, double daily_growth, double seconds)const;
  double CalculateResource(double current, double multiplier, double population, double seconds, double divisor)const;


};

#endif
