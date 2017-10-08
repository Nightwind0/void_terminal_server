
#include "CombatTools.h"
#include <algorithm>
#include <cmath>
#include "void_util.h"
#include "ResourceMaster.h"
#include "Universe.h"

CombatTools::CombatTools(DatabaseConnPtr dbconn, DatagramSocketPtr pSocket):ToolSet(dbconn),m_comm_tools(dbconn, pSocket)
{
}

CombatTools::~CombatTools()
{
}


void CombatTools::SendShipDestroyed(const std::string &target)
{
  MessagePtr explodemsg = std::make_shared<Message>(Message::SYSTEM, "SHIPEXPLODE");
  m_comm_tools.SendMessage(target, explodemsg);

}


void CombatTools::LogDamage(const std::string &attacker, const std::string &target_ship, int damage, int cursector)
{
    m_comm_tools.SendMsgToSector(attacker + " destroys " + IntToString(damage) + " shields of " + target_ship, cursector, attacker);
}

void CombatTools::LogAttack(const std::string &attacker, const std::string &target, const std::string &target_ship, int missiles, int cursector)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    RM->SendSystemMail(target, attacker + " fired missiles at " + target_ship + endr);

    RM->Log(DEBUG, "{" + attacker + " fires " + IntToString(missiles) + " on " + target_ship + "}");

    m_comm_tools.SendMsgToSector(attacker + " fires " + IntToString(missiles) + " missiles at " + target_ship, cursector, attacker);
}


void CombatTools::LogShipDestruction(ShipHandlePtr pShip, const std::string &attacker,
					const std::string &target, int cursector)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    std::string target_ship = pShip->GetName();

    Event event(Event::SHIPDESTROYED);
    event.SetActor(attacker);
    event.SetShipType(static_cast<unsigned char>(pShip->GetType()));
    event.SetShipName(target_ship);

    LogEvent(event);
    
    RM->SendSystemMail(target, "Your ship (" + target_ship + ") was destroyed by " + attacker + endr);
    m_comm_tools.SendMsgToSector(attacker + " destroys " + target_ship + "!!", cursector, attacker);

}


void CombatTools::DestroyShip(ShipHandlePtr pShip, PlayerHandlePtr pPlayer, PlayerHandlePtr pTarget, int cursector)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();

    AwardPointsForShipDestroy ( pPlayer );

    std::string player = pPlayer->GetName();
    std::string target = pTarget->GetName();

    /// Other player loses points
    int point_loss = CONFIG_INT(RM,"point_loss_ship_destroy");
    int points = pTarget->GetPoints();

    pTarget->Lock();
    pTarget->SetPoints ( points - point_loss );
    pTarget->Unlock();

    LogShipDestruction(pShip, player,target,cursector) ;

    /// @todo : fill the sector with radiation
}




int CombatTools::FireMissilesAtShip(int missiles, ShipHandlePtr pShip, ShipHandlePtr pTargetShip, int oshields)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    float mean_damage = CONFIG_FLOAT(RM, "mean_missile_dmg");
    float deviation = CONFIG_FLOAT(RM,"missile_dmg_deviation");

    int odamage = (int)g_random(missiles, mean_damage, deviation);
    
    if(odamage <0) odamage = 0;
        
    if(odamage > oshields)
    {
	odamage = oshields;
    }
    
    oshields -= odamage;

    pShip->Lock();

    int cur_missiles = pShip->GetMissiles() - missiles;

    pShip->SetMissiles ( cur_missiles );
    pShip->Unlock();
    
    pTargetShip->Lock();
    pTargetShip->SetShields( oshields );
    pTargetShip->Unlock();


    return odamage;

}


void CombatTools::KillPlayer(PlayerHandlePtr pPlayer, PlayerHandlePtr pTarget)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();

    int points = CONFIG_INT(RM,"point_loss_kill");

    std::string target = pTarget->GetName();
    std::string player = pPlayer->GetName();

    RM->Log(DEBUG,player + " kills " + target );

    AwardPointsForKill ( pPlayer );

    pTarget->Lock();
    pTarget->SetPoints ( pTarget->GetPoints() - points );
    pTarget->SetIsDead( true );
    pTarget->Unlock();

    MessagePtr deathmsg = std::make_shared<Message>(Message::BATTLE, player + " destroys your escape pod." + endr + "You are finished.");
    m_comm_tools.SendMessage(target, deathmsg);
	    
    MessagePtr explodemsg = std::make_shared<Message>(Message::SYSTEM, "PLAYERDEATH");
    m_comm_tools.SendMessage(target, explodemsg);
    
    RM->SendSystemMail(target, (std::string)"Your escape pod was destroyed, and you have died." + endr + "Therefore will not be able to sign on for 24 hours after your death occured." + (std::string)endr);
    
}



ShipHandlePtr CombatTools::CreateEscapePodForPlayer(const std::string &player, int cursector)
{

    std::string nextvalsql = "select nextval('ship_nkey_seq');";
    
    pqxx::result dbresult = DBExec(nextvalsql);

    int shipnum = dbresult[0][0].as<int>();

    std::string stmt = "insert into ship (nkey, sname, kowner, ktype) values(" + std::to_string(shipnum) + ",'*POD*','" + player + "',10);";
    // TODO: Get escape pode type from config table


    dbresult = DBExec(stmt);


    std::string sit = "update player set kcurrentship = '" + std::to_string(shipnum) + "' where sname = '" + player + "';";
    dbresult = DBExec(sit);
        
    std::shared_ptr<Integer> epi = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(shipnum));
    PrimaryKey key(epi);
    
    ShipHandlePtr escapepod = std::make_shared<ShipHandle>(GetDBConn(), key);
    escapepod->Lock();
    escapepod->SetSector(cursector);
    escapepod->Unlock();
    

    return escapepod;
    
}

void CombatTools::MoveShipRandomly(ShipHandlePtr ship)
{
    const int NUMJUMPS = 3;
    int cursec = (int)ship->GetSector();

    /// @todo get value from config table
    for(int i = 0; i < NUMJUMPS; i++)
    {
	std::vector<Sector> adj = Universe::GetAdjacentSectors(cursec);
	
	int r = rand() % adj.size();

	cursec = adj[r];
    }

    /// @todo find a way to actually process moves here, rather than just magically teleporting.
    /// @todo there needs to be a method in the base class for the user and alien threads that 
    /// provides for everything involved in moving a ship (aside from turns?)
    ship->Lock();
    ship->SetSector(cursec);
    ship->Unlock();
}


void CombatTools::AwardPointsForShipDestroy(PlayerHandlePtr player)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    int points = CONFIG_INT(RM,"point_gain_ship_destroy");
    
    player->Lock();
    player->SetPoints( player->GetPoints() + points);
    player->Unlock();
    
}

void CombatTools::AwardPointsForKill(PlayerHandlePtr player)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    int kill_points = CONFIG_INT(RM,"point_gain_kill");
    player->Lock();
    player->SetPoints( player->GetPoints() + kill_points);
    player->Unlock();
}
