#include "VoidCommandTow.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <sstream>

using std::ostringstream;
using std::left;
using std::right;

VoidCommandTow::VoidCommandTow(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandTow::~VoidCommandTow()
{
}

string VoidCommandTow::GetCommandString()const
{
    return "Tow";
}

string VoidCommandTow::GetDescription()const
{
    return "Tow a ship or stop towing.";
}

string VoidCommandTow::GetSyntax()const
{
    return "tow <ship number> or tow";
}

    
bool VoidCommandTow::ClaimCommand(const string &command)
{
    if (CompStrings(command,"tow")) return true;

    return false;
}

bool VoidCommandTow::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandTow(arguments);
}


std::list<int> VoidCommandTow::GetValidShipList(int cur_sector, int cur_ship)
{
    PlayerHandlePtr player = get_thread()->GetPlayer();

    std::list<int> valid_ships;

    std::string query = "select nkey from ship where ksector = '" + IntToString(cur_sector)
	+ "' and kowner = '" + player->GetName().GetAsString() + "' and nkey != '" + IntToString(cur_ship) + "' and (bcloaked != 'T' or bcloaked is null);";

    pqxx::result dbresult = get_thread()->DBExec(query);
    int numships = dbresult.size();

    for(auto row : dbresult)
    {
      valid_ships.push_back(row[0].as<int>());
    }


    return valid_ships;

}


// Return false for command failure (bad arguments..)
bool VoidCommandTow::CommandTow(const std::string &arguments)
{

    ShipHandlePtr ship = create_handle_to_current_ship(get_player());
    ShipTypeHandlePtr shiptype = ship->GetShipTypeHandle();

    int tps = (int)shiptype->GetTurnsPerSector();

//    PlayerHandle * player = get_thread()->GetPlayer();

    std::list<int> valid_ships = GetValidShipList((int)ship->GetSector(), (int)ship->GetNkey());

    if(!valid_ships.size())
    {
	Send(Color()->get(RED) + "There are no ships that you can tow in this sector.");

	return true;
    }

    int shipdestnum;

    if(!arguments.size())
    {
	ship->Lock();
	ship->BreakTow();
	ship->Unlock();
	Send(Color()->get(GREEN) + "You have disengaged your tow" + endr);

	return true;
    }
    else
    {

	
	shipdestnum = atoi(arguments.c_str());
	
	if(find(valid_ships.begin(),valid_ships.end(), shipdestnum) == valid_ships.end())
	{
	    Send(Color()->get(RED));
	    get_thread()->SendWordWrapped(std::string(endr) + "That is not a valid ship to tow. Remember that in order to tow"
					  " a ship, you must be in the same sector, and it must be owned by you." + endr,80);
	 

	    return true;
	}
    }

    ship->Lock();
    ship->SetTow(shipdestnum);
    ship->Unlock();


    Send(Color()->get(GREEN) + std::string(endr) + "You are now towing." + endr);
    
    std::shared_ptr<Integer> towship = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(shipdestnum));
    PrimaryKey key(towship);
    ShipHandle towshiphandle(get_thread()->GetDatabaseConn(),key);
    ShipTypeHandlePtr tsthandle= towshiphandle.GetShipTypeHandle();

    tps += tsthandle->GetTurnsPerSector();
    
    Send(Color()->get(GREEN) + "You now take " + Color()->get(WHITE) + IntToString(tps) + Color()->get(GREEN) + " turns per sector." + endr);


    return true;
    
}
