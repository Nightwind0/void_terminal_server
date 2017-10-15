#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockShipYardSell.h"

using std::string;


VoidCommandDockShipYardSell::VoidCommandDockShipYardSell(VoidServerThread *thread):VoidCommand(thread),EvaluateShipBehavior(thread),m_ship_tools(thread->GetDatabaseConn())
{
}
VoidCommandDockShipYardSell::~VoidCommandDockShipYardSell()
{
}

string VoidCommandDockShipYardSell::GetCommandString()const
{
    return "Sell";
}
string VoidCommandDockShipYardSell::GetDescription()const
{
    return "sell an extra star ship";
}
string VoidCommandDockShipYardSell::GetSyntax()const
{
    return "sell <ship id>";
}
    
bool VoidCommandDockShipYardSell::ClaimCommand(const string &command)
{
    if(CompStrings(command,"sell"))
	return true;
    else return false;
}

bool VoidCommandDockShipYardSell::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    return DockShipYardSell(arguments);

}


std::list<int> VoidCommandDockShipYardSell::GetOwnedEmptyShipsInSector(const std::string &player, int sector)
{
    
    std::list<int> ships;

    std::string query = "select nkey from ship where ksector = '" + IntToString(sector) + "' and kowner = '" + player + "' and nkey not in (select kcurrentship from player where sname = '" + player + "');";

    pqxx::result dbresult = get_thread()->DBExec(query);


    for(auto row : dbresult)
    {
      ships.push_back(row[0].as<int>());
    }

    
    return ships;
}    

bool VoidCommandDockShipYardSell::DockShipYardSell(const string &arguments)
{

    PlayerHandlePtr player = get_thread()->GetPlayer();
    int shipnum = atoi(arguments.c_str());

    if(!arguments.size() || (shipnum == 0 && arguments != "0"))
    {
	return false;
    }

    std::string playername = player->GetName();
    ShipHandlePtr pship = create_handle_to_current_ship(player);
    int sector = pship->GetSector();


    std::list<int> validships = GetOwnedEmptyShipsInSector(playername, sector);

    if(!validships.size())
    {
	Send(Color()->get(RED) + "There are no ships in this sector that you can sell." + endr);
	return true;
    }

    if(find(validships.begin(),validships.end(), shipnum) == validships.end())
    {
	Send(Color()->get(RED));
	get_thread()->SendWordWrapped((std::string)"That is not a valid ship id to sell. In order to sell a ship, it must be in this sector, it must be owned by you, and it must not be your current ship. If you want to sell the ship you came in with, you can buy a new ship and trade your current one in." + endr,80);
	return true;
    }

    int value = EvaluateShip(shipnum);

    bool sellit = false;
    Send(Color()->get(GREEN) + "We will give you " + Color()->get(WHITE) + IntToString(value) + Color()->get(GREEN) + " for it. (Y/n): " );

    std::string sellresponse = ReceiveLine();

    LOWERCASE(sellresponse);

    if(CompStrings(sellresponse,"yes"))
    {
	sellit = true;
    }

    if(sellit)
    {
	m_ship_tools.DeleteShip(shipnum);
	player->Lock();
	player->SetCredits ( player->GetCredits() + value);
	player->Unlock();

	Send(Color()->get(GREEN) + "Thanks, here are your credits." + endr);
    }
    else
    {
	Send(Color()->get(GREEN) + "Okay, come back if you change your mind." + endr);
    }


    return true;
}

 
