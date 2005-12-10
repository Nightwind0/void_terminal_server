
#ifndef VOID_SHIP_TOOLS_H
#define VOID_SHIP_TOOLS_H

#include "ToolSet.h"
#include "CommTools.h"

class ShipTools : public ToolSet
{
public:
    ShipTools(PGconn *dbconn);
    virtual ~ShipTools();

    void DeleteShip(int shipnum);
private:
//    CommTools m_comm_tools;

};

#endif
