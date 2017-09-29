

#ifndef VOID_COMMAND_DOCK_SHIPYARD_HOLDS_H
#define VOID_COMMAND_DOCK_SHIPYARD_HOLDS_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "VoidCommandPurchaseShipItem.h"

using std::string;

class VoidCommandDockShipYardHolds : public VoidCommandPurchaseShipItem
{
 public:
    VoidCommandDockShipYardHolds(VoidServerThread *thread);
    virtual ~VoidCommandDockShipYardHolds();


    
 private:
    
    std::string GetItemName()const;
    int GetMaxOfItem(ShipTypeHandlePtr shiptype)const;
    int GetCurrentOfItem(ShipHandlePtr shiptype)const;
    void SetItemNumber(ShipHandlePtr ship,int num);
    int GetPriceOfItem()const;

    VoidCommandDockShipYardHolds();

};
    
#endif
