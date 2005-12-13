#include "SentinelTools.h"
#include "ResourceMaster.h"

SentinelTools::SentinelTools(PGconn *dbconn, DatagramSocket * pSocket):ToolSet(dbconn),m_comm_tools(dbconn,pSocket)
{
}

SentinelTools::~SentinelTools()
{
}

void SentinelTools::ClearZeroedSentinels()
{
    std::string delete_stmt = "delete from sentinels where ncount = 0;";
    
    PGresult *dbresult = DBExec(delete_stmt);
    
    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK )
    {
	DBException e("DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }
    
    PQclear(dbresult );


}

void SentinelTools::ReclaimAndReloadSentinels( int num, PlayerHandle * player, ShipHandle * pShip, int cursector )
{

    std::string stmt = "update sentinels set ncount = ncount - " + IntToString(num) + " where ksector = '" + IntToString(cursector)+ 
	"' and kplayer = '" + (std::string)player->GetName() + "';";

    PGresult * dbresult = DBExec(stmt);

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK )
    {
	DBException e("DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    PQclear(dbresult);

    ClearZeroedSentinels();

    pShip->Lock();
    pShip->SetSentinels( pShip->GetSentinels() + num );
    pShip->Unlock();

}

int SentinelTools::SentinelCapacity ( int sector )
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    int maximum = CONFIG_INT(RM,"sentinels_per_sector");

    std::string sql = "select sum(ncount) from sentinels where ksector = '" + IntToString(sector) + "';";

    PGresult * dbresult = DBExec(sql);
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK )
    {
	DBException e("DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    int present=0; 

    if(PQntuples(dbresult) >0)
    {
	// There already are some here.
	present = atoi(PQgetvalue(dbresult,0,0));	
    }

    PQclear(dbresult);

    return maximum - present;
}

void SentinelTools::DeploySentinelsAndUnload(int num, PlayerHandle * pPlayer, ShipHandle * pFrom, int sector)
{
    // At this level, "num" should be verified as acceptable
    std::string player = pPlayer->GetName();

    std::string sql = "select ncount from sentinels where ksector = '" + IntToString ( sector )  + "' and kplayer = '" + PrepareForSQL(player) + "\';";

    PGresult * dbresult = DBExec(sql);
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK )
    {
	DBException e("DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    std::string stmt = "";

    if(PQntuples(dbresult) >0)
    {
	// There already are some here. Therefore, theres an entry to update.
	int numsentinels = atoi(PQgetvalue(dbresult,0,0));	
	
    	numsentinels += num;

	stmt = "update sentinels set ncount = " + IntToString(numsentinels) + " where ksector = " + IntToString(sector) + " and kplayer = '" + player + "';";
    }
    else
    {
	// No entry exists. Create it.
	stmt = "insert into sentinels (ksector,kplayer,ncount) values (" + IntToString(sector) + ",'" + player + "','" +  IntToString(num) + "');";
    }
    
    // Clear last usage
    PQclear(dbresult);
    
    dbresult = DBExec(stmt);
    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK )
    {
	DBException e("DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    PQclear(dbresult);
    
    pFrom->Lock();
    int shipsentinels = pFrom->GetSentinels();
    pFrom->SetSentinels ( shipsentinels - num );
    pFrom->Unlock();
    
}

int SentinelTools::GetPersonalSentinelCount(const std::string &player, int cursector)
{
    std::string sentquery = "select sum(ncount) from sentinels where ksector = '" + IntToString(cursector) + "' and kplayer = '" + player 
	+ "';";
    
    PGresult * dbresult = DBExec(sentquery);
        
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	    
	DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }
    
    int numsentinels = 0;
    
    if(PQntuples(dbresult) >0)
    {
	numsentinels = atoi(PQgetvalue(dbresult,0,0));
    }
    
    PQclear(dbresult);

    return numsentinels;
}


int SentinelTools::GetApplicableSentinelCount(const std::string &target,int maxattack, int cursector)
{
    std::string sentquery = "select ncount from sentinels,player where ksector = '" + IntToString(cursector) + "' and (kplayer = '" + target 
	+ "' or player.kalliance = (select kalliance from player where sname = '" + target + "'));";
    
    PGresult * dbresult = DBExec(sentquery);
        
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	    
	DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }
    
    int numsentinels = 0;
    
    if(PQntuples(dbresult) >0)
    {
	numsentinels = atoi(PQgetvalue(dbresult,0,0));
    }
    
    PQclear(dbresult);

    return std::min(numsentinels,maxattack);
}
void SentinelTools::LogSentinelDamage(const std::string &shipname, const std::string &attacker, int cursector)
{
    m_comm_tools.SendMsgToSector("Sentinels engage to protect " + shipname, cursector, attacker);
}  


int SentinelTools::InflictSentinelDamage(int numsentinels, ShipHandle * pTarget, const std::string &sentinel_owner, int shields, int sector)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    float mean_damage = CONFIG_FLOAT(RM, "mean_sentinel_dmg");
    float deviation = CONFIG_FLOAT(RM, "sentinel_dmg_deviation");

    int damage  = std::max((int)g_random(numsentinels, mean_damage,deviation),0);
    pTarget->Lock();
    pTarget->SetShields ( shields - damage );
    pTarget->Unlock();


    RemoveSentinels ( numsentinels, sentinel_owner, sector );

    return damage;
}

void SentinelTools::RemoveSentinels(int num, const std::string &player, int sector)
{
    
    std::string sentquery = "select ncount,sname from sentinels,player where ksector = '" + IntToString(sector) + "' and (player.sname = '" + player +
	"' or player.kalliance = (select kalliance from player where sname = '"+player+"'));";
    
    
    PGresult * dbresult = DBExec(sentquery);
        
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    int entries = PQntuples(dbresult);

    for(int i = 0; i < entries; i++)
    {
	std::string owner = PQgetvalue(dbresult,i,1);
	int sentinels = atoi(PQgetvalue(dbresult,i,0));

	sentinels = std::min(sentinels,num);

	// Subtract from this one
	std::string update = "update sentinels set ncount = ncount - " + IntToString(sentinels)
	    + " where kplayer = '" + owner + "' and ksector = '"  + IntToString(sector) + "';";
	
	PGresult * updateresult = DBExec(update);
	
	if(PQresultStatus(updateresult) != PGRES_COMMAND_OK)
	{
		DBException e("DB error: " + std::string(PQresultErrorMessage(updateresult)));
		PQclear(updateresult);
		throw e;
	}

	PQclear(updateresult);

	num -= sentinels;
	
	if(num == 0)
	    break;
       
	
    }

    PQclear(dbresult);

    ClearZeroedSentinels();

}
