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
    CombatTools(PGconn * dbconn, DatagramSocket * pSocket);
    virtual ~CombatTools();

    double g_random(int) const;
    double g_rand() const;
    
    void DestroyShip(ShipHandle *pShip, PlayerHandle * pPlayer, PlayerHandle * pTarget, int cursector);
    void LogAttack(const std::string &attacker, const std::string &target, 
		   const std::string &target_ship, int missiles, int cursector);
    void LogDamage(const std::string &attacker, const std::string &target_ship, 
		   int damage, int cursector);
    int FireMissilesAtShip(int missiles, ShipHandle * pShip, ShipHandle * pTargetShip, int oshields);
    void KillPlayer(PlayerHandle * pPlayer, PlayerHandle * pTarget);

    int GetApplicableSentinelCount(const std::string target, int maxattack, int cursector);
    int InflictSentinelDamage(int numsentinels, ShipHandle * pTarget, const std::string &sentinel_owner, int shields);
    void LogSentinelDamage(const std::string &shipname, const std::string &attacker, int cursector);


    ShipHandle CreateEscapePodForPlayer(const std::string &player, int cursector);
    void MoveShipRandomly(ShipHandle *ship);
    void SendShipDestroyed(const std::string &target);

private:

    void AwardPointsForShipDestroy(PlayerHandle *player);
    void AwardPointsForKill(PlayerHandle * player);
    void LogShipDestruction(ShipHandle * pShip, const std::string &attacker, const std::string &target, int cur_sector);
    void RemoveSentinels(int num, const std::string &player);       

    CommTools m_comm_tools;
};

#endif 
