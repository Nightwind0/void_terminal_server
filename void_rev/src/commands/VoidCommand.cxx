#include "VoidCommand.h"
#include <vector>
#include <algorithm>
#include <string>
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "VoidServerThread.h"
#include "void_util.h"
#include "libpq-fe.h"
#include "ResourceMaster.h"

using std::string;

const char *  VoidCommand::endr = "\n\r";


std::shared_ptr<ColorType>  VoidCommand::Color()const
{
    return get_thread()->Color();
}

VoidCommand::VoidCommand(VoidServerThread *pthread):m_pthread(pthread)
{
}

VoidCommand::~VoidCommand()
{
}


bool VoidCommand::isValidPlayer(const std::string playername)const
{
    std::string query = "select count(0) from player where sname = '" + playername + "';";

    PGresult * dbresult = get_thread()->DBExec(query);

    if(PQntuples(dbresult) != 1)
    {
	PQclear(dbresult);
	return false;
    }

    if(atoi(PQgetvalue(dbresult,0,0)) != 1)
    {
	PQclear(dbresult);
	return false;
    }

    PQclear(dbresult);
    return true;
}

void VoidCommand::Send(const std::string &str) const
{
    get_thread()->Send(str);
}

std::string VoidCommand::ReceiveLine() const
{
    return get_thread()->ReceiveLine();
}


PlayerHandle * VoidCommand::get_player() const
{
    return get_thread()->GetPlayer();
}


bool VoidCommand::move_player_to_sector(int sector)
{

    ShipHandle *pship = create_handle_to_current_ship(get_player());

    PlayerHandle * player = get_thread()->GetPlayer();

    // TODO: Broadcast this data to players in the sector ("Player blah blah just left the sector to sector blah")

    // TODO: Mines, trackers, sentinels, enemy ships attacking.
    if(pship == NULL)
    {
	// Problem. no ship. 
    }
    else
    {

	int cur_turns = (int)player->GetTurnsLeft();
	ShipTypeHandle shiptype= pship->GetShipTypeHandle();
	int tps = (int)shiptype.GetTurnsPerSector();



	Integer tow = pship->GetTow();

	if(!tow.IsNull())
	{
	    Integer towship(ShipHandle::FieldName(ShipHandle::NKEY),tow.GetAsString());
	    PrimaryKey key(&towship);

	    ShipHandle towshiphandle(get_thread()->GetDBConn(),key);
	    ShipTypeHandle towshiptype = towshiphandle.GetShipTypeHandle();

	    tps += (int)towshiptype.GetTurnsPerSector();

	    if(tps <= cur_turns)
	    {

		towshiphandle.Lock();
		towshiphandle.SetSector(sector);
		towshiphandle.Unlock();
		Send(Color()->get(LIGHTBLUE) + towshiphandle.GetName().GetAsString() + Color()->get(GREEN) + " enters the sector in tow." + endr);
	
	    }
	
	}

	if(tps <= cur_turns)
	{

	    player->Lock();
	    player->SetTurnsLeft( cur_turns - tps);
	    player->Unlock();
	    pship->Lock();
	    pship->SetSector(sector);
	    pship->Unlock();

	    
	    
	    
	    get_thread()->PostCommand("display", "");
	}
	else
	{
	    Send(Color()->get(LIGHTRED) + "You don't have enough turns." + endr);
	    delete pship;
	    return false;
	}
	
	//TODO: Determine if there are interesting things here and provide option to stop
    }

    delete pship;
    return true;
}



PlayerHandle* VoidCommand::create_handle_to_player_in_ship(int ship) const
{
    std::string query = "SELECT sname from Player where kcurrentship = " + IntToString(ship) + ";";

    PGresult *dbresult;

    dbresult = get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Get player in ship error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    if(PQntuples(dbresult) > 1)
    {
	DBException e("Multiple players in same ship!!");
	PQclear(dbresult);
	throw e;
    }

    if(PQntuples(dbresult) < 1)
    {
	return NULL;
    }

    std::string name = PQgetvalue(dbresult,0,0);

    PQclear(dbresult);

    Text playername(PlayerHandle::FieldName(PlayerHandle::NAME), name);
    PrimaryKey key(&playername);

    PlayerHandle * player = new PlayerHandle(get_thread()->GetDBConn(),key, false);

    return player;

}

VoidServerThread * VoidCommand::get_thread()const
{
    return m_pthread;
}

ShipHandle *VoidCommand::create_handle_to_current_ship(PlayerHandle *player) const
{
    Integer shipnum(ShipHandle::FieldName(ShipHandle::NKEY), player->GetCurrentShip().GetAsString());
    PrimaryKey key(&shipnum);

    ShipHandle *ship = new ShipHandle(get_thread()->GetDBConn(), key, false);

    return ship;
}
std::string VoidCommand::get_config(const std::string &key) const
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    return RM->GetConfig(key);
}


