
#ifndef VOID_COMMAND_COMPUTER_SHIPLIST
#define VOID_COMMAND_COMPUTER_SHIPLIST

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include "ShipListBehavior.h"
#include <string>

using std::string;

class VoidCommandComputerShipList : public VoidCommand, private ShipListBehavior
{
 public:
    VoidCommandComputerShipList(VoidServerThread *thread);
    virtual ~VoidCommandComputerShipList();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:


    VoidCommandComputerShipList();
    bool CommandComputerShipList(const std::string &arguments);

};

#endif
