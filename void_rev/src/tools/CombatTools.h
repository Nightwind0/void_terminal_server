#ifndef VOID_COMBAT_TOOLS_H
#define VOID_COMBAT_TOOLS_H

#include "ShipHandle.h"
#include "PlayerHandle.h"
#include "CommTools.h"
#include <list>
#include <string>
#include <libpq-fe.h>

class CombatTools : public ToolSet
{
public:
    CombatTools(PGconn * dbconn, DatagramSocketPtr  pSocket);
    virtual ~CombatTools();
    
    void DestroyShip(ShipHandlePtr pShip, PlayerHandlePtr pPlayer, PlayerHandlePtr pTarget, int cursector);
    void LogAttack(const std::string &attacker, const std::string &target, 
		   const std::string &target_ship, int missiles, int cursector);
    void LogDamage(const std::string &attacker, const std::string &target_ship, 
		   int damage, int cursector);
    int FireMissilesAtShip(int missiles, ShipHandlePtr pShip, ShipHandlePtr pTargetShip, int oshields);
    void KillPlayer(PlayerHandlePtr pPlayer, PlayerHandlePtr pTarget);

    ShipHandlePtr CreateEscapePodForPlayer(const std::string &player, int cursector);
    void MoveShipRandomly(ShipHandlePtr ship);
    void SendShipDestroyed(const std::string &target);

private:

    void AwardPointsForShipDestroy(PlayerHandlePtr player);
    void AwardPointsForKill(PlayerHandlePtr player);
    void LogShipDestruction(ShipHandlePtr pShip, const std::string &attacker, const std::string &target, int cur_sector);

    CommTools m_comm_tools;
};

#endif 
