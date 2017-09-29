#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandStatus.h"
#include "Universe.h"
#include <sstream>
#include "ResourceMaster.h"

VoidCommandStatus::VoidCommandStatus(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandStatus::~VoidCommandStatus()
{
}

string VoidCommandStatus::GetCommandString()const
{
    return "Status";
}

string VoidCommandStatus::GetDescription()const
{
    return "Show your current player and ship status";
}

string VoidCommandStatus::GetSyntax()const
{
    return "status";
}

    
bool VoidCommandStatus::ClaimCommand(const string &command)
{
    if (CompStrings(command,"status")) return true;

    return false;
}

bool VoidCommandStatus::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    
    ShowStatus();

    return true;
}

std::string VoidCommandStatus::PrettyValue(PGresult *dbresult, int row, int column, bool usedash)
{
    if(PQgetisnull(dbresult,row,column))
    {
	return usedash?"-":"0";
    }
    else
    {
	return PQgetvalue(dbresult,row,column);
    }
}

std::string VoidCommandStatus::SendValue(const std::string &name, const std::string &value)
{

    std::ostringstream os;
    os << Color()->get(GREEN);
    os.width(20);
    os << std::left << name;
    os << "    " << Color()->get(WHITE);
    os.width(10);
    os << std::right << value;

    return os.str();
}

std::string VoidCommandStatus::ToBool(const std::string &value)
{
    if(value == "t")
	return "Y";
    else return "N";
}

std::string VoidCommandStatus::SendRow(const std::string &col1,const  std::string &col2,const std::string &col3)
{


    std::ostringstream os;

    os << Color()->get(GREEN);
    os.width(20);
    os << std::left <<  col1 << "    ";
    os << Color()->get(WHITE);
    os.width(10);
    os << std::right << col2 << "    ";
    os << Color()->get(CYAN);
    os.width(10);
    os << std::left <<  col3;

    return os.str();

}
    
