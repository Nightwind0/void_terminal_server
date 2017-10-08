

#include "ShipTools.h"


ShipTools::ShipTools(DatabaseConnPtr dbconn):ToolSet(dbconn)
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

    DBExec(deletestmt);
}


