#include "DeleteShipBehavior.h"
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

DeleteShipBehavior::DeleteShipBehavior(VoidServerThread *thread)
{
    m_thread = thread;
}

DeleteShipBehavior::~DeleteShipBehavior()
{
}


void DeleteShipBehavior::delete_ship(int ship)
{
    /// @todo set values using handles, for locking purposes
    std::string deletestmt = (std::string)"update ship set ktowship = null where ktowship = '" + IntToString(ship) + "';" + "update player set kcurrentship = null where sname = (select sname from player where kcurrentship = '" + IntToString(ship) + "');";

    deletestmt += "delete from ship where nkey = '" + IntToString(ship) + "';";
    
    PGresult *delresult = m_thread->DBExec(deletestmt);
    
    
    if(PQresultStatus(delresult) != PGRES_COMMAND_OK)
    {
	
	DBException e("Delete ship error: " + std::string(PQresultErrorMessage(delresult)));
	PQclear(delresult);
	throw e;
    }
    
    PQclear(delresult);
}
