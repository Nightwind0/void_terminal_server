#ifndef VOID_SENTINEL_TOOLS_H
#define VOID_SENTINEL_TOOLS_H

#include "ShipHandle.h"
#include "PlayerHandle.h"
#include "CommTools.h"
#include <list>
#include <string>
#include <libpq-fe.h>

class SentinelTools : public ToolSet
{
public:
    SentinelTools(PGconn * dbconn, DatagramSocket * pSocket);
    virtual ~SentinelTools();


    int GetApplicableSentinelCount(const std::string &target, int maxattack, int cursector);
    int InflictSentinelDamage(int numsentinels, ShipHandle * pTarget, const std::string &sentinel_owner, int shields, int cursector);
    void LogSentinelDamage(const std::string &shipname, const std::string &attacker, int cursector);
    void DeploySentinelsAndUnload(int num, PlayerHandle * pPlayer, ShipHandle * pFrom, int cursector);    
    int SentinelCapacity ( int sector );
    void ReclaimAndReloadSentinels(int num, PlayerHandle * player, ShipHandle * pShip, int cursector );
    int GetPersonalSentinelCount(const std::string &player, int cursector);
private:
    void RemoveSentinels(int num, const std::string &player, int sector);       
    void ClearZeroedSentinels();

    CommTools m_comm_tools;
};

#endif 
