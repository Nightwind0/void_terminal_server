
#ifndef VOID_COMMAND_DOCK_H
#define VOID_COMMAND_DOCK_H

#include "VoidCommandSubCommands.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;

class VoidCommandDock : public VoidCommandSubCommands
{
 public:
    VoidCommandDock(VoidServerThread *thread);
    virtual ~VoidCommandDock();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);

    bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:




    VoidCommandDock();


    void DisplayIntro()const;
    void DisplayOutro()const;
    std::string GetPrompt()const;
    void DisplayOptionsFull();
    std::string GetOptionDescriptionColor()const;
    std::string GetOptionColor()const;


};

#endif
