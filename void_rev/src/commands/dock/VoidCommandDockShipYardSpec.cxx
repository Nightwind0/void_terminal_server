#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockShipYardSpec.h"
#include <sstream>
#include <iostream>

using std::string;

using std::ostringstream;

using std::left;

using std::right;

VoidCommandDockShipYardSpec::VoidCommandDockShipYardSpec(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandDockShipYardSpec::~VoidCommandDockShipYardSpec()
{
}

string VoidCommandDockShipYardSpec::GetCommandString()const
{
    return "Spec";
}
string VoidCommandDockShipYardSpec::GetDescription()const
{
    return "show specs and stats for a star ship";
}
string VoidCommandDockShipYardSpec::GetSyntax()const
{
    return "spec <ship number>";
}
    
bool VoidCommandDockShipYardSpec::ClaimCommand(const string &command)
{
    if(CompStrings(command,"spec"))
	return true;
    else return false;
}

bool VoidCommandDockShipYardSpec::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    return DockShipYardSpec(arguments);

}
    

bool VoidCommandDockShipYardSpec::DockShipYardSpec(const string &arguments)
{

    PlayerHandle * player = get_thread()->GetPlayer();
    int shiptype = atoi(arguments.c_str());
    
    std::string query = "select shiptype.nkey,shiptype.sname, nmaxmissiles,nmaxshields,nmaxattack,nmaxholds,ninitholds,nmaxsentinels,nmaxtrackers,nmaxmines,nmaxpeople,nmaxprobes,nturnspersector,bwarpdrive,bcloakable,banalyzer,nscandistance,ncost,  shipmanufacturer.sname, nforecolor, nbackcolor, ntransrange from shiptype,shipmanufacturer where bforsale = TRUE and shiptype.kmanufacturer = shipmanufacturer.nkey and shiptype.nkey = '" + IntToString(shiptype) + "' order by shiptype.nkey;";

    PGresult *dbresult= get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Spec ship error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    if(PQntuples(dbresult) != 1 || (shiptype == 0 && arguments != "0"))
    {
	Send(Color()->get(RED) + "Sorry, that is not a vaild ship number.");
	return false;
    }
        
    std::string nkey = PQgetvalue(dbresult,0,0);
    std::string name = PQgetvalue(dbresult,0,1);
    int maxmissiles = atoi(PQgetvalue(dbresult,0,2));
    int maxshields = atoi(PQgetvalue(dbresult,0,3));
    int maxattack = atoi(PQgetvalue(dbresult,0,4));
    int maxholds = atoi(PQgetvalue(dbresult,0,5));
    int initholds = atoi(PQgetvalue(dbresult,0,6));
    int maxsentinels = atoi(PQgetvalue(dbresult,0,7));
    int maxtrackers = atoi(PQgetvalue(dbresult,0,8));
    int maxmines = atoi(PQgetvalue(dbresult,0,9));
    int maxpeople = atoi(PQgetvalue(dbresult,0,10));
    int maxprobes = atoi(PQgetvalue(dbresult,0,11));
    int tps = atoi(PQgetvalue(dbresult,0,12));
    bool warpdrive = std::string(PQgetvalue(dbresult,0,13)) == "t";
    bool cloakable = std::string(PQgetvalue(dbresult,0,14)) == "t";
    bool analyzer = std::string(PQgetvalue(dbresult,0,15)) == "t";
    int scandistance = atoi(PQgetvalue(dbresult,0,16));
    int cost = atoi(PQgetvalue(dbresult,0,17));
    std::string manufacturer = PQgetvalue(dbresult,0,18);
    
    FGColor fgcolor = (FGColor)atoi(PQgetvalue(dbresult,0,19));
    BGColor bgcolor = (BGColor)atoi(PQgetvalue(dbresult,0,20));
    int transrange = atoi(PQgetvalue(dbresult,0,21));

    Send(Color()->get(fgcolor,bgcolor) + 
	 "                    " + manufacturer + ' ' + name + "                    " + Color()->blackout() + endr);

    Send(endr + Color()->get(BLUE,BG_WHITE) + "    General Features    " + Color()->blackout() + endr);
    Send(Color()->get(GREEN));
    ostringstream os;


    os.fill(' ');
    os.width(16);
    os << left << " Cost" << Color()->get(WHITE);
    os.width(7);
    os << right << cost << Color()->get(GREEN);
    os.width(16);
    os << left << " Turns / Sector" << Color()->get(WHITE);
    os.width(7);
    os << right << tps << Color()->get(GREEN);
    os.width(16);
    os << left << " Max Holds" << Color()->get(WHITE);
    os.width(7);
    os << right << maxholds << Color()->get(GREEN);

    os << endr;

    os.width(16);
    os << left << " Initial Holds" << Color()->get(WHITE);
    os.width(7);
    os << right << initholds << Color()->get(GREEN);
    os.width(16);
    os << left << " Max People(10k)" << Color()->get(WHITE);
    os.width(7);
    os << right << maxpeople << Color()->get(GREEN);
    os.width(16);
    os << left << " Max Probes" << Color()->get(WHITE);
    os.width(7);
    os << right << maxprobes ;

    os << endr;

    Send(os.str());
    os.str("");

    Send(endr + Color()->get(BLACK,BG_RED) + "    Weaponry     " + Color()->blackout() + endr);
    Send(Color()->get(GREEN));
    os.width(16);
    os << left << " Max Missiles" << Color()->get(WHITE);
    os.width(7);
    os << right << maxmissiles << Color()->get(GREEN);
    os.width(16);
    os << left << " Missile Attack" << Color()->get(WHITE);
    os.width(7);
    os << right << maxattack << Color()->get(GREEN);
    os.width(16);
    os << left << " Max Sentinels" << Color()->get(WHITE);
    os.width(7);
    os << right << maxsentinels << Color()->get(GREEN);

    os << endr;

    Send(os.str());
    os.str("");
    
    Send(endr + Color()->get(BLACK,BG_WHITE) + "    Defenses     " + Color()->blackout() + endr);
    Send(Color()->get(GREEN));
    os.width(16);
    os << left << " Max Shields" << Color()->get(WHITE);
    os.width(7);
    os << right << maxshields << Color()->get(GREEN);
    os.width(16);
    os << left << " Max Mines" << Color()->get(WHITE);
    os.width(7);
    os << right << maxmines << Color()->get(GREEN);
    os.width(16);
    os << left << " Max Trackers" << Color()->get(WHITE);
    os.width(7);
    os << right << maxtrackers;

    os << endr;

    Send(os.str());
    os.str("");

    Send(endr + Color()->get(BLUE,BG_WHITE) + "    Features     " + Color()->blackout() + endr);
    Send(Color()->get(GREEN));
    os.width(16);
    os << left << " Warp Drive" << Color()->get(WHITE);
    os.width(7);
    os << right << std::string((warpdrive) ?"Y":"N") << Color()->get(GREEN);
    os.width(16);
    os << left << " Cloakable" << Color()->get(WHITE);
    os.width(7);
    os << right << std::string((cloakable)?"Y":"N") << Color()->get(GREEN);
    os.width(16);
    os << left << " Analyzer" << Color()->get(WHITE);
    os.width(7);
    os << right << std::string((analyzer)?"Y":"N") << Color()->get(GREEN);

    os << endr;
    os.width(16);
    os << left << " Scan Distance" << Color()->get(WHITE);
    os.width(7);
    os << right << scandistance << Color()->get(GREEN);
    os.width(16);
    os << left << " Beam Range" << Color()->get(WHITE);
    os.width(7);
    os << right << transrange ;

    os << endr << endr;

    Send(os.str());
    os.str("");

    PQclear(dbresult);

    return true;
    
}

 
