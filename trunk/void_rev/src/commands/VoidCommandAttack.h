
#ifndef VOID_COMMAND_ATTACK
#define VOID_COMMAND_ATTACK

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>
#include "CombatTools.h"
#include "ShipTools.h"


using std::string;

class VoidCommandAttack : public VoidCommand
{
 public:
    VoidCommandAttack(VoidServerThread *thread);
    virtual ~VoidCommandAttack();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 protected:

    //   int CreateEscapePodForPlayer(const std::string player);
    // void MoveShipRandomly(ShipHandle *ship);

    int PromptNumberOfMissiles(int maxattack, int missiles_available);
 private:

    VoidCommandAttack();
    bool CommandAttack(int ship);

    CombatTools m_combat_tools;
    ShipTools m_ship_tools;

};

#endif
