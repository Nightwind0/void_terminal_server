
#ifndef VOID_COMMAND_SUBCOMMANDS_H
#define VOID_COMMAND_SUBCOMMANDS_H

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;

class VoidCommandSubCommands : public VoidCommand
{
 public:
    VoidCommandSubCommands(VoidServerThread *thread);
    virtual ~VoidCommandSubCommands();

    virtual string GetCommandString()const=0;
    virtual string GetDescription()const=0;
    virtual string GetSyntax()const=0;
    
    virtual bool ClaimCommand(const string &command)=0;
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    

 protected:

    bool DoCommand(const string &command, const string &arguments, bool bFromPost);

    virtual void DisplayOptions();
    virtual void MainLoop();
    void AddSubCommand(std::shared_ptr<VoidCommand> command);

    virtual void DisplayIntro()const =0;
    virtual void DisplayOutro()const =0;
    virtual std::string GetPrompt()const = 0;
    virtual void DisplayOptionsFull() = 0;
    virtual std::string GetOptionDescriptionColor()const =0;
    virtual std::string GetOptionColor()const =0;

 private:


    VoidCommandSubCommands();


    std::vector<std::shared_ptr<VoidCommand>> m_commandlist;

};

#endif
