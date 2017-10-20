#include "VoidCommandMove.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"


VoidCommandMove::VoidCommandMove(VoidServerThread *thread):VoidCommandDisplay(thread)
{
}
VoidCommandMove::~VoidCommandMove()
{
}

string VoidCommandMove::GetCommandString()const
{
    return "Move";
}

string VoidCommandMove::GetDescription()const
{
    return "Move to the specified sector. Default is zero.";
}

string VoidCommandMove::GetSyntax()const
{
    return "move <sector>";
}

    
bool VoidCommandMove::ClaimCommand(const string &command)
{
    if (CompStrings(command,"move")) return true;

    return false;
}

bool VoidCommandMove::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandMove(arguments);
}

// Return false for command failure (bad arguments..)
bool VoidCommandMove::CommandMove(const std::string &arguments)
{
    std::ostringstream os;
    int sec = atoi(arguments.c_str());

    ShipHandlePtr ship = create_handle_to_current_ship(get_player());

    if(ship->GetSector() == sec)
    {
	//Already there
	return true;
    }

    if( ship->GetIsCloaked())
    {
	Send(Color()->get(RED) + "You cannot move while your ship is cloaked." + endr);

	return true;
    }


    if(sec <0 || sec >= Universe::GetNumSectors(*get_thread()->GetDatabaseConn()))
    {
	os << Color()->get(LIGHTRED) << "Sector " << Color()->get(RED) << sec << Color()->get(LIGHTRED) << " does not exist." << endr;
	get_thread()->Send(os.str());

	return true;
    }

    std::vector<Sector> sectors = Universe::GetAdjacentSectors(ship->GetSector());
    
    
    std::vector<Sector>::iterator f = find(sectors.begin(),sectors.end(), sec);
    
    if(f == sectors.end())
    {

	get_thread()->Send(Color()->get(GRAY) + "Calculating flight path..." + endr);

	std::set<Sector> avoids;
	std::deque<Sector> path = Universe::GetFlightPath(avoids, ship->GetSector(), sec);

	os << Color()->get(BROWN) << "Flight path:" << endr;

	for(std::deque<Sector>::iterator i = path.begin();
	    i != path.end();
	    i++)
	{
	    os << Color()->get(LIGHTPURPLE) << '<' << Color()->get(GRAY) << *i << Color()->get(LIGHTPURPLE) <<  '>' ;    
	}

	os << endr;

	get_thread()->Send(os.str());

	get_thread()->Send(Color()->get(GREEN) + "Proceed? (Y/n/x)" + Color()->get(WHITE));
	
	std::string answer = get_thread()->ReceiveLine();

	if(CompStrings(answer, "yes") || CompStrings(answer, "YES"))
	{
	    bool done = false;
	    while(!path.empty() && !done)
	    {
		int ns = path.front();
		path.pop_front();
		
		if(!move_player_to_sector(ns) && !path.empty()) {
		  Send(endr);
		  Send("Continue? (Y/n): ");
		  std::string proceed = get_thread()->ReceiveLine();
		  if(CompStrings(proceed, "yes")){
		    done = false;
		  } else {
		    done = true;
		  }
		}
		
	    }
	}
	else if(CompStrings(answer, "x"))
	{
	    while(!path.empty())
	    {
		int ns = path.front();
		path.pop_front();
		
		move_player_to_sector(ns);
	    }
	}

	return true;
	
    }
    else
    {
	move_player_to_sector(sec);
    }
    

    return true;
}


bool VoidCommandMove::move_player_to_sector(Sector sector){
  //  ShipHandlePtr ship = create_handle_to_current_ship(get_player());
  // ShipTypeHandlePtr ship_type = ship.GetShipTypeHandle();
  bool stop = false;
  std::string sector_data = DisplaySector(sector, false, stop);
  VoidCommand::move_player_to_sector(sector);
  Send(sector_data);
  return !stop;
}
