#ifndef VOID_SHIPLIST_BEHAVIOR_H
#define VOID_SHIPLIST_BEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"
#include <list>
#include "Behavior.h"

class ShipListBehavior : public Behavior
{
 public:
    ShipListBehavior(VoidServerThread *t);
    virtual ~ShipListBehavior();

    std::list<int> GetOwnedShips();
    void ShowShipList(const std::list<int> &ships);
 private:

};

#endif //VOID_ESCAPEPOD_BEHAVIOR_H
