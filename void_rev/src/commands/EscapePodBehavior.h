#ifndef VOID_ESCAPEPOD_BEHAVIOR_H
#define VOID_ESCAPEPOD_BEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"

class EscapePodBehavior
{
 public:
    EscapePodBehavior(VoidServerThread *t);
    ~EscapePodBehavior();
 protected:
    int CreateEscapePodForPlayer(const std::string player);
    void MoveShipRandomly(ShipHandle *ship);

 private:
    VoidServerThread *m_thread;
};

#endif //VOID_ESCAPEPOD_BEHAVIOR_H
