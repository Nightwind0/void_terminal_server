
#ifndef VOID_COMMAND_COMPUTER_CLOAK
#define VOID_COMMAND_COMPUTER_CLOAK

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>

using std::string;

class VoidCommandComputerCloak : public VoidCommand
{
 public:
    VoidCommandComputerCloak(VoidServerThread *thread);
    virtual ~VoidCommandComputerCloak();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:


    VoidCommandComputerCloak();
    bool CommandComputerCloak(const std::string &arguments);

};

#endif
