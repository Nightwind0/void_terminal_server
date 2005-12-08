#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandDisplay.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "OutpostHandle.h"
#include <math.h>
#include <sstream>

VoidCommandDisplay::VoidCommandDisplay(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandDisplay::~VoidCommandDisplay()
{
}

string VoidCommandDisplay::GetCommandString()const
{
    return "Display";
}

string VoidCommandDisplay::GetDescription()const
{
    return "Show the current sector and it's contents";
}

string VoidCommandDisplay::GetSyntax()const
{
    return "display";
}

    
bool VoidCommandDisplay::ClaimCommand(const string &command)
{
    if (CompStrings(command,"display")) return true;

    return false;
}

bool VoidCommandDisplay::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    int sector=0;

    if(arguments.size() && bFromPost)
    {
	sector = atoi(arguments.c_str());
    }
    else
    {
	ShipHandle * ship=  create_handle_to_current_ship(get_thread()->GetPlayer());
	sector = ship->GetSector();
	delete ship;
    }

    get_thread()->Send(DisplaySector( sector ));

    return true;
}
    
std::string VoidCommandDisplay::DisplaySector(int sector)
{
    std::vector<int> sectors = Universe::GetAdjacentSectors(sector);
    std::ostringstream os;

    std::string query = "select sname from territory,sectors where sectors.kterritory = territory.nkey and sectors.nsector = '" + IntToString(sector) + "';";

    PGresult * dbresult = get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Display sector error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }



    ResourceMaster::GetInstance()->Log(AUDIT, "^Displaying sector^");
    os << Color()->get(LIGHTGREEN) << endr <<  "Sector " << Color()->get(WHITE) << sector;
    if(PQntuples(dbresult) && !PQgetisnull(dbresult,0,0))
    {
	os << Color()->get(GRAY) << ' ' <<  PQgetvalue(dbresult,0,0);
    }
    PQclear(dbresult);
    os << endr;

    os << DisplayStardockInSector(sector);
    os << DisplayOutpostsInSector(sector);
    os << DisplayShipsInSector(sector); 

    os << Color()->get(GRAY) << "Adjacent Sectors: ";


    std::vector<int>::iterator i ;

    for(i=sectors.begin();i != sectors.end(); ++i)
    {
	os << Color()->get(RED) << '[' << Color()->get(WHITE) << *i << Color()->get(RED) << "] ";
    }

    return os.str();   



}


std::string VoidCommandDisplay::DisplayStardockInSector(int sector)
{
    std::ostringstream os;
    
    std::string query = "select bstardock,sstardockname from sectors where nsector = '" + IntToString(sector) + "';";

    PGresult *dbresult;
    dbresult = get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Display sector error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    Boolean has_stardock("bstardock", PQgetvalue(dbresult,0,0), PQgetisnull(dbresult,0,0));

    if((bool)has_stardock)
    {
	Text stardock("sstardockname",PQgetvalue(dbresult,0,1), PQgetisnull(dbresult,0,1));
	os << Color()->get(GREEN) << "Stardock: " << Color()->get(WHITE) << (std::string)stardock << endr;
    }

    PQclear(dbresult);
    return os.str();


}
 


