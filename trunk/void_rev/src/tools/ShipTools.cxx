

#include "ShipTools.h"


ShipTools::ShipTools(PGconn *dbconn):ToolSet(dbconn)
{
}

ShipTools::~ShipTools()
{
}


void ShipTools::DeleteShip(int shipnum)
{
    /// @todo set values using handles, for locking purposes
    std::string deletestmt = (std::string)"update ship set ktowship = null where ktowship = '" + IntToString(shipnum) + "';" + "update player set kcurrentship = null where sname = (select sname from player where kcurrentship = '" + IntToString(shipnum) + "');";

    deletestmt += "delete from ship where nkey = '" + IntToString(shipnum) + "';";
    
    PGresult *delresult = DBExec(deletestmt);
    
    
    if(PQresultStatus(delresult) != PGRES_COMMAND_OK)
    {
	
	DBException e("Delete ship error: " + std::string(PQresultErrorMessage(delresult)));
	PQclear(delresult);
	throw e;
    }
    
    PQclear(delresult);
}


