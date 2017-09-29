#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockArmoryShields.h"
#include <sstream>
#include <iostream>

using std::string;

using std::ostringstream;

using std::left;

using std::right;

VoidCommandDockArmoryShields::VoidCommandDockArmoryShields(VoidServerThread *thread):VoidCommandPurchaseShipItem(thread)
{
}
VoidCommandDockArmoryShields::~VoidCommandDockArmoryShields()
{
}

std::string VoidCommandDockArmoryShields::GetItemName()const
{
    return "shields";
}
int VoidCommandDockArmoryShields::GetMaxOfItem(ShipTypeHandlePtr shiptype)const
{
    return shiptype->GetMaxShields();
}
int VoidCommandDockArmoryShields::GetCurrentOfItem(ShipHandlePtr ship)const
{
    return ship->GetShields();
}
void VoidCommandDockArmoryShields::SetItemNumber(ShipHandlePtr ship,int num)
{
    ship->Lock();
    ship->SetShields(num);
    ship->Unlock();
}
int VoidCommandDockArmoryShields::GetPriceOfItem()const
{
    return atoi(get_config("shield_price").c_str());
}
