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
    SentinelTools(PGconn * dbconn, DatagramSocketPtr pSocket);
    virtual ~SentinelTools();


    int GetApplicableSentinelCount(const std::string &target, int maxattack, int cursector);
    int InflictSentinelDamage(int numsentinels, ShipHandlePtr pTarget, const std::string &sentinel_owner, int shields, int cursector);
    void LogSentinelDamage(const std::string &shipname, const std::string &attacker, int cursector);
    void DeploySentinelsAndUnload(int num, PlayerHandlePtr pPlayer, ShipHandlePtr pFrom, int cursector);    
    int SentinelCapacity ( int sector );
    void ReclaimAndReloadSentinels(int num, PlayerHandlePtr player, ShipHandlePtr pShip, int cursector );
    int GetPersonalSentinelCount(const std::string &player, int cursector);
private:
    void RemoveSentinels(int num, const std::string &player, int sector);       
    void ClearZeroedSentinels();

    CommTools m_comm_tools;
};

#endif 