void VoidCommandStatus::ShowStatus()
{

    PlayerHandlePtr player = get_player();
    ShipHandlePtr ship = create_handle_to_current_ship(player);

    ShipTypeHandlePtr shiptype = ship->GetShipTypeHandle();

    std::ostringstream os;
    std::ostringstream query;

    query << "select p.sname, p.ncredits, p.nturnsleft, p.npoints, p.kalliance, s.nkey,";
    query << "s.sname, s.ballianceowned, s.bpersonal, s.nsentinels, s.nmissiles,";
    query << "s.nmines, s.ntrackers, s.nshields, s.nplasma, s.nmetals, s.ncarbon,";
    query << "s.npeople, s.nexplosives, s.nprobes, s.nholds, s.bcloaked, s.ktowship, ";
    query << "t.sname, t.nmaxmissiles, t.nmaxshields, t.nmaxattack, t.nmaxholds, t.nmaxsentinels, ";
    query << "t.nmaxtrackers, t.nmaxmines, t.nmaxpeople, t.nmaxprobes, t.nturnspersector, ";
    query << "t.bwarpdrive, t.bcloakable, t.banalyzer, t.nscandistance, t.nforecolor, ";
    query << "t.ntransrange ";
    query << "from player p, ship s, shiptype t, shipmanufacturer tm ";
    query << " where p.sname = '" << player->GetName().GetAsString() << "' and  p.kcurrentship = s.nkey and s.ktype = t.nkey and t.kmanufacturer = tm.nkey;";

    ResourceMaster::GetInstance()->Log(DEBUG2, query.str());

    PGresult *dbresult = get_thread()->DBExec(query.str());


    os << Color()->get(CYAN,BG_WHITE) << "        Status        " +  Color()->blackout() +endr;
    os << SendValue("Player Name:",PQgetvalue(dbresult,0,0)) << endr;
    os << SendValue("Credits:",PQgetvalue(dbresult,0,1)) << endr;
    os << SendRow("Turns:",PQgetvalue(dbresult,0,2), CONFIG_STRING(ResourceMaster::GetInstance(),"turns_per_day")) << endr; // TODO: Get actual turns per day value


    int tps = atoi(PQgetvalue(dbresult,0,33));
    
    if(!PQgetisnull(dbresult,0,22))
    {
	Integer towship(ShipHandle::FieldName(ShipHandle::NKEY), PQgetvalue(dbresult,0,22));
	PrimaryKey key(&towship);
	ShipHandle towshiphandle(get_thread()->GetDBConn(),key);
	ShipTypeHandlePtr tsthandle= towshiphandle.GetShipTypeHandle();
	
	
	tps += tsthandle->GetTurnsPerSector();
    }

    os << SendValue("Turns/Sector", IntToString(tps)) << endr;
    os << SendValue("Points:",PQgetvalue(dbresult,0,3)) << endr;
    os << SendValue("Alliance:",PQgetvalue(dbresult,0,4)) << endr;
    // os << SendRow("Ship:",PQgetvalue(dbresult,0,5), PQgetvalue(dbresult,0,6)) << endr;
    os << Color()->get(GREEN);
    os.width(20);
    os << std::left <<"Ship Name:" ;
    os << Color()->get(LIGHTBLUE);
    os << "    ";
    os.width(20 + sizeof(Color()->get(LIGHTBLUE)));
    os << std::left <<  PQgetvalue(dbresult,0,6) << endr;
    os << SendValue("Ship Number:", PQgetvalue(dbresult,0,5)) << endr;
    os << SendValue("Ship Type:", PQgetvalue(dbresult,0,23)) << endr;
    os << SendRow("Sentinels:",PrettyValue(dbresult,0,9), PrettyValue(dbresult,0,28)) << endr;
    os << SendRow("Missiles:",PrettyValue(dbresult,0,10), PrettyValue(dbresult,0,24)) << endr;
    os << SendRow("Mines:", PrettyValue(dbresult,0,11), PrettyValue(dbresult,0,30)) << endr;
    os << SendRow("Trackers:",PrettyValue(dbresult,0,12), PrettyValue(dbresult,0,29)) << endr;
    os << SendRow("Shields:",PrettyValue(dbresult,0,13), PrettyValue(dbresult,0,25)) << endr;
    os << SendRow("Holds:", IntToString(atoi(PQgetvalue(dbresult,0,14)) + atoi(PQgetvalue(dbresult,0,15))
					+ atoi(PQgetvalue(dbresult,0,16))), PQgetvalue(dbresult,0,20)) << endr;
    os << SendValue("Plasma:",PrettyValue(dbresult,0,14)) << endr;
    os << SendValue("Metals:",PrettyValue(dbresult,0,15)) << endr;
    os << SendValue("Carbon:",PrettyValue(dbresult,0,16)) << endr;
    os << SendRow("People:",PrettyValue(dbresult,0,17), PrettyValue(dbresult,0,31)) << endr;
    os << SendRow("Probes:",PrettyValue(dbresult,0,19), PrettyValue(dbresult,0,32)) << endr;
    os << SendValue("Tow:", PrettyValue(dbresult,0,22,true)) << endr;
    os << SendValue("Beam Range:", PrettyValue(dbresult,0,39)) << endr;
    os << SendValue("Scan:", PrettyValue(dbresult,0,37,true)) << endr;
    os << SendValue("Warp Drive:", ToBool(PQgetvalue(dbresult,0,34))) << endr;
    os << SendValue("Cloak:",ToBool(PQgetvalue(dbresult,0,35))) << endr;
    os << SendValue("Analyzer:", ToBool(PQgetvalue(dbresult,0,36))) << endr;


    Send(os.str());
		   
/*
    get_thread()->Send(Color()->get(GREEN) + "Player: " + Color()->get(LIGHTBLUE)  + (std::string)player->GetName() + endr);
    get_thread()->Send(Color()->get(GREEN) + "Current Ship: " + Color()->get(WHITE) + "[" +  Color()->get(LIGHTGREEN)
	 + ship->GetNkey().GetAsString() + Color()->get(WHITE) + "]"+ Color()->get(LIGHTBLUE) + " " + (std::string)ship->GetName() + endr);
    get_thread()->Send(Color()->get(GREEN) + "Ship Type: " + (std::string)shiptype.GetShipTypeName(Color()) + Color()->blackout() + endr);
    get_thread()->Send(Color()->get(GREEN) + "Credits: " + Color()->get(WHITE) + player->GetCredits().GetAsString() + endr);
    get_thread()->Send(Color()->get(GREEN) + "Points: " + Color()->get(WHITE) + player->GetPoints().GetAsString() + endr);
    get_thread()->Send(Color()->get(GREEN) + "Holds: " + Color()->get(WHITE) + IntToString(ship->GetPlasma() + ship->GetMetals() + ship->GetCarbon()) 
	 +Color()->get(LIGHTCYAN) + "/" + Color()->get(WHITE) +  ship->GetHolds().GetAsString() + endr);
    get_thread()->Send(Color()->get(LIGHTPURPLE) + "Plasma: " + Color()->get(WHITE) + ship->GetPlasma().GetAsString() + endr);
    get_thread()->Send(Color()->get(BROWN) + "Metals: " + Color()->get(WHITE) + ship->GetMetals().GetAsString() + endr);
    get_thread()->Send(Color()->get(CYAN) + "Carbon: " + Color()->get(WHITE) + ship->GetCarbon().GetAsString() + endr);
    get_thread()->Send(Color()->get(GREEN) + "Missiles: " + Color()->get(WHITE) + ship->GetMissiles().GetAsString() + Color()->get(LIGHTCYAN) + "/" + Color()->get(WHITE)
	 +shiptype.GetMaxMissiles().GetAsString() + endr);
    get_thread()->Send(Color()->get(CYAN,BG_WHITE) + "                      " + Color()->get(WHITE) + endr);


*/

}

