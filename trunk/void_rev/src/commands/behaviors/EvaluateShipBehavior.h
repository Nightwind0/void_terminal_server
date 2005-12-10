#ifndef VOID_EVALSHIP_BEHAVIOR_H
#define VOID_EVALSHIP_BEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"
#include "Behavior.h"

class EvaluateShipBehavior : public Behavior
{
 public:
    EvaluateShipBehavior(VoidServerThread *t);
    virtual ~EvaluateShipBehavior();

    int EvaluateShip(int shipnum);

 private:

};

#endif 
