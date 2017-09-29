

#ifndef VOID_COMMAND_DOCK_ARMORY_MISSILES_H
#define VOID_COMMAND_DOCK_ARMORY_MISSILES_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "VoidCommandPurchaseShipItem.h"

using std::string;

class VoidCommandDockArmoryMissiles : public VoidCommandPurchaseShipItem
{
 public:
    VoidCommandDockArmoryMissiles(VoidServerThread *thread);
    virtual ~VoidCommandDockArmoryMissiles();


    
 private:
    
    std::string GetItemName()const;
    int GetMaxOfItem(ShipTypeHandlePtr shiptype)const;
    int GetCurrentOfItem(ShipHandlePtr shiptype)const;
    void SetItemNumber(ShipHandlePtr ship,int num);
    int GetPriceOfItem()const;

    VoidCommandDockArmoryMissiles();

};
    
#endif
