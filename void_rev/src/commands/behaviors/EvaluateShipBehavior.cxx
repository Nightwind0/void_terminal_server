#include "EvaluateShipBehavior.h"
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

EvaluateShipBehavior::EvaluateShipBehavior(VoidServerThread *thread)
{
    m_thread = thread;
}

EvaluateShipBehavior::~EvaluateShipBehavior()
{
}

int EvaluateShipBehavior::EvaluateShip(int shipnum)
{

    


    std::string sql = "select nmissiles, nshields, nholds - ninitholds, nsentinels, nmines, ncost from ship,shiptype where ship.ktype = shiptype.nkey and ship.nkey = '" + IntToString(shipnum) + "';";

    PGresult *dbresult = m_thread->DBExec(sql);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	DBException e("Evaluate ship error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    if(PQntuples(dbresult) != 1)
    {
	DBException e("Evaluate ship didnt return a row.");
	PQclear(dbresult);
	throw e;
    }


    int cost = atoi(PQgetvalue(dbresult,0,5));
    int value = cost /2;

    int missiles = atoi(PQgetvalue(dbresult,0,0));
    int shields = atoi(PQgetvalue(dbresult,0,1));
    int holds = atoi(PQgetvalue(dbresult,0,2));
    int sentinels = atoi(PQgetvalue(dbresult,0,3));
    int mines = atoi(PQgetvalue(dbresult,0,4));

    value += missiles * 100; /// @todo get real missile cost from config
    value += shields * 100; 
    value += holds * 100;
    value += sentinels * 150;

    return value;
}
