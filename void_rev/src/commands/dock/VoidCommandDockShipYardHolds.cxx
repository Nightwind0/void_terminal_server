#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockShipYardHolds.h"
#include <sstream>
#include <iostream>

using std::string;

using std::ostringstream;

using std::left;

using std::right;

VoidCommandDockShipYardHolds::VoidCommandDockShipYardHolds(VoidServerThread *thread):VoidCommandPurchaseShipItem(thread)
{
}
VoidCommandDockShipYardHolds::~VoidCommandDockShipYardHolds()
{
}

std::string VoidCommandDockShipYardHolds::GetItemName()const
{
    return "holds";
}
int VoidCommandDockShipYardHolds::GetMaxOfItem(ShipTypeHandlePtr shiptype)const
{
    return shiptype->GetMaxHolds();
}
int VoidCommandDockShipYardHolds::GetCurrentOfItem(ShipHandlePtr ship)const
{
    return ship->GetHolds();
}
void VoidCommandDockShipYardHolds::SetItemNumber(ShipHandlePtr ship,int num)
{
    ship->Lock();
    ship->SetHolds(num);
    ship->Unlock();
}
int VoidCommandDockShipYardHolds::GetPriceOfItem()const
{
    return atoi(get_config("hold_price").c_str());
}
