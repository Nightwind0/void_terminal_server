#include "SectorCommBehavior.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <math.h>
#include "ResourceMaster.h"
#include "SocketException.h"
#include <algorithm>
#include "VoidServerThread.h"
#include <list>
#include <string>

SectorCommBehavior::SectorCommBehavior(VoidServerThread *thread)
{
    m_thread = thread;
}

SectorCommBehavior::~SectorCommBehavior()
{
}

std::list<std::string> SectorCommBehavior::get_players_in_sector(int sector)
{
    std::list<std::string> playerlist;
    std::string query = "select player.sname from player,ship where ship.ksector = '" + IntToString(sector)
	+  "' and player.kcurrentship = ship.nkey;";

    PGresult *dbresult = m_thread->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Get players in sector error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    int numplayers = PQntuples(dbresult);

    for(int i=0;i<numplayers;i++)
    {
	playerlist.push_back(PQgetvalue(dbresult,i,0));
    }

    PQclear(dbresult);

    return playerlist;
}

void SectorCommBehavior::SendMsgToSector(const std::string &str, int sec, const std::string &exceptplayer)
{
    Message msg;
    msg.SetType(Message::BATTLE);
    msg.SetFrom("SYSTEM");


    msg.SetString(str);

    std::list<std::string> players = get_players_in_sector(sec);

    for(std::list<std::string>::iterator iter = players.begin();
	iter != players.end(); iter++)
    {
	if(*iter != exceptplayer)
	    ResourceMaster::GetInstance()->SendMessage(m_thread->GetLocalSocket(), *iter, &msg);
    }
    
    
}