std::string VoidCommandDisplay::DisplayShipsInSector(int sector)
{
    std::ostringstream os;
    std::string shipquery = "select s.nkey,s.sname, tm.sname, t.sname, s.nmissiles, t.nforecolor, t.nbackcolor, s.bcloaked, s.kowner, p.bmob, s.kalliance, s.nshields, t.nmaxshields from ship s,shiptype t, player p, shipmanufacturer tm where s.ksector ='"+IntToString(sector) + "' and s.ktype = t.nkey and tm.nkey = t.kmanufacturer and (s.bcloaked = false or s.bcloaked is null) and (p.sname = s.kowner) order by s.nkey;";
    PGresult *dbresult;

    dbresult = get_thread()->DBExec(shipquery);


    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Display sector error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    int numships = PQntuples(dbresult);


    if(numships)
    {
	os << Color()->get(GREEN) << "Ships:" << endr;
    }

    for(int i=0;i<numships;i++)
    {
	
	int ship = atoi(PQgetvalue(dbresult,i,0));

	int shields = atoi(PQgetvalue(dbresult,i,11));
	int maxshields = atoi(PQgetvalue(dbresult,i,12));
	double percent =(double) shields / (double)maxshields;
 
	int pc = truncf(percent * 10) * 10;

	

	os << '\t' << Color()->get(GREEN) << '[' << Color()->get(WHITE) << ship << Color()->get(GREEN) << ']';
	os << ' ' << Color()->get(LIGHTBLUE) << PQgetvalue(dbresult,i,1) << ' ';
	os << Color()->get((FGColor)atoi(PQgetvalue(dbresult,i,5)),(BGColor)atoi(PQgetvalue(dbresult,i,6))) << PQgetvalue(dbresult,i,2) <<' ' <<  PQgetvalue(dbresult,i,3) << Color()->get(GREEN);
	os << " w/" << Color()->get(WHITE) << atoi(PQgetvalue(dbresult,i,4)) << Color()->get(GREEN) << " missiles ";
	os << Color()->get(LIGHTPURPLE) << '(' << Color()->get(GRAY) << pc << '%' << Color()->get(LIGHTPURPLE) << ')' << Color()->get(GREEN) + " shields " <<  endr;

	PlayerHandle * player = create_handle_to_player_in_ship(ship);
// Need to check for null here, there may not be a player in the ship.
	

	if(player)
	{
	    os << "\t\t" << Color()->get(LIGHTGREEN) << "piloted by " ;
	
	}
	else
	{
	    os << "\t\t" << Color()->get(GREEN) << "owned by " ;
	}
/* 
	    if((bool)player->GetIsMob())
	    {
		os << Color()->get(LIGHTPURPLE);
	    }
	    else 
	    {
		os << Color()->get(LIGHTCYAN);
	    }
	    
	    os << (std::string)player->GetName();
	    
	    Text alliance = player->GetAlliance();
	    
	    if(!alliance.IsNull())
	    {
		os << Color()->get(GRAY) << " {" << Color()->get(RED) <<  alliance.GetAsString() << Color()->get(GRAY) << '}';
	    }
	    
	    os << endr;
*/


	if(PQgetisnull(dbresult,i,9) || std::string(PQgetvalue(dbresult,i,9)) != "t")
	{
	    os << Color()->get(LIGHTCYAN);
	}
	else
	{
	    // Owned by a mob
	    os << Color()->get(LIGHTPURPLE);
	}
	
	os << PQgetvalue(dbresult,i,8);
	
	if(!PQgetisnull(dbresult,i,10))
	{
	    os << Color()->get(GRAY) << " {" << Color()->get(RED) <<  PQgetvalue(dbresult,i,10) << Color()->get(GRAY) << '}';
	}
	os << endr;
	
	
	delete player;
	    
    }
    
    
    PQclear(dbresult);
       
    os << endr;

    return os.str();

}

std::string VoidCommandDisplay::DisplayOutpostsInSector(int sector)
{

    std::ostringstream os;
    std::string outpostquery = "select sname, bbuyplasma, bbuymetals, bbuycarbon, fplasmaprice, fmetalsprice,fcarbonprice, floor((round(date_part('epoch',now())) - round(date_part('epoch',dlastvisit))) / 60) from Outpost where ksector = " + IntToString(sector) + ";";
    PGresult *dbresult;

    dbresult = get_thread()->DBExec(outpostquery);


    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Display outpost error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    int numoutposts = PQntuples(dbresult);

    if(numoutposts == 0 ) return "";

    os << Color()->get(GREEN) << "Outposts:" << endr;


    for(int i=0; i < numoutposts; i++)
    {
	os << '\t' << Color()->get(LIGHTPURPLE) << PQgetvalue(dbresult,i,0);

	os << Color()->get(WHITE) << " (";

	int minutes = atoi(PQgetvalue(dbresult,i,7));

	Boolean buyplasma("bbuyplasma", PQgetvalue(dbresult,i,1), PQgetisnull(dbresult,i,1));
	Boolean buymetals("bbuymetals", PQgetvalue(dbresult,i,2), PQgetisnull(dbresult,i,2));
	Boolean buycarbon("bbuycarbon", PQgetvalue(dbresult,i,3), PQgetisnull(dbresult,i,3));

	double plasmaprice = atof(PQgetvalue(dbresult,i,4));
	double metalsprice = atof(PQgetvalue(dbresult,i,5));
	double carbonprice = atof(PQgetvalue(dbresult,i,6));


	if(buyplasma.GetValue())
	{
	    os << Color()->get(LIGHTBLUE) << 'B' ;
	    os << (int) round( OutpostHandle::GetBuyRateAfterTime(minutes,plasmaprice));
	    os << ',';
	}
	else 
	{
	    os << Color()->get(LIGHTCYAN) << 'S' ;
	    os << (int) round(OutpostHandle::GetSellRateAfterTime(minutes,plasmaprice));
	    os << ',';
	}

	if(buymetals.GetValue())
	{
	    os << Color()->get(LIGHTBLUE) << 'B';
	    os << (int) round(OutpostHandle::GetBuyRateAfterTime(minutes,metalsprice));
	    os << ',';
	}
	else
	{
	    os << Color()->get(LIGHTCYAN) << 'S' ;
	    os << (int) round(OutpostHandle::GetSellRateAfterTime(minutes,metalsprice));
	    os << ',';
	}

	if(buycarbon.GetValue())
	{
	    os << Color()->get(LIGHTBLUE) << 'B';
	    os << (int) round(OutpostHandle::GetBuyRateAfterTime(minutes,carbonprice));
	}
	else 
	{
	    os << Color()->get(LIGHTCYAN) << 'S';
	    os << (int) round(OutpostHandle::GetSellRateAfterTime(minutes,carbonprice));
	}
	
	os << Color()->get(WHITE) << ')' << endr;

    }

    return os.str();

}

