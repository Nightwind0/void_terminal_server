#include "VoidCommandClaim.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <sstream>
#include "ResourceMaster.h"

using std::ostringstream;
using std::left;
using std::right;

VoidCommandClaim::VoidCommandClaim(VoidServerThread *thread):VoidCommand(thread),m_combat_tools(thread->GetDatabaseConn(),thread->GetLocalSocket()),m_ship_tools(thread->GetDatabaseConn()),m_comm_tools(thread->GetDatabaseConn(),thread->GetLocalSocket())
{
}
VoidCommandClaim::~VoidCommandClaim()
{
}

string VoidCommandClaim::GetCommandString()const
{
    return "Claim";
}

string VoidCommandClaim::GetDescription()const
{
    return "Claim another ship in this sector by boarding it.";
}

string VoidCommandClaim::GetSyntax()const
{
    return "claim <ship number>";
}

    
bool VoidCommandClaim::ClaimCommand(const string &command)
{
    if (CompStrings(command,"claim")) return true;

    return false;
}

bool VoidCommandClaim::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandClaim(arguments);
}



std::list<int> VoidCommandClaim::GetValidShipList(int cur_sector,  const std::string &player)
{

    std::list<int> valid_ships;

    std::ostringstream query;

    /// @todo replace t.nkey != 10 with value from config table for the escape pod type
    /// @todo consider replacing the hardcoded 10% shields with a config lookup
    query << "select s.nkey from ship s, shiptype t, player p, sectors sec where sec.nsector = '" << cur_sector << "' and (sec.kterritory != 0 or kterritory is null) and ";
    query << "s.kowner != '" << player << "' and ";
    query << "s.ktype = t.nkey and t.nkey != '10' and p.sname = '" << player;
    query << "' and s.ksector = '" << cur_sector << "' and s.nkey not in (select ktowship from ship where ktowship = s.nkey) ";
    query << "and ((s.nshields < (t.nmaxshields / 10) or s.nshields is null) ";
    query << "or (p.kalliance = s.kalliance and s.ballianceowned = TRUE and s.nkey not in (select kcurrentship from player where kcurrentship = s.nkey )));";


    pqxx::result dbresult = get_thread()->DBExec(query.str());


    for(auto row : dbresult)
    {
      valid_ships.push_back(row[0].as<int>());
    }


    return valid_ships;

}

// Return false for command failure (bad arguments..)
bool VoidCommandClaim::CommandClaim(const std::string &arguments)
{

    static std::string rules = 	std::string("Remember that in order to claim a ship, it's cannot be under tow, and it must either "
					    "have less than 10% of it's shields intact, or be owned by your alliance and not currently occupied." + std::string(endr) +
					    "You may not claim escape pods." + std::string(endr) + "You may not claim in federation territory." + std::string(endr));


    std::string playername = get_thread()->GetPlayer()->GetName();

    unsigned int shipdestnum = atoi(arguments.c_str());

    if(!arguments.size() || (shipdestnum == 0 && arguments != "0"))
    {
	return false;
    }

    ShipHandlePtr ship = create_handle_to_current_ship(get_player());
    PlayerHandlePtr player = get_thread()->GetPlayer();

    int sector = (int)ship->GetSector();

    std::list<int> valid_ships = GetValidShipList(sector, playername);

    if(!valid_ships.size())
    {
	Send(Color()->get(RED) + "There are no ships that you can claim in this sector." + Color()->get(GREEN) + endr);
	get_thread()->SendWordWrapped(rules,80);

	return true;
    }

  	
    
    
    if(find(valid_ships.begin(),valid_ships.end(), shipdestnum) == valid_ships.end())
    {

	Send(Color()->get(RED) + "You cannot claim that ship." + Color()->get(GREEN) + endr);
	get_thread()->SendWordWrapped(rules,80);

	return true;
    }

    
    bool occupied = false;
    std::string occupation = "select sname from player where kcurrentship = '" + IntToString(shipdestnum) + "';";

    pqxx::result dbresult = get_thread()->DBExec(occupation);

    std::string oplayer;
    
    if(dbresult.size() >0)
    {
	occupied = true;
	oplayer = dbresult[0][0].as<std::string>();
    }


    Send(Color()->get(LIGHTBLUE) + "You board the ship." + endr);

    if(occupied)
    {
	ShipHandlePtr shiph = m_combat_tools.CreateEscapePodForPlayer(oplayer,sector);

	m_combat_tools.MoveShipRandomly(shiph);
    }


    ShipHandlePtr oshiph = ShipHandle::HandleFromNkey(get_thread()->GetDatabaseConn(),shipdestnum);

    

    oshiph->Lock();
    oshiph->SetOwner(playername);
    oshiph->Unlock();
    

    std::string shipname = oshiph->GetName();

    Event claimevent(Event::SHIPCLAIMED);
    claimevent.SetActor(playername);
    if(occupied) claimevent.SetSubject(oplayer); // We display who was forced out if anyone was
    claimevent.SetShipName(shipname);
    
    m_comm_tools.LogEvent(claimevent);

    ResourceMaster::GetInstance()->SendSystemMail(oplayer, playername + " boarded and claimed " + shipname + "!" + endr);
    m_comm_tools.SendMsgToSector(playername + " boards " + shipname + endr, sector, playername);

    if(occupied)
    {
	player->Lock();
	/// @todo get point value from config table
	player->SetPoints(player->GetPoints() + 500);
	player->Unlock();

	ResourceMaster::GetInstance()->SendSystemMail(oplayer, playername + " forced you off " + shipname + " into an escape pod!" + endr);
	m_comm_tools.SendMsgToSector(playername + " forces " + oplayer + " off his ship!", sector,playername);
	Send(Color()->get(PURPLE) + "You force " + Color()->get(LIGHTCYAN) + oplayer + Color()->get(PURPLE) + " into an escape pod." + endr + "The escape pod jettisons off into space." + endr);    
    }

    m_comm_tools.SendMsgToSector(playername + " claims " + shipname + endr, sector,playername);

    Send(Color()->get(GREEN) + "You have taken ownership of this craft." + endr);
    Send(Color()->get(WHITE) + "500 " + Color()->get(GREEN) + " points." + endr);

    return true;
    
}
