
#ifndef VOID_COMMAND_DOCK_SHIPYARD_SPEC_H
#define VOID_COMMAND_DOCK_SHIPYARD_SPEC_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandDockShipYardSpec : public VoidCommand
{
 public:
    VoidCommandDockShipYardSpec(VoidServerThread *thread);
    virtual ~VoidCommandDockShipYardSpec();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    bool DockShipYardSpec(const std::string &arguments);

    VoidCommandDockShipYardSpec();

};
    
#endif
