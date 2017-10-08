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

    PlayerHandlePtr player = get_thread()->GetPlayer();
    int shiptype = atoi(arguments.c_str());
    
    std::string query = "select shiptype.nkey,shiptype.sname, nmaxmissiles,nmaxshields,nmaxattack,nmaxholds,ninitholds,nmaxsentinels,nmaxtrackers,nmaxmines,nmaxpeople,nmaxprobes,nturnspersector,bwarpdrive,bcloakable,banalyzer,nscandistance,ncost,  shipmanufacturer.sname, nforecolor, nbackcolor, ntransrange from shiptype,shipmanufacturer where bforsale = TRUE and shiptype.kmanufacturer = shipmanufacturer.nkey and shiptype.nkey = '" + IntToString(shiptype) + "' order by shiptype.nkey;";

    pqxx::result dbresult = get_thread()->DBExec(query);


    if(dbresult.size() != 1 || (shiptype == 0 && arguments != "0"))
    {
	Send(Color()->get(RED) + "Sorry, that is not a vaild ship number." + endr);
	return false;
    }

    auto shipinfo = dbresult[0];
        
    std::string nkey = shipinfo[0].as<std::string>();
    std::string name = shipinfo[1].as<std::string>();
    int maxmissiles = shipinfo[2].as<int>();
    int maxshields = shipinfo[3].as<int>();
    int maxattack = shipinfo[4].as<int>();
    int maxholds = shipinfo[5].as<int>();
    int initholds = shipinfo[6].as<int>();
    int maxsentinels = shipinfo[7].as<int>();
    int maxtrackers = shipinfo[8].as<int>();
    int maxmines = shipinfo[9].as<int>();
    int maxpeople = shipinfo[10].as<int>();
    int maxprobes = shipinfo[11].as<int>();
    int tps = shipinfo[12].as<int>();
    bool warpdrive = shipinfo[13].as<std::string>() == "t";
    bool cloakable = shipinfo[14].as<std::string>() == "t";
    bool analyzer = shipinfo[15].as<std::string>() == "t";
    int scandistance = shipinfo[16].as<int>();
    int cost = shipinfo[17].as<int>();
    std::string manufacturer = shipinfo[18].as<std::string>();
    
    FGColor fgcolor = (FGColor)shipinfo[19].as<int>();
    BGColor bgcolor = (BGColor)shipinfo[20].as<int>();
    int transrange = shipinfo[21].as<int>();

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



    return true;
    
}

 
