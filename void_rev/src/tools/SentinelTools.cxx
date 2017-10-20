#include "SentinelTools.h"
#include "ResourceMaster.h"

SentinelTools::SentinelTools(DatabaseConnPtr dbconn, DatagramSocketPtr pSocket):ToolSet(dbconn),m_comm_tools(dbconn,pSocket)
{
}

SentinelTools::~SentinelTools()
{
}

void SentinelTools::ClearZeroedSentinels()
{
    std::string delete_stmt = "delete from sentinels where ncount = 0;";
    
    DBExec(delete_stmt);
}

void SentinelTools::ReclaimAndReloadSentinels( int num, PlayerHandlePtr player, ShipHandlePtr pShip, int cursector )
{

    std::string stmt = "update sentinels set ncount = ncount - " + IntToString(num) + " where ksector = '" + IntToString(cursector)+ 
	"' and kplayer = '" + (std::string)player->GetName() + "';";

    DBExec(stmt);
    ClearZeroedSentinels();

    pShip->Lock();
    pShip->SetSentinels( pShip->GetSentinels() + num );
    pShip->Unlock();

}

int SentinelTools::SentinelCapacity ( int sector )
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    int maximum = config_int(RM,"sentinels_per_sector");

    std::string sql = "select sum(ncount) from sentinels where ksector = '" + IntToString(sector) + "';";

    pqxx::result r = DBExec(sql);

    int present=0; 

    if(r.size() >0)
    {
	// There already are some here.
      present = r[0][0].as<int>();
    }


    return maximum - present;
}

void SentinelTools::DeploySentinelsAndUnload(int num, PlayerHandlePtr pPlayer, ShipHandlePtr pFrom, int sector)
{
    // At this level, "num" should be verified as acceptable
    std::string player = pPlayer->GetName();

    std::string sql = "select ncount from sentinels where ksector = '" + IntToString ( sector )  + "' and kplayer = " + GetDBConn()->quote(player) + ";";

    pqxx::result dbresult = DBExec(sql);
    std::string stmt = "";

    if(dbresult.size() >0)
    {
	// There already are some here. Therefore, theres an entry to update.
      int numsentinels = dbresult[0][0].as<int>(0);
	
      numsentinels += num;

      stmt = "update sentinels set ncount = " + IntToString(numsentinels) + " where ksector = " + IntToString(sector) + " and kplayer = " + GetDBConn()->quote(player) + ";";
    }
    else
    {
	// No entry exists. Create it.
      stmt = "insert into sentinels (ksector,kplayer,ncount) values (" + IntToString(sector) + "," + GetDBConn()->quote(player) + "," +  IntToString(num) + ");";
    }
    
    
    pFrom->Lock();
    int shipsentinels = pFrom->GetSentinels();
    pFrom->SetSentinels ( shipsentinels - num );
    pFrom->Unlock();
    
}

int SentinelTools::GetPersonalSentinelCount(const std::string &player, int cursector)
{
  std::string sentquery = "select sum(ncount) from sentinels where ksector = '" + IntToString(cursector) + "' and kplayer = " + GetDBConn()->quote(player) 
	+ ";";
    
  pqxx::result dbresult = DBExec(sentquery);

    
  int numsentinels = 0;
    
  if(dbresult.size() >0){
    numsentinels = dbresult[0][0].as<int>(0);
  }
  

  return numsentinels;
}


int SentinelTools::GetApplicableSentinelCount(const std::string &target,int maxattack, int cursector)
{
  std::string sentquery = "select ncount from sentinels,player where ksector = " + IntToString(cursector) + " and (kplayer = " + GetDBConn()->quote(target) 
    + " or player.kalliance = (select kalliance from player where sname = " + GetDBConn()->quote(target) + "));";
    
  pqxx::result dbresult = DBExec(sentquery);

    
  int numsentinels = 0;
  
    if(dbresult.size() >0) {
      numsentinels = dbresult[0][0].as<int>(0);
    }
    
    return std::min(numsentinels,maxattack);
}
void SentinelTools::LogSentinelDamage(const std::string &shipname, const std::string &attacker, int cursector)
{
    m_comm_tools.SendMsgToSector("Sentinels engage to protect " + shipname, cursector, attacker);
}  


int SentinelTools::InflictSentinelDamage(int numsentinels, ShipHandlePtr pTarget, const std::string &sentinel_owner, int shields, int sector)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    float mean_damage = config_float(RM, "mean_sentinel_dmg");
    float deviation = config_float(RM, "sentinel_dmg_deviation");

    int damage  = std::max((int)g_random(numsentinels, mean_damage,deviation),0);
    pTarget->Lock();
    pTarget->SetShields ( shields - damage );
    pTarget->Unlock();


    RemoveSentinels ( numsentinels, sentinel_owner, sector );

    return damage;
}

void SentinelTools::RemoveSentinels(int num, const std::string &player, int sector)
{
    
  std::string sentquery = "select ncount,sname from sentinels,player where ksector = " + IntToString(sector) + " and (player.sname = " + GetDBConn()->quote(player) +
    " or player.kalliance = (select kalliance from player where sname = "+GetDBConn()->quote(player)+"));";
    
    
  pqxx::result dbresult = DBExec(sentquery);
        

  for(auto row: dbresult) {
      std::string owner = row[1].as<std::string>();
      int sentinels = row[0].as<int>(0);

      sentinels = std::min(sentinels,num);

      // Subtract from this one
      std::string update = "update sentinels set ncount = ncount - " + IntToString(sentinels)
	+ " where kplayer = " + GetDBConn()->quote(owner) + " and ksector = "  + IntToString(sector) + ";";
	
      pqxx::result updateresult = DBExec(update);

      num -= sentinels;
	
      if(num == 0)
	break;
       
      
    }

    ClearZeroedSentinels();
}
