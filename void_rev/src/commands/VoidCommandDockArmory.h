
#ifndef VOID_COMMAND_DOCK_ARMORY_H
#define VOID_COMMAND_DOCK_ARMORY_H

#include "VoidCommandSubCommands.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;

class VoidCommandDockArmory : public VoidCommandSubCommands
{
 public:
    VoidCommandDockArmory(VoidServerThread *thread);
    virtual ~VoidCommandDockArmory();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);



    
 private:




    VoidCommandDockArmory();

    void DisplayIntro()const;
    void DisplayOutro()const;
    std::string GetPrompt()const;
    void DisplayOptionsFull();
    std::string GetOptionDescriptionColor()const;
    std::string GetOptionColor()const;


};

#endif
