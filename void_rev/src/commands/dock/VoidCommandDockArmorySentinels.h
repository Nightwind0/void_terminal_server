

#ifndef VOID_COMMAND_DOCK_ARMORY_SENTINELS_H
#define VOID_COMMAND_DOCK_ARMORY_SENTINELS_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "VoidCommandPurchaseShipItem.h"

using std::string;

class VoidCommandDockArmorySentinels : public VoidCommandPurchaseShipItem
{
 public:
    VoidCommandDockArmorySentinels(VoidServerThread *thread);
    virtual ~VoidCommandDockArmorySentinels();


    
 private:
    
    std::string GetItemName()const;
    int GetMaxOfItem(ShipTypeHandle *shiptype)const;
    int GetCurrentOfItem(ShipHandle * shiptype)const;
    void SetItemNumber(ShipHandle *ship,int num);
    int GetPriceOfItem()const;

    VoidCommandDockArmorySentinels();

};
    
#endif
