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


ColorType * VoidCommand::Color()const
{
    return get_thread()->Color();
}

VoidCommand::VoidCommand(VoidServerThread *pthread):m_pthread(pthread)
{
}

VoidCommand::~VoidCommand()
{
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


    // TODO: Broadcast this data to players in the sector ("Player blah blah just left the sector to sector blah")

    // TODO: Mines, trackers, sentinels, enemy ships attacking.
    if(pship == NULL)
    {
	// Problem. no ship. 
    }
    else
    {
	pship->Lock();
	pship->SetSector(sector);
	pship->Unlock();


	Integer tow = pship->GetTow();

	ResourceMaster::GetInstance()->Log(DEBUG2, "Get tow reports : " + IntToString((int)tow));                           

	if(!tow.IsNull())
	{
	    Integer towship(ShipHandle::FieldName(ShipHandle::NKEY),tow.GetAsString());
	    PrimaryKey key(&towship);

	    ShipHandle towshiphandle(get_thread()->GetDBConn(),key);

	    ResourceMaster::GetInstance()->Log(DEBUG2, "towshiphandle reports : " + IntToString((int)towship));                           

	    towshiphandle.Lock();
	    towshiphandle.SetSector(sector);
	    towshiphandle.Unlock();
	    Send(Color()->get(LIGHTBLUE) + towshiphandle.GetName().GetAsString() + Color()->get(GREEN) + " enters the sector in tow." + endr);
	}
	
	get_thread()->PostCommand("display", "");
	
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


