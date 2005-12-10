
#ifndef VOID_COMMAND_DOCK_SHIPYARD_SELL_H
#define VOID_COMMAND_DOCK_SHIPYARD_SELL_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "EvaluateShipBehavior.h"
#include "ShipTools.h"

using std::string;



class VoidCommandDockShipYardSell : public VoidCommand, private EvaluateShipBehavior
{
 public:
    VoidCommandDockShipYardSell(VoidServerThread *thread);
    virtual ~VoidCommandDockShipYardSell();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:

    std::list<int> GetOwnedEmptyShipsInSector(const std::string &player, int sector);
    bool DockShipYardSell(const std::string &arguments);

    VoidCommandDockShipYardSell();

    ShipTools m_ship_tools;

};
    
#endif
