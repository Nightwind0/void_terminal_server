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

using std::min;


VoidCommandAttack::VoidCommandAttack(VoidServerThread *thread):VoidCommand(thread),EscapePodBehavior(thread),SectorCommBehavior(thread), DeleteShipBehavior(thread)
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
    return "Engage another ship in warefare.";
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

double VoidCommandAttack::g_random(int missiles) const
{
    double stddev = 1.75; /// @todo get from config table

    // the multiplier is actually the mean damage for each missile
    double multiplier = 2; /// @todo get from config table
    

    double total = 0;
    for(int i=0;i<missiles;i++)
    {
        total += g_rand() *stddev  + multiplier;
    }
                                                                                
    return total;
}




double VoidCommandAttack::g_rand() const
{    
    double x1, x2, w, y1, y2;
    
    do {
	x1 = 2.0 * ((double)random() / (double)RAND_MAX) - 1.0;
	x2 = 2.0 * ((double)random() / (double)RAND_MAX) - 1.0;
	w = x1 * x1 + x2 * x2;

    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    
    
    return y1;
}



void VoidCommandAttack::remove_sentinels(int num, const std::string &player)
{

/*
    std::string personalsentquery = "select sum(ncount) from sentinels where ksector = '" + IntToString(cursector) + "' and kplayer = '" + player 
	+ "';";
    
    dbresult = get_thread()->DBExec(sentquery);
    
    
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	
	DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	delete ship;
	throw e;
    }

    int personalsentinels = atoi(PQgetvalue(dbresult,0,0));

    PQclear(dbresult);

    std::string alliancesentquery = "select sum(ncount";
*/
    
}




void VoidCommandAttack::KillPlayer(const std::string &player)
{
    ResourceMaster *RM = ResourceMaster::GetInstance();
    std::string stmt = "update player set bdead = TRUE where sname = '" + player + "';";

    PGresult * dbresult = get_thread()->DBExec(stmt);
    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	DBException e("Player death error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    PQclear(dbresult);

    Message explodemsg(Message::SYSTEM, "PLAYERDEATH");
    RM->SendMessage(get_thread()->GetLocalSocket(),player, &explodemsg);

    RM->SendSystemMail(player, (std::string)"Your escape pod was desroyed, and you have died." + endr + "Therefore will not be able to sign on for 24 hours after your death occured." + (std::string)endr);

    
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
    
    // Is ship valid


// Fed alliance?
    // Appreciation calcs

    
//    std::string query = "select sent.ncount, sent.kplayer, sentp.kalliance, p.kalliance, ";


    ResourceMaster *RM = ResourceMaster::GetInstance();
    
    ShipHandle * ship = create_handle_to_current_ship(get_thread()->GetPlayer());

    if(ship->GetNkey() == othership)
    {
	Send(Color()->get(RED) + "You can't attack yourself." + endr);
	delete ship;
	return true;
    }

    int cursector = (int)ship->GetSector();
    bool fedterritory = false;

    /// @todo make a GetTerritory(int sector) method somewhere
    std::string checkterritory = "select kterritory from sectors where nsector = '" + IntToString(cursector) + "';";

    PGresult *checktresult = get_thread()->DBExec(checkterritory);

    if(!PQgetisnull(checktresult,0,0) && atoi(PQgetvalue(checktresult,0,0)) == 0) ///@todo get from config table
    {
	fedterritory = true;
    }

    PQclear(checktresult);


    if(fedterritory)
    {
	Send(Color()->get(RED) + "The federation will not allow warfare in federation space, stand down your attack." + endr);

	return true;
    }


    std::string player = (std::string)get_thread()->GetPlayer()->GetName();


    Send(Color()->get(YELLOW) + "Engaging with ship " + IntToString(othership) + endr);

    bool firstattack = true;

    do
    {


	std::string shipquery = "select s.nmissiles, s.nshields, t.nmaxattack, t.nkey , t.nmaxshields, s.sname, s.nkey from ship s, shiptype t where t.nkey = s.ktype and  s.nkey = '" + IntToString(ship->GetNkey())
	    + "';";

	PGresult *dbresult = get_thread()->DBExec(shipquery);
	
	int missiles = atoi(PQgetvalue(dbresult,0,0));
	int shields = atoi(PQgetvalue(dbresult,0,1));
	int maxattack = atoi(PQgetvalue(dbresult,0,2));
	int shiptype = atoi(PQgetvalue(dbresult,0,3));
	int maxshields = atoi(PQgetvalue(dbresult,0,4));
	std::string shipname = PQgetvalue(dbresult,0,5);
	int shipn = atoi(PQgetvalue(dbresult,0,6));
    
       
	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    
	    DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	    PQclear(dbresult);
	    delete ship;
	    throw e;
	}



	if(missiles < 1)
	{
	    Send(Color()->get(RED) + "You have no more missiles." + endr);
	    // TODO : Just break?
	    break;
	}


	PQclear(dbresult);





	// check if they are still in the sector
	std::string othershipquery = "select s.nmissiles, t.nmaxattack, s.nshields, s.kowner, t.nkey, t.nmaxshields, s.sname from ship s, shiptype t where s.ksector = '" + IntToString(cursector)
	    + "' and s.nkey = '" + IntToString(othership) + "' and s.ktype = t.nkey;";

	Integer othershipi(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(othership));
	PrimaryKey key(&othershipi);
	ShipHandle othershiph(get_thread()->GetDBConn(),key);
	
	dbresult = get_thread()->DBExec(othershipquery);

	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    
	    DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	    PQclear(dbresult);
	    delete ship;
	    throw e;
	}

	if(PQntuples(dbresult) < 1)
	{
	    Send(Color()->get(RED) + "That ship is not in this sector." + endr);
	    PQclear(dbresult);
	    delete ship;
	    return true; // TODO: Break?
	}

	Send(Color()->get(YELLOW) + "You have " + Color()->get(WHITE) + IntToString(missiles) + Color()->get(YELLOW) + " available, with max attack of " +
	     Color()->get(WHITE) + IntToString(maxattack) + endr);
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
	}

	
	int omissiles = atoi(PQgetvalue(dbresult,0,0));
	int omaxattack = atoi(PQgetvalue(dbresult,0,1));
	int oshields = atoi(PQgetvalue(dbresult,0,2));
	std::string oplayer = PQgetvalue(dbresult,0,3);
	int oshiptype = atoi(PQgetvalue(dbresult,0,4));
	int omaxshields = atoi(PQgetvalue(dbresult,0,5));
	std::string oshipname = PQgetvalue(dbresult,0,6);

	PQclear(dbresult);
	
	bool bplayerinship = false;

	std::string playerinship = "select count(1) from player where kcurrentship = '" + IntToString(othership) + "';";

	dbresult = get_thread()->DBExec(playerinship);

	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	    PQclear(dbresult);
	    delete ship;
	    throw e;
	}


	if(atoi(PQgetvalue(dbresult,0,0)) == 1)
	{
	    bplayerinship = true;
	}

	PQclear(dbresult);
	
	
	int nm = atoi(nmstr.c_str());

	if(nm == 0)
	    break;
	if(firstattack)
	{
	    RM->SendSystemMail(oplayer, player + " fired missiles at " + oshipname + endr);
	    firstattack = false;
	}


	if(nm < 0) nm = 0;
	
	if(nm > maxattack)
	{
	    Send(Color()->get(RED) + "Defaulting to maximum attack of " + Color()->get(WHITE) + IntToString(maxattack) + endr);
	    nm = maxattack;
	}

	if(nm > missiles)
	{
	    Send(Color()->get(RED) + "Defaulting to number of remaining missiles : " + Color()->get(WHITE) + IntToString(missiles) + endr);
	    nm = missiles;
	}

	missiles -= nm;

	ship->Lock();
	ship->SetMissiles(missiles);
	ship->Unlock();




	Send(Color()->get(BROWN) + "You fire " + Color()->get(WHITE) + IntToString(nm) + Color()->get(BROWN) + " missiles." + endr);
	RM->Log(DEBUG, "{" + player + " fires " + IntToString(nm) + " missiles on " + oplayer + "}");



	SendMsgToSector(player + " fires " + IntToString(nm) + " missiles at " + oshipname,cursector, player);
	

	int odamage = g_random(nm);


	if(odamage <0) odamage = 0;
	
	
	if(odamage > oshields)
	{
	    odamage = oshields;
	}

	oshields -= odamage;

	othershiph.Lock();
	othershiph.SetShields( oshields );
	othershiph.Unlock();

	try{
	    Send(Color()->get(BROWN) + "Your attack destroys " + Color()->get(BLACK, BG_RED) + IntToString(odamage) + Color()->get(BROWN) + " of their shields." + endr);
	    
	    double percent =(double) oshields / (double)omaxshields;
	    
	    int pc = truncf(percent * 10) * 10;

	    /// @todo need a function to calculate shield percentages 
	    Send(Color()->get(BROWN) + "Their shields are at roughly " + Color()->get(BLACK,BG_WHITE) + IntToString(pc) + '%' + endr);
	}
	catch(SocketException se)
	{
	    // It's vital that a disconnect here doesn't stop things
	}
	

	/// @todo broadcast to sector
	SendMsgToSector(player + " destroys " + IntToString(odamage) + " shields of " + oshipname, cursector, player);




	if(!oshields)
	{


	    Event event(Event::SHIPDESTROYED);
	    event.SetActor(player);
	    event.SetShipType(oshiptype);
	    event.SetShipName(oshipname);

	    get_thread()->LogEvent(event);

	    /// @todo Other player loses points
	    /// @todo : fill the sector with radiation

	    RM->SendSystemMail(oplayer, "Your ship (" + oshipname + ") was destroyed by " + player + endr);
	    SendMsgToSector(player + " destroys " + oshipname + "!!", cursector, player);

	    // OTHER SHIP GOES DOWN

	    delete_ship(othership);	   	    
	   	    
	    if(oshiptype == 10) /// @todo get from configuration table
	    {
		// Escape pod goes byebye
		Send(Color()->get(BLACK, BG_RED) + "You obliterate the escape pod!!");
		Message deathmsg(Message::BATTLE, player + " destroys your escape pod." + endr + "You are finished.");
		
		RM->SendMessage(get_thread()->GetLocalSocket(),oplayer, &deathmsg);
		
		/// @todo mail player
		if(bplayerinship)
		    KillPlayer(oplayer);

	    }
	    else
	    {


		if(bplayerinship)
		{
		    int escapepodnum = CreateEscapePodForPlayer(oplayer);

		    Integer epi(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(escapepodnum));
		    PrimaryKey key(&epi);

		    ShipHandle escapepod(get_thread()->GetDBConn(), key);
		    escapepod.Lock();
		    escapepod.SetSector(cursector);
		    escapepod.Unlock();
		    MoveShipRandomly(&escapepod);
		}
		/// @todo mail player

		get_thread()->GetPlayer()->Lock();
		get_thread()->GetPlayer()->SetPoints( get_thread()->GetPlayer()->GetPoints() + 1000); /// @todo get from config table
		get_thread()->GetPlayer()->Unlock();

		Send(Color()->get(BLACK,BG_RED) + "*** YOU HAVE DESTROYED THE OTHER SHIP ***" + endr);

		if(bplayerinship)
		{
		    
		    Send(Color()->get(BROWN) + "An escape pod explodes off into space." + endr);
		    Message explodemsg(Message::SYSTEM, "SHIPEXPLODE");
		    RM->SendMessage(get_thread()->GetLocalSocket(),oplayer, &explodemsg);
		}

	    }
		    
	    return true;
	}

	std::string sentquery = "select sum(ncount) from sentinels,player where ksector = '" + IntToString(cursector) + "' and kplayer = '" + oplayer 
	    + "' or player.kalliance = (select kalliance from player where sname = '" + oplayer + "');";
	
	dbresult = get_thread()->DBExec(sentquery);


	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    
	    DBException e("Attack DB error: " + std::string(PQresultErrorMessage(dbresult)));
	    PQclear(dbresult);
	    delete ship;
	    throw e;
	}

	int numsentinels = 0;

	if(PQntuples(dbresult) >0)
	{
	    numsentinels = atoi(PQgetvalue(dbresult,0,0));
	}

	PQclear(dbresult);

	int nr = omaxattack;

	if(nr > omissiles)
	    nr = omissiles;


	if(numsentinels)
	{
	    Send(Color()->get(LIGHTRED) + "Sentinels engage to protect the ship!" + endr);
	    Send(Color()->get(BLACK,BG_WHITE) + IntToString(nr) + Color()->get(WHITE) + " sentinels fire at your ship in a counter-attack!" + endr);
	}

	int sentineldamage = g_random(min(omaxattack,numsentinels));


	if(sentineldamage <0) sentineldamage = 0;

	if(numsentinels)
	{
	    Send(Color()->get(WHITE) + "The sentinels increase shield damage by " + Color()->get(LIGHTRED) + IntToString(sentineldamage) + endr);

	    SendMsgToSector("Sentinels engage to protect " + oshipname, cursector, player);

	}

	remove_sentinels(min(omaxattack,numsentinels), oplayer);

	omissiles -= nr;

	othershiph.Lock();
	othershiph.SetMissiles(omissiles);
	othershiph.Unlock();
	

	int damage = g_random(nr);

	if (damage <0) damage =0;
	damage += sentineldamage;

	if(damage > shields)
	{
	    damage = shields;
	}

	shields -= damage;
	
	ship->Lock();
	ship->SetShields(shields);
	ship->Unlock();


	if(nr)
	{
	    Send(Color()->get(WHITE) + endr + "The other ship counterattacks with " + Color()->get(RED) + IntToString(nr) + Color()->get(WHITE) + " missiles." + endr);

	    /// @todo broadcast to sector

	    SendMsgToSector(oshipname + " counters with " + IntToString(nr) + " missiles, doing " + IntToString(damage) + " damage.", cursector, player);


	    double percent =(double) shields / (double)maxshields;
	    percent *= 100;

	    percent = trunc(percent);
	    
	    Send(Color()->get(WHITE) + "You lose " + Color()->get(LIGHTRED) + IntToString(damage) + Color()->get(WHITE) + " shields." + endr);
	    Send(Color()->get(WHITE) + "Your shields are at " + Color()->get(RED, BG_WHITE) + DoubleToString(percent) + '%' + endr);

	    
	}
	else
	{
	    Send(Color()->get(WHITE) + endr + "The other ship is out of missiles." + endr);

	    /// @todo broadcast to sector
	    Message missilemsg(Message::BATTLE, "You are out of missiles.");
	    RM->SendMessage(get_thread()->GetLocalSocket(),oplayer, &missilemsg);
	}

	
	RM->Log(DEBUG,"{" +  oplayer + " counters with " + IntToString(nr) + " missiles." + "}");	


	if(shields < 1)
	{
	    // OUR DEATH


	    Event event(Event::SHIPDESTROYED);
	    event.SetActor(oplayer);
	    event.SetShipType(shiptype);
	    event.SetShipName(shipname);

	    get_thread()->LogEvent(event);

	    
	    get_thread()->GetPlayer()->Lock();
	    get_thread()->GetPlayer()->SetPoints ( get_thread()->GetPlayer()->GetPoints() - 1000);
	    get_thread()->GetPlayer()->Unlock();


	    SendMsgToSector(shipname + " was destroyed by a counterattack!!", cursector, player);

	    Message deathmsg(Message::BATTLE, "You destroyed " + shipname + "!!!" + endr);
	    
	    RM->SendMessage(get_thread()->GetLocalSocket(),oplayer, &deathmsg);

	    /// @todo : fill the sector with radiation

	    // SHIP GOES DOWN

	    
	    delete_ship(shipn);
	    
	    if(shiptype == 10) /// @todo get from configuration table
	    {
		// Escape pod goes byebye
		Send(Color()->get(BLACK, BG_RED) + "Your escape pod was destroyed!!" + endr);

		/// @todo broadcast to sector

		
		/// @todo mail player

		KillPlayer(player);
		
	    }
	    else
	    {

		int escapepodnum = CreateEscapePodForPlayer(player);

		Integer epi(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(escapepodnum));
		PrimaryKey key(&epi);

		ShipHandle escapepod(get_thread()->GetDBConn(), key);
		escapepod.Lock();
		escapepod.SetSector(cursector);
		escapepod.Unlock();
		MoveShipRandomly(&escapepod);

		/// @todo mail player

		/// @todo points for other player

		
		Message explodemsg(Message::SYSTEM, "SHIPEXPLODE");
		RM->SendMessage(get_thread()->GetLocalSocket(),player, &explodemsg);


		


	    }


		
	    return true;
	}
	
	


	
	
    }while(true);

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
    
    return true;
}
