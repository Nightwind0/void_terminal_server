
#ifndef VOID_COMMAND_DOCK_SHIP_YARD_H
#define VOID_COMMAND_DOCK_SHIP_YARD_H

#include "VoidCommandSubCommands.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;

class VoidCommandDockShipYard : public VoidCommandSubCommands
{
 public:
    VoidCommandDockShipYard(VoidServerThread *thread);
    virtual ~VoidCommandDockShipYard();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);



    
 private:




    VoidCommandDockShipYard();

    void DisplayIntro()const;
    void DisplayOutro()const;
    std::string GetPrompt()const;
    void DisplayOptionsFull();
    std::string GetOptionDescriptionColor()const;
    std::string GetOptionColor()const;


};

#endif
