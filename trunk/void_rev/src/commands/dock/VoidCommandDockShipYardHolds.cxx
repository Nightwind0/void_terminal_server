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
int VoidCommandDockShipYardHolds::GetMaxOfItem(ShipTypeHandle *shiptype)const
{
    return shiptype->GetMaxHolds();
}
int VoidCommandDockShipYardHolds::GetCurrentOfItem(ShipHandle * ship)const
{
    return ship->GetHolds();
}
void VoidCommandDockShipYardHolds::SetItemNumber(ShipHandle *ship,int num)
{
    ship->Lock();
    ship->SetHolds(num);
    ship->Unlock();
}
int VoidCommandDockShipYardHolds::GetPriceOfItem()const
{
    return 100; // TODO: Get value from table
}
