#include "VoidCommandAttack.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <math.h>
#include "ResourceMaster.h"
#include "SocketException.h"
#include <algorithm>
#include <memory>

using std::min;


VoidCommandAttack::VoidCommandAttack(VoidServerThread *thread):VoidCommand(thread),m_combat_tools(thread->GetDatabaseConn(), thread->GetLocalSocket()), m_ship_tools(thread->GetDatabaseConn()),m_sentinel_tools(thread->GetDatabaseConn(),thread->GetLocalSocket())
{
}
VoidCommandAttack::~VoidCommandAttack()
{
}

string VoidCommandAttack::GetCommandString()const
{
    return "Attack";
}

string VoidCommandAttack::GetDescription()const
{
    return "Engage another ship in warfare.";
}

string VoidCommandAttack::GetSyntax()const
{
    return "attack <ship number>";
}

    
bool VoidCommandAttack::ClaimCommand(const string &command)
{
    if (CompStrings(command,"attack")) return true;

    return false;
}



int VoidCommandAttack::PromptNumberOfMissiles(int maxattack, int missiles_available)
{
    Send(Color()->get(YELLOW) + "You have " + Color()->get(WHITE) + IntToString(missiles_available) + Color()->get(YELLOW) 
	 + " available, with max attack of " + Color()->get(WHITE) + IntToString(maxattack) + endr);
    Send(Color()->get(YELLOW) + "How many missiles to fire? " + Color()->get(WHITE));
    
    std::string nmstr;
    
    try{
	nmstr = ReceiveLine();
    }
    catch(ControlException e)
    {
	
    }
    catch(SocketException se)
    {
	// Cant stop this train now.
    }
    

    int nm = atoi(nmstr.c_str());

    if(nm == 0)
	return true;
    
    if(nm < 0) nm = 0;
    
    if(nm > maxattack)
    {
	Send(Color()->get(RED) + "Defaulting to maximum attack of " + Color()->get(WHITE) + IntToString(maxattack) + endr);
	nm = maxattack;
    }
    
    if(nm > missiles_available)
    {
	Send(Color()->get(RED) + "Defaulting to number of remaining missiles : " + Color()->get(WHITE) + IntToString(missiles_available) + endr);
	nm = missiles_available;
    }


    return nm;


}


bool VoidCommandAttack::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{                                                                                                           

    if(arguments.size() < 1) return false;

    int ship = atoi(arguments.c_str());
    
    return CommandAttack(ship);
}

