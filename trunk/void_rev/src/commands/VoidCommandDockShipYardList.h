
#ifndef VOID_COMMAND_DOCK_SHIPYARD_LIST_H
#define VOID_COMMAND_DOCK_SHIPYARD_LIST_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandDockShipYardList : public VoidCommand
{
 public:
    VoidCommandDockShipYardList(VoidServerThread *thread);
    virtual ~VoidCommandDockShipYardList();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    void DockShipYardList();

    VoidCommandDockShipYardList();

};
    
#endif
