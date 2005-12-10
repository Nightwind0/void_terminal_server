
#ifndef VOID_COMMAND_CLAIM
#define VOID_COMMAND_CLAIM

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>
#include "CombatTools.h"
#include "CommTools.h"
#include "ShipTools.h"

using std::string;

class VoidCommandClaim : public VoidCommand
{
 public:
    VoidCommandClaim(VoidServerThread *thread);
    virtual ~VoidCommandClaim();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 protected:

    //  int CreateEscapePodForPlayer(const std::string player);
    // void MoveShipRandomly(ShipHandle *ship);
 private:

    std::list<int> GetValidShipList(int cur_sector,  const std::string &player);

//    void ShowValidShipList(const std::list<int> &ships);
    VoidCommandClaim();
    bool CommandClaim(const std::string &arguments);

    CombatTools m_combat_tools;
    ShipTools m_ship_tools;
    CommTools m_comm_tools;
};

#endif