// Return false for command failure (bad arguments..)
bool VoidCommandAttack::CommandAttack(int othership)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();

    int escapepod_num = CONFIG_INT(RM,"escape_pod_nkey");
    
    ShipHandlePtr  ship (create_handle_to_current_ship(get_player()));

    if(ship->GetNkey() == othership)
    {
	Send(Color()->get(RED) + "You can't attack yourself." + endr);
	return true;
    }
    
    if( ship->GetIsCloaked())
    {
	Send(Color()->get(RED) + "You cannot attack from a cloaked ship." + endr);
	return true;
    }

    int cursector = (int)ship->GetSector();
    bool fedterritory = false;

    /// @todo make a GetTerritory(int sector) method somewhere
    std::string checkterritory = "select kterritory from sectors where nsector = " + IntToString(cursector) + ";";

    pqxx::result result = get_thread()->DBExec(checkterritory);

    if(!result[0][0].is_null() && result[0][0].as<int>(0) == 0) ///@todo get from config table
    {
	fedterritory = true;
    }

    if(fedterritory)
    {
	Send(Color()->get(RED) + "The federation will not allow warfare in federation space, stand down your attack." + endr);

	return true;
    }


    std::string player = (std::string)get_player()->GetName();

    Send(Color()->get(YELLOW) + "Engaging with ship " + IntToString(othership) + endr);

    std::string shipquery = "select s.nmissiles, s.nshields, t.nmaxattack, t.nkey , t.nmaxshields, s.sname, s.nkey from ship s, shiptype t where t.nkey = s.ktype and  s.nkey = " + IntToString(ship->GetNkey())
	+ ';';
    
    pqxx::result dbresult = get_thread()->DBExec(shipquery);
    
    int missiles = dbresult[0][0].as<int>(0);
    int shields = dbresult[0][1].as<int>(0);
    int maxattack = dbresult[0][2].as<int>(0);
    int shiptype = dbresult[0][3].as<int>();
    int maxshields = dbresult[0][4].as<int>(0);
    std::string shipname = dbresult[0][5].as<std::string>();
    int shipn = dbresult[0][6].as<int>();
    
    
    if(missiles < 1)
    {
	Send(Color()->get(RED) + "You have no more missiles." + endr);

	return true;
    }

    // check if they are in the sector
    std::string othershipquery = "select s.nmissiles, t.nmaxattack, s.nshields, s.kowner, t.nkey, t.nmaxshields,"
	" s.sname from ship s, shiptype t where s.ksector = '" + IntToString(cursector)
	+ "' and s.nkey = '" + IntToString(othership) + "' and s.ktype = t.nkey;";
    
    std::shared_ptr<Integer> othershipi = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(othership));
    PrimaryKey key(othershipi);
    ShipHandlePtr othershiph = std::make_shared<ShipHandle>(get_thread()->GetDatabaseConn(),key);
	
    dbresult = get_thread()->DBExec(othershipquery);
      
    if(dbresult.size() < 1)
    {
	Send(Color()->get(RED) + "That ship is not in this sector." + endr);
	return true; 
    }
    
    int omissiles = dbresult[0][0].as<int>(0);
    int omaxattack = dbresult[0][1].as<int>(0);
    int oshields = dbresult[0][2].as<int>(0);
    std::string oplayer = dbresult[0][3].as<std::string>();
    int oshiptype = dbresult[0][4].as<int>();
    int omaxshields = dbresult[0][5].as<int>(0);
    std::string oshipname = dbresult[0][6].as<std::string>();
    bool targetdestroyed = false;
    
    std::shared_ptr<Text> namet = std::make_shared<Text>(PlayerHandle::FieldName(PlayerHandle::NAME),oplayer);
    PrimaryKey okey(namet);

    PlayerHandlePtr otherplayer = std::make_shared<PlayerHandle>(get_thread()->GetDatabaseConn(), okey, true);
 
    bool bplayerinship =  (otherplayer->GetCurrentShip() == othership);
    int nm = PromptNumberOfMissiles(maxattack,missiles);
	
    missiles -= nm;
        
    Send(Color()->get(BROWN) + "You fire " + Color()->get(WHITE) + IntToString(nm) + Color()->get(BROWN) + " missiles." + endr);
    
    m_combat_tools.LogAttack ( player, oplayer, oshipname, nm, cursector);

    int odamage = m_combat_tools.FireMissilesAtShip ( nm, ship, othershiph, oshields );
    
    try{
	Send(Color()->get(BROWN) + "Your attack destroys " + Color()->get(BLACK, BG_RED) + IntToString(odamage) 
	     + Color()->get(BROWN) + " of their shields." + endr);
	
	
	double percent = (double) (oshields - odamage) / (double)omaxshields;    
	int pc = truncf(percent * 10) * 10;
	
	/// @todo need a function to calculate shield percentages 
	Send(Color()->get(BROWN) + "Their shields are at roughly " + Color()->get(BLACK,BG_WHITE) + IntToString(pc) + '%' + endr);
    }
    catch(SocketException se)
    {
	// It's vital that a disconnect here doesn't stop things
    }
    
    m_combat_tools.LogDamage ( player, oshipname, odamage, cursector );

    oshields -= odamage;

    if(oshields < 1 )
    {
	targetdestroyed = true;

	m_combat_tools.DestroyShip ( othershiph, get_thread()->GetPlayer(), otherplayer, cursector);

	m_ship_tools.DeleteShip(othership);	   	    

	if(oshiptype == escapepod_num)
	{
	    // Escape pod goes byebye
	    Send(Color()->get(BLACK, BG_RED) + "You obliterate the escape pod!!");

	    if(bplayerinship)
		m_combat_tools.KillPlayer( get_thread()->GetPlayer(), otherplayer);
	    
	}
	else
	{

	    Send(Color()->get(BLACK,BG_RED) + "*** YOU HAVE DESTROYED THE OTHER SHIP ***" + endr);
	    if(bplayerinship)
	    {
		ShipHandlePtr escapepod = m_combat_tools.CreateEscapePodForPlayer(oplayer, cursector);
		m_combat_tools.MoveShipRandomly(escapepod);

		m_combat_tools.SendShipDestroyed(oplayer);
	    		    
		Send(Color()->get(BROWN) + "An escape pod explodes off into space." + endr);
	    }
		
	}
    }

