#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockShipYardList.h"

using std::string;


VoidCommandDockShipYardList::VoidCommandDockShipYardList(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandDockShipYardList::~VoidCommandDockShipYardList()
{
}

string VoidCommandDockShipYardList::GetCommandString()const
{
    return "List";
}
string VoidCommandDockShipYardList::GetDescription()const
{
    return "show available ships and their numbers";
}
string VoidCommandDockShipYardList::GetSyntax()const
{
    return "list";
}
    
bool VoidCommandDockShipYardList::ClaimCommand(const string &command)
{
    if(CompStrings(command,"list"))
	return true;
    else return false;
}

bool VoidCommandDockShipYardList::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    DockShipYardList();
    return true;
}
    

void VoidCommandDockShipYardList::DockShipYardList()
{
    get_thread()->SendClearScreen();
    
    std::string query = "select shiptype.nkey,shiptype.sname, shipmanufacturer.sname, nforecolor, nbackcolor from shiptype,shipmanufacturer where bforsale = TRUE and shiptype.kmanufacturer = shipmanufacturer.nkey order by shiptype.nkey;";

    PGresult *dbresult= get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Display sector error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    int numships = PQntuples(dbresult);


    Send(Color()->get(RED, BG_WHITE) + "        Ship List        " + Color()->blackout() + endr);


    for(int i=0;i< numships;i++)
    {

	std::string nkey = PQgetvalue(dbresult,i,0);
	std::string name = PQgetvalue(dbresult,i,1);
	std::string manufacturer = PQgetvalue(dbresult,i,2);

	FGColor fgcolor = (FGColor)atoi(PQgetvalue(dbresult,i,3));
	BGColor bgcolor = (BGColor)atoi(PQgetvalue(dbresult,i,4));

	Send(Color()->get(LIGHTCYAN) + nkey + Color()->get(GRAY) + ") " + Color()->get(fgcolor,bgcolor) + manufacturer +' ' + name + endr);


	
    }


    PQclear(dbresult);
    
}

 
