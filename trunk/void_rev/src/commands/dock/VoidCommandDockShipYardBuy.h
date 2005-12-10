
#ifndef VOID_COMMAND_DOCK_SHIPYARD_BUY_H
#define VOID_COMMAND_DOCK_SHIPYARD_BUY_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "EvaluateShipBehavior.h"

using std::string;

class VoidCommandDockShipYardBuy : public VoidCommand, private EvaluateShipBehavior
{
 public:
    VoidCommandDockShipYardBuy(VoidServerThread *thread);
    virtual ~VoidCommandDockShipYardBuy();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    bool DockShipYardBuy(const std::string &arguments);

    VoidCommandDockShipYardBuy();

};
    
#endif