// Sentinel CounterAttack phase. Happens regardless of the other ship being destroyed or fleeing
    int numcountersentinels = m_sentinel_tools.GetApplicableSentinelCount(oplayer,omaxattack,cursector);
    int countersentineldamage = 0;
    
    if(numcountersentinels)
    {
	Send(Color()->get(LIGHTRED) + "Sentinels engage to attempt to protect the ship!" + endr);
	Send(Color()->get(BLACK,BG_WHITE) + IntToString(numcountersentinels) + Color()->get(WHITE) + 
	     " sentinels fire at your ship in a counter-attack!" + endr);
	    
	countersentineldamage = m_sentinel_tools.InflictSentinelDamage(numcountersentinels, ship, oplayer, shields, cursector);
	m_sentinel_tools.LogSentinelDamage(oshipname,player,cursector);
	    
	Send(Color()->get(WHITE) + "The sentinels increase shield damage by " + Color()->get(LIGHTRED) + IntToString(countersentineldamage) + endr);

	    
    }

    shields -= countersentineldamage;

    
// CounterAttack phase

    if( !targetdestroyed ) 
    {

	int countermissiles = std::min(omissiles,omaxattack);
	int counterattack = m_combat_tools.FireMissilesAtShip(countermissiles,othershiph,ship,shields);

	m_combat_tools.LogAttack(oplayer,player,shipname,countermissiles,cursector);
	
	omissiles -= countermissiles;

	shields -= counterattack;
	
	if(counterattack)
	{
	    Send(Color()->get(WHITE) + endr + "The other ship counterattacks with " + Color()->get(RED) 
		 + IntToString(countermissiles) + Color()->get(WHITE) + " missiles." + endr);
	    
	    m_combat_tools.LogDamage(oplayer,shipname,counterattack,cursector);
	    
	    double percent =(double) shields / (double)maxshields;
	    percent *= 100;
	    
	    percent = trunc(percent);
	    
	    Send(Color()->get(WHITE) + "You lose " + Color()->get(LIGHTRED) + IntToString(counterattack) + Color()->get(WHITE) + " shields." + endr);
	    Send(Color()->get(WHITE) + "Your shields are at " + Color()->get(RED, BG_WHITE) + DoubleToString(percent) + '%' + endr);
	    
	}
	else
	{
	    Send(Color()->get(WHITE) + endr + "The other ship is out of missiles." + endr);
	    
	    /// @todo broadcast to sector
	    MessagePtr missilemsg = std::make_shared<Message>(Message::BATTLE, "You are out of missiles.");
	    RM->SendMessage(get_thread()->GetLocalSocket(),oplayer, missilemsg);
	}
	
	
	RM->Log(DEBUG,"{" +  oplayer + " counters with " + IntToString(countermissiles) + " missiles." + "}");	
    }
	
    if(shields < 1)
    {
	// OUR SHIP BLEW UP! OH NOES!

	m_combat_tools.DestroyShip(ship,otherplayer,get_thread()->GetPlayer(),cursector);	    

	MessagePtr deathmsg = std::make_shared<Message>(Message::BATTLE, "You destroyed " + shipname + "!!!" + endr);
	RM->SendMessage(get_thread()->GetLocalSocket(),oplayer, deathmsg);
	    
	m_ship_tools.DeleteShip(shipn);
	    
	if(shiptype == escapepod_num)
	{
	    // Escape pod goes byebye
	    Send(Color()->get(BLACK, BG_RED) + "Your escape pod was destroyed!!" + endr);

	    m_combat_tools.KillPlayer(get_thread()->GetPlayer(), otherplayer);
		
	}
	else
	{

	    ShipHandlePtr escapepod = m_combat_tools.CreateEscapePodForPlayer(player, cursector);

	    m_combat_tools.MoveShipRandomly(escapepod);

	    Send(Color()->get(RED) + "Your escape pod explodes off into space." + endr);
		
	    m_combat_tools.SendShipDestroyed(player);
	}
		
	return true;
    }
	
    return true;
}	


	
	
    // check sentinel count (it could change dynamically)
    // in fact, re-check everything here.. do the whole SQL again
    //nm = choose number of missiles <= maxattack
    //if nm = 0 break
    // subtract gassuan_rand(10 * nm) from other ships shields
    // is other ship destroyed? other death
    // nr = number equal to the lower of the other ships max attack and the other ships number of missiles
    // if there are sentinels owned by the ships owner or alilance in the sector
    // then nr += the lower of the other ships max attack and the number of applicable sentinels left
    // shields -= gaussian_rand() * 10 * nr
    // Send what happened to the other player in case they are online
    // are we dead? death
    //loop

    // other death:
    // appreciation
    // points for you
    // negative points for them
    // RM->ExplodeShip(ship)
    // if (escape pod) die
    // else escape pode code
    // Send mail to player
    // log event

    // death:
    // if escape pod die
    // else escape pod code
    // Send message to player in case they are online, notifying of the kill
    // log event
    

