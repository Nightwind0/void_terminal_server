#ifndef VOID_DELETESHIP_BEHAVIOR_H
#define VOID_DELETESHIP_BEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"

class DeleteShipBehavior
{
 public:
    DeleteShipBehavior(VoidServerThread *t);
    ~DeleteShipBehavior();
 protected:
    void delete_ship(int shipnum);

 private:
    VoidServerThread *m_thread;
};

#endif //VOID_ESCAPEPOD_BEHAVIOR_H
