#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockArmoryMissiles.h"
#include <sstream>
#include <iostream>

using std::string;

using std::ostringstream;

using std::left;

using std::right;

VoidCommandDockArmoryMissiles::VoidCommandDockArmoryMissiles(VoidServerThread *thread):VoidCommandPurchaseShipItem(thread)
{
}
VoidCommandDockArmoryMissiles::~VoidCommandDockArmoryMissiles()
{
}

std::string VoidCommandDockArmoryMissiles::GetItemName()const
{
    return "missiles";
}
int VoidCommandDockArmoryMissiles::GetMaxOfItem(ShipTypeHandlePtr shiptype)const
{
    return shiptype->GetMaxMissiles();
}
int VoidCommandDockArmoryMissiles::GetCurrentOfItem(ShipHandlePtr ship)const
{
    return ship->GetMissiles();
}
void VoidCommandDockArmoryMissiles::SetItemNumber(ShipHandlePtr ship,int num)
{
    ship->Lock();
    ship->SetMissiles(num);
    ship->Unlock();
}
int VoidCommandDockArmoryMissiles::GetPriceOfItem()const
{
    return atoi(get_config("missile_price").c_str());
}
