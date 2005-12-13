
#include "CombatTools.h"
#include <algorithm>
#include <cmath>
#include "void_util.h"
#include "ResourceMaster.h"
#include "Universe.h"

CombatTools::CombatTools(PGconn *dbconn, DatagramSocket * pSocket):ToolSet(dbconn),m_comm_tools(dbconn, pSocket)
{
}

CombatTools::~CombatTools()
{
}


void CombatTools::SendShipDestroyed(const std::string &target)
{
    Message explodemsg(Message::SYSTEM, "SHIPEXPLODE");
    m_comm_tools.SendMessage(target, &explodemsg);

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


void CombatTools::LogShipDestruction(ShipHandle * pShip, const std::string &attacker,
					const std::string &target, int cursector)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    std::string target_ship = pShip->GetName();

    Event event(Event::SHIPDESTROYED);
    event.SetActor(attacker);
    event.SetShipType(pShip->GetType());
    event.SetShipName(target_ship);

    LogEvent(event);
    
    RM->SendSystemMail(target, "Your ship (" + target_ship + ") was destroyed by " + attacker + endr);
    m_comm_tools.SendMsgToSector(attacker + " destroys " + target_ship + "!!", cursector, attacker);

}


void CombatTools::DestroyShip(ShipHandle *pShip, PlayerHandle * pPlayer, PlayerHandle * pTarget, int cursector)
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




int CombatTools::FireMissilesAtShip(int missiles, ShipHandle * pShip, ShipHandle * pTargetShip, int oshields)
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


void CombatTools::KillPlayer(PlayerHandle * pPlayer, PlayerHandle * pTarget)
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

    Message deathmsg(Message::BATTLE, player + " destroys your escape pod." + endr + "You are finished.");
    m_comm_tools.SendMessage(target, &deathmsg);
	    
    Message explodemsg(Message::SYSTEM, "PLAYERDEATH");
    m_comm_tools.SendMessage(target, &explodemsg);
    
    RM->SendSystemMail(target, (std::string)"Your escape pod was destroyed, and you have died." + endr + "Therefore will not be able to sign on for 24 hours after your death occured." + (std::string)endr);
    
}



ShipHandle CombatTools::CreateEscapePodForPlayer(const std::string &player, int cursector)
{

    std::string nextvalsql = "select nextval('ship_nkey_seq');";
    
    PGresult *dbresult = DBExec(nextvalsql);

    std::string nextval = PQgetvalue(dbresult,0,0);
    PQclear(dbresult);

    int shipnum = atoi(nextval.c_str());


    std::string stmt = "insert into ship (nkey, sname, kowner, ktype) values(" + nextval + ",'*POD*','" + player + "',10);";
    // TODO: Get escape pode type from config table

     dbresult = DBExec(stmt);

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	DBException e("Escape pod create error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    PQclear(dbresult);

    std::string sit = "update player set kcurrentship = '" + nextval + "' where sname = '" + player + "';";

    dbresult = DBExec(sit);

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	DBException e("Escape pod create error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    PQclear(dbresult);
    
    Integer epi(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(shipnum));
    PrimaryKey key(&epi);
    
    ShipHandle escapepod(GetDBConn(), key);
    escapepod.Lock();
    escapepod.SetSector(cursector);
    escapepod.Unlock();
    

    return escapepod;
    
}

void CombatTools::MoveShipRandomly(ShipHandle *ship)
{
    const int NUMJUMPS = 3;
    int cursec = (int)ship->GetSector();

    srand(time(NULL));

    /// @todo get value from config table
    for(int i = 0; i < NUMJUMPS; i++)
    {
	std::vector<int> adj = Universe::GetAdjacentSectors(cursec);
	
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


void CombatTools::AwardPointsForShipDestroy(PlayerHandle *player)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    int points = CONFIG_INT(RM,"point_gain_ship_destroy");
    
    player->Lock();
    player->SetPoints( player->GetPoints() + points);
    player->Unlock();
    
}

void CombatTools::AwardPointsForKill(PlayerHandle * player)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    int kill_points = CONFIG_INT(RM,"point_gain_kill");
    player->Lock();
    player->SetPoints( player->GetPoints() + kill_points);
    player->Unlock();
}
