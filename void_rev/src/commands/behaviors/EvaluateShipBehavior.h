#ifndef VOID_EVALSHIP_BEHAVIOR_H
#define VOID_EVALSHIP_BEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"

class EvaluateShipBehavior
{
 public:
    EvaluateShipBehavior(VoidServerThread *t);
    ~EvaluateShipBehavior();
 protected:
    int EvaluateShip(int shipnum);

 private:
    VoidServerThread *m_thread;
};

#endif 
