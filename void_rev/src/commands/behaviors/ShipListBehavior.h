#ifndef VOID_SHIPLIST_BEHAVIOR_H
#define VOID_SHIPLIST_BEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"
#include <list>

class ShipListBehavior
{
 public:
    ShipListBehavior(VoidServerThread *t);
    ~ShipListBehavior();
 protected:
    std::list<int> GetOwnedShips();
    void ShowShipList(const std::list<int> &ships);
 private:
    VoidServerThread *m_thread;
};

#endif //VOID_ESCAPEPOD_BEHAVIOR_H
