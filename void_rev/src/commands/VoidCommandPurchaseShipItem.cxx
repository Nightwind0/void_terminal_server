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

VoidCommandPurchaseShipItem::VoidCommandPurchaseShipItem(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandPurchaseShipItem::~VoidCommandPurchaseShipItem()
{
}

string VoidCommandPurchaseShipItem::GetCommandString()const
{
    return GetItemName();
}
string VoidCommandPurchaseShipItem::GetDescription()const
{
    return "buy " + GetItemName() + " or check " + GetItemName() +" status";
}
string VoidCommandPurchaseShipItem::GetSyntax()const
{
    return GetItemName() + " <amount> or " + GetItemName();
}
    
bool VoidCommandPurchaseShipItem::ClaimCommand(const string &command)
{
    if(CompStrings(command,GetItemName()))
	return true;
    else return false;
}

bool VoidCommandPurchaseShipItem::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    return PurchaseShipItem(arguments);

}

void VoidCommandPurchaseShipItem::ShowItemStatus()
{
    ShipHandle * ship = create_handle_to_current_ship(get_thread()->GetPlayer());

    ShipTypeHandle shiptype = ship->GetShipTypeHandle();

    ostringstream os;

    os << endr << Color()->get(GREEN);
    os << "Your ship has " << Color()->get(WHITE) << GetCurrentOfItem(ship);
    os << Color()->get(GREEN) << ' ' << GetItemName() << " out of ";
    os << Color()->get(WHITE) << GetMaxOfItem(&shiptype);
    os << Color()->get(GREEN) << " maximum." << endr;
    os << "Leaving room for " ;
    os << Color()->get(LIGHTCYAN) << GetMaxOfItem(&shiptype) - GetCurrentOfItem(ship);
    os << Color()->get(GREEN) << " more." << endr;

    int playercredits = (int)get_thread()->GetPlayer()->GetCredits();

    int afford = playercredits / GetPriceOfItem(); // TODO: Grab missile price from config table

    os << endr <<  GetItemName()  << " cost " <<  Color()->get(WHITE) << GetPriceOfItem() << Color()->get(GREEN) <<  " each." << endr;
    
    os << "With your " << Color()->get(WHITE) << playercredits;
    os << Color()->get(GREEN) << " credits, you could afford ";
    os << Color()->get(LIGHTCYAN) << afford ;
    os << Color()->get(GREEN) << ' ' << GetItemName()  << endr << endr;

    Send(os.str());
    
    delete ship;
}
    

bool VoidCommandPurchaseShipItem::PurchaseShipItem(const string &arguments)
{

    PlayerHandle * player = get_thread()->GetPlayer();


    if(!arguments.size())
    {
	ShowItemStatus();
	return true;
    }

    int nummissiles = atoi(arguments.c_str());

    if(nummissiles <0)
    {
	Send(Color()->get(RED) + "A positive number, wise guy." + endr);

	return true;
    }

    ShipHandle * ship = create_handle_to_current_ship(get_thread()->GetPlayer());
    ShipTypeHandle shiptype = ship->GetShipTypeHandle();

    int playercredits = (int)get_thread()->GetPlayer()->GetCredits();
    int afford = playercredits / GetPriceOfItem(); // TODO: Grab missile price from config table

    int maxmissiles = GetMaxOfItem(&shiptype) - GetCurrentOfItem(ship);


    if(afford <= 0)
    {
	Send(Color()->get(RED) + "You can't afford any " + GetItemName() + endr);
	delete ship;

	return true;
    }

    if(maxmissiles <= 0)
    {
	Send(Color()->get(RED) + "You don't have room for more " + GetItemName() + " in your ship." + endr);
	delete ship;

	return true;
    }

    if(nummissiles > maxmissiles)
    {
	Send(Color()->get(GREEN) + "You only have room for " + Color()->get(WHITE) + IntToString(maxmissiles) 
	     + Color()->get(GREEN) + " more " + GetItemName() +". Is that okay? (Y/n) : " );

	std::string accept = ReceiveLine();
	LOWERCASE(accept);

	if(CompStrings(accept,"yes"))
	{
	    nummissiles = maxmissiles;
	}
	else
	{
	    delete ship;
	    return true;
	}
    }


    if(nummissiles > afford)
    {
	Send(Color()->get(GREEN) + "You only have enough money for " + Color()->get(WHITE) + IntToString(afford)
	     + Color()->get(GREEN) + " more " + GetItemName() +". Is that okay? (Y/n) : ");

	std::string accept = ReceiveLine();
	LOWERCASE(accept);

	if(CompStrings(accept,"yes"))
	{
	    nummissiles = afford;
	}
	else
	{
	    delete ship;
	    return true;
	}
    }

    int totalprice = 100 * nummissiles;

    Send(Color()->get(GREEN) + endr + "Total comes to "  + Color()->get(WHITE) +  IntToString(totalprice) +
	 Color()->get(GREEN) + " credits. Still interested? (Y/n) : ");

    std::string proceed = ReceiveLine();

    if(CompStrings(proceed,"yes"))
    {
	player->Lock();
	player->SetCredits(  player->GetCredits() - totalprice );
	player->Unlock();


	SetItemNumber(ship,GetCurrentOfItem(ship)+ nummissiles);


	Send(Color()->get(GREEN) + endr + "Thank you, your " +  GetItemName() + " were loaded onto your ship." + endr);

	delete ship;

	return true;

    }
    else
    {
	delete ship;
	return true;
    }


    return true;
    
}

 
