#include "VoidCommandBeam.h"
#include "ShipListBehavior.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <sstream>

using std::ostringstream;
using std::left;
using std::right;

VoidCommandBeam::VoidCommandBeam(VoidServerThread *thread):VoidCommand(thread),ShipListBehavior(thread)
{
}
VoidCommandBeam::~VoidCommandBeam()
{
}

string VoidCommandBeam::GetCommandString()const
{
    return "Beam";
}

string VoidCommandBeam::GetDescription()const
{
    return "Beam to a ship from a list, or specify a ship number.";
}

string VoidCommandBeam::GetSyntax()const
{
    return "beam or beam <ship number>";
}

    
bool VoidCommandBeam::ClaimCommand(const string &command)
{
    if (CompStrings(command,"beam")) return true;

    return false;
}

bool VoidCommandBeam::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandBeam(arguments);
}




std::list<int> VoidCommandBeam::GetValidShipList(int cur_sector, int beamrange, int cur_ship)
{

    std::list<int> ships  = GetOwnedShips();

    
    std::list<int> avoids;
    std::list<int> valid_ships;

    for(std::list<int>::iterator iter = ships.begin();
	iter != ships.end();
	iter++)
    {
	if(*iter == cur_ship) continue;
	
	Integer nkey(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(*iter));
	PrimaryKey key(&nkey);
	ShipHandle shiph(get_thread()->GetDBConn(), key);

	ShipTypeHandlePtr shiptype = shiph.GetShipTypeHandle();

	int sector = (int)shiph.GetSector();


	std::deque<int> path = Universe::GetFlightPath(avoids, cur_sector, sector);

	if(path.size() <= beamrange)
	{

	    valid_ships.push_back(*iter);
	}
    }

    return valid_ships;

}



// Return false for command failure (bad arguments..)
bool VoidCommandBeam::CommandBeam(const std::string &arguments)
{

    ShipHandlePtr ship = create_handle_to_current_ship(get_player());

    ShipTypeHandlePtr shiptype = ship->GetShipTypeHandle();
    PlayerHandlePtr player = get_thread()->GetPlayer();

    std::list<int> valid_ships = GetValidShipList((int)ship->GetSector(), (int)shiptype->GetTransRange(), (int)ship->GetNkey());

    if(!valid_ships.size())
    {
	Send(Color()->get(RED) + "There are no ships you can currently beam to from this ship.");

	return true;
    }

    int shipdestnum;

    if(!arguments.size())
    {
	ShowShipList(valid_ships);

	
	bool done = false;
	
	while(!done)
	{
	    
	    Send(Color()->get(GREEN) + "Which Ship Number (-1 to cancel): " + Color()->get(WHITE));
	    std::string sshipnum = ReceiveLine();
	    
	    shipdestnum = atoi(sshipnum.c_str());

	    if(shipdestnum < 0)
	    {
		return true;
	    }

	    if(find(valid_ships.begin(),valid_ships.end(), shipdestnum) == valid_ships.end())
	    {
		Send(Color()->get(RED) + endr +  "That is not a valid ship number." + endr);
	
	    }
	    else
	    {
		done = true;
	    }
	}
    }
    else
    {

	
	shipdestnum = atoi(arguments.c_str());
	
	if(find(valid_ships.begin(),valid_ships.end(), shipdestnum) == valid_ships.end())
	{
	    Send(Color()->get(RED));
	    get_thread()->SendWordWrapped(std::string(endr) + "That is not a valid ship to beam to. Remember that in order to beam"
					  " to a ship, you must be within your ships beam range of it, and it must be owned by you." + endr,80);
	    
	    return true;
	}
    }


    Integer tow = ship->GetTow();

    if(!tow.IsNull())
    {
	ship->Lock();
	ship->BreakTow();
	ship->Unlock();
	Send(Color()->get(GREEN) + "The tow on your ship disengages as you leave." + endr);
    }

    player->Lock();
    player->SetCurrentShip(shipdestnum);
    player->Unlock();




    Send(Color()->get(GREEN));
    get_thread()->SendWordWrapped(std::string(endr) + "You have beamed successfully to another ship." + endr,80);

    return true;
}
