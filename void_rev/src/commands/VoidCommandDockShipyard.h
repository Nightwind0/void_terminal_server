
#ifndef VOID_COMMAND_DOCK_SHIPYARD_H
#define VOID_COMMAND_DOCK_SHIPYARD_H

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;

class VoidCommandDock : public VoidCommand
{
 public:
    VoidCommandDock(VoidServerThread *thread);
    virtual ~VoidCommandDock();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:


    VoidCommandDock();
    bool CommandDockShipyard(const std::string &arguments);

    void DisplayIntro(const std::string &dockname);

    bool DoCommand(const std::string &cmd);

    std::list<VoidCommand*> m_commandlist;

};

#endif
