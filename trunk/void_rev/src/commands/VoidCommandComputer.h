
#ifndef VOID_COMMAND_COMPUTER_H
#define VOID_COMMAND_COMPUTER_H

#include "VoidCommandSubCommands.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;

class VoidCommandComputer : public VoidCommandSubCommands
{
 public:
    VoidCommandComputer(VoidServerThread *thread);
    virtual ~VoidCommandComputer();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);


    
 private:




    VoidCommandComputer();


    void DisplayIntro()const;
    void DisplayOutro()const;
    std::string GetPrompt()const;
    void DisplayOptionsFull();
    std::string GetOptionDescriptionColor()const;
    std::string GetOptionColor()const;


};

#endif
