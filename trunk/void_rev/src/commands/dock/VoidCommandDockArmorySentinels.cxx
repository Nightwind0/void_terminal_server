#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockArmorySentinels.h"
#include <sstream>
#include <iostream>

using std::string;

using std::ostringstream;

using std::left;

using std::right;

VoidCommandDockArmorySentinels::VoidCommandDockArmorySentinels(VoidServerThread *thread):VoidCommandPurchaseShipItem(thread)
{
}
VoidCommandDockArmorySentinels::~VoidCommandDockArmorySentinels()
{
}

std::string VoidCommandDockArmorySentinels::GetItemName()const
{
    return "sentinels";
}
int VoidCommandDockArmorySentinels::GetMaxOfItem(ShipTypeHandle *shiptype)const
{
    return shiptype->GetMaxSentinels();
}
int VoidCommandDockArmorySentinels::GetCurrentOfItem(ShipHandle * ship)const
{
    return ship->GetSentinels();
}
void VoidCommandDockArmorySentinels::SetItemNumber(ShipHandle *ship,int num)
{
    ship->Lock();
    ship->SetSentinels(num);
    ship->Unlock();
}
int VoidCommandDockArmorySentinels::GetPriceOfItem()const
{
    return 150; // TODO: Get value from table
}
