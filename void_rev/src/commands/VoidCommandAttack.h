
#ifndef VOID_COMMAND_ATTACK
#define VOID_COMMAND_ATTACK

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include "EscapePodBehavior.h"
#include "SectorCommBehavior.h"
#include <string>

using std::string;

class VoidCommandAttack : public VoidCommand, public EscapePodBehavior, public SectorCommBehavior
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
 private:



    void remove_sentinels(int num, const std::string &player);
    double g_random(int) const;
    double g_rand() const;
    void delete_ship(int ship);

    void KillPlayer(const std::string &player);
    VoidCommandAttack();
    bool CommandAttack(int ship);

};

#endif
