
#ifndef VOID_COMMAND_COMPUTER_PLAYERS
#define VOID_COMMAND_COMPUTER_PLAYERS

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>

using std::string;

class VoidCommandComputerPlayers : public VoidCommand
{
 public:
    VoidCommandComputerPlayers(VoidServerThread *thread);
    virtual ~VoidCommandComputerPlayers();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:


    VoidCommandComputerPlayers();
    bool CommandComputerPlayers(const std::string &arguments);

};

#endif
