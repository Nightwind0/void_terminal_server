#include "VoidCommandComputerPlayers.h"
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

VoidCommandComputerPlayers::VoidCommandComputerPlayers(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandComputerPlayers::~VoidCommandComputerPlayers()
{
}

string VoidCommandComputerPlayers::GetCommandString()const
{
    return "Players";
}

string VoidCommandComputerPlayers::GetDescription()const
{
    return "view a ranked list of players in the game.";
}

string VoidCommandComputerPlayers::GetSyntax()const
{
    return "players";
}

    
bool VoidCommandComputerPlayers::ClaimCommand(const string &command)
{
    if (CompStrings(command,"players")) return true;

    return false;
}

bool VoidCommandComputerPlayers::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandComputerPlayers(arguments);
}




// Return false for command failure (bad arguments..)
bool VoidCommandComputerPlayers::CommandComputerPlayers(const std::string &arguments)
{


    std::string query = "select player.sname, player.npoints, shipmanufacturer.sname, shiptype.sname, shiptype.nforecolor,shiptype.nbackcolor, player.kalliance from player left outer join ship on (ship.nkey = player.kcurrentship)  left outer join shiptype on (ship.ktype = shiptype.nkey) left outer join shipmanufacturer on (shiptype.kmanufacturer = shipmanufacturer.nkey) order by player.npoints desc;";

    PGresult *dbresult = get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	DBException e("Player list error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    ostringstream os;

    os << Color()->get(RED,BG_WHITE);
    os << "                Player List               " << endr;
    os << Color()->get(WHITE);
    os.width(15);
    os << left;
    os << "Player";
    os.width(15);
    os << left;
    os << "Alliance";
    os.width(8);
    os << left;
    os << "Points";
    os.width(26);
    os <<left;
    os << "Current Ship Type";
    os << Color()->blackout();
    os << endr;

    int numplayers = PQntuples(dbresult);

    for(int i=0;i<numplayers;i++)
    {
	os << Color()->get(LIGHTBLUE);
	os.width(15);
	os << left;
	os << PQgetvalue(dbresult,i,0);
	os << Color()->get(RED);
	os.width(15);
	os << left;
	os << PQgetvalue(dbresult,i,6);
	os << Color()->get(LIGHTGREEN);
	os.width(8);
	os << left;
	os << PQgetvalue(dbresult,i,1);
      
	FGColor fgcolor = (FGColor)atoi(PQgetvalue(dbresult,i,4));
	BGColor bgcolor = (BGColor)atoi(PQgetvalue(dbresult,i,5));

	os << Color()->get(fgcolor,bgcolor);
	os.width(6);
	os << left;
	os << PQgetvalue(dbresult,i,2);
	os.width(20);
	os << left;
	os << PQgetvalue(dbresult,i,3);
	os << Color()->blackout();
	os << endr;

    }

    PQclear(dbresult);

    Send(os.str());

    return true;
}
