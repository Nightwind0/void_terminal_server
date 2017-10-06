
#ifndef VOID_COMMAND_COMPUTER_DOCKLIST
#define VOID_COMMAND_COMPUTER_DOCKLIST

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>

using std::string;

class VoidCommandComputerDockList : public VoidCommand
{
 public:
    VoidCommandComputerDockList(VoidServerThread *thread);
    virtual ~VoidCommandComputerDockList();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    VoidCommandComputerDockList();
    bool CommandComputerShipList(const std::string &arguments);

};

#endif
