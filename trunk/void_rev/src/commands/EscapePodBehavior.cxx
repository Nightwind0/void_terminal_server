#include "EscapePodBehavior.h"
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

EscapePodBehavior::EscapePodBehavior(VoidServerThread *thread)
{
    m_thread = thread;
}

EscapePodBehavior::~EscapePodBehavior()
{
}

int EscapePodBehavior::CreateEscapePodForPlayer(const std::string player)
{

    std::string nextvalsql = "select nextval('ship_nkey_seq');";
    
    PGresult *dbresult = m_thread->DBExec(nextvalsql);

    std::string nextval = PQgetvalue(dbresult,0,0);
    PQclear(dbresult);

    int shipnum = atoi(nextval.c_str());


    std::string stmt = "insert into ship (nkey, sname, kowner, ktype) values(" + nextval + ",'*POD*','" + player + "',10);";
    // TODO: Get escape pode type from config table

     dbresult = m_thread->DBExec(stmt);

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	DBException e("Escape pod create error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    PQclear(dbresult);

    std::string sit = "update player set kcurrentship = '" + nextval + "' where sname = '" + player + "';";

    dbresult = m_thread->DBExec(sit);

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	DBException e("Escape pod create error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    PQclear(dbresult);
    

    return shipnum;
    
}

void EscapePodBehavior::MoveShipRandomly(ShipHandle *ship)
{
    const int NUMJUMPS = 3;
    int cursec = (int)ship->GetSector();

    srand(time(NULL));

    /// @todo get value from config table
    for(int i = 0; i < NUMJUMPS; i++)
    {
	std::vector<int> adj = Universe::GetAdjacentSectors(cursec);
	
	int r = rand() % adj.size();

	cursec = adj[r];
    }

    /// @todo find a way to actually process moves here, rather than just magically teleporting.
    /// @todo there needs to be a method in the base class for the user and alien threads that 
    /// provides for everything involved in moving a ship (aside from turns?)
    ship->Lock();
    ship->SetSector(cursec);
    ship->Unlock();
}
