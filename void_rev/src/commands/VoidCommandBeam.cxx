#include "VoidCommandBeam.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <sstream>

using std::ostringstream;
using std::left;
using std::right;

VoidCommandBeam::VoidCommandBeam(VoidServerThread *thread):VoidCommand(thread)
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

std::list<int> VoidCommandBeam::GetOwnedShips()
{
    PlayerHandle * player = get_thread()->GetPlayer();

    std::string query = "select nkey from ship where kowner = '" + (std::string)player->GetName() + "';";

    PGresult *dbresult = get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	
	DBException e(std::string("Get Owned Ships: ") +  PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	throw e;
    }

    int numships = PQntuples(dbresult);

    std::list<int> ships;

    for(int i=0;i< numships; i++)
    {
	ships.push_back( atoi(PQgetvalue(dbresult,i,0)));
    }

    PQclear(dbresult);

    return ships;
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

	ShipTypeHandle shiptype = shiph.GetShipTypeHandle();

	int sector = (int)shiph.GetSector();


	std::deque<int> path = Universe::GetFlightPath(avoids, cur_sector, sector);

	if(path.size() <= beamrange)
	{

	    valid_ships.push_back(*iter);
	}
    }

    return valid_ships;

}


void VoidCommandBeam::ShowValidShipList(const std::list<int> &ships)
{
 
   
    ostringstream os;
    os << Color()->get(BROWN,BG_WHITE) << "                    Ship List                    " << Color()->blackout() << endr;
    os << Color()->get(LIGHTGREEN);
    os.fill(' ');
    os.width(10);
    os << left << "Ship#";
    os.width(30);
    os << left << "Ship";
    os.width(30);
    os << left << "Ship Type";
    os.width(6);
    os << left << "Sector";
    os << endr;

   

    for(std::list<int>::const_iterator iter = ships.begin();
	iter != ships.end();
	iter++)
    {

	Integer nkey(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(*iter));
	PrimaryKey key(&nkey);
	ShipHandle shiph(get_thread()->GetDBConn(), key);

	ShipTypeHandle shiptype = shiph.GetShipTypeHandle();

	int sector = (int)shiph.GetSector();

	
	FGColor fgcolor = (FGColor)((int)shiptype.GetForeColor());
	BGColor bgcolor = (BGColor)((int)shiptype.GetBackColor());
	

	os << Color()->get(WHITE);
	os.width(10);
	os << left <<  *iter;
	os << Color()->get(LIGHTBLUE);
	os.width(30);
	os << left << (std::string)shiph.GetName();
	os << Color()->get(fgcolor,bgcolor);
	os.width(30);
	os << left << (std::string)shiptype.GetShipTypeName(Color());
	os << Color()->get(WHITE,BG_BLACK);
	os.width(6);
	os << right << sector;
	os << endr;	
    }
    os << endr;

    Send(os.str());

}

// Return false for command failure (bad arguments..)
bool VoidCommandBeam::CommandBeam(const std::string &arguments)
{

    ShipHandle * ship = create_handle_to_current_ship(get_player());

    ShipTypeHandle shiptype = ship->GetShipTypeHandle();
    PlayerHandle * player = get_thread()->GetPlayer();

    std::list<int> valid_ships = GetValidShipList((int)ship->GetSector(), (int)shiptype.GetTransRange(), (int)ship->GetNkey());

    if(!valid_ships.size())
    {
	Send(Color()->get(RED) + "There are no ships you can currently beam to from this ship.");

	delete ship;
	return true;
    }

    int shipdestnum;

    if(!arguments.size())
    {
	ShowValidShipList(valid_ships);

	
	bool done = false;
	
	while(!done)
	{
	    
	    Send(Color()->get(GREEN) + "Which Ship Number (-1 to cancel): " + Color()->get(WHITE));
	    std::string sshipnum = ReceiveLine();
	    
	    shipdestnum = atoi(sshipnum.c_str());

	    if(shipdestnum < 0)
	    {
		delete ship;
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



    
    delete ship;
    return true;
    
}
