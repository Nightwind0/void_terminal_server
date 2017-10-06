#include <string>
#include <iostream>
#include "VoidServerThread.h"
#include "Socket.h"
#include "SocketException.h"
#include "libpq-fe.h"
#include "void_util.h"
#include "ResourceMaster.h"
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include "LoginHandle.h"
#include "PlayerHandle.h"
#include "ShipTypeHandle.h"
#include "ShipHandle.h"
#include "OutpostHandle.h"
#include <list>
#include <unistd.h>
#include "Universe.h"
#include "VoidCommandMail.h"
#include "VoidCommandCheckMail.h"
#include "VoidCommand.h"
#include "VoidCommandDisplay.h"
#include "VoidCommandTow.h"
#include "VoidCommandMove.h"
#include "VoidCommandPort.h"
#include "VoidCommandStatus.h"
#include "VoidCommandWho.h"
#include "VoidCommandHail.h"
#include "VoidCommandTransmit.h"
#include "VoidCommandDock.h"
#include "VoidCommandBeam.h"
#include "VoidCommandAttack.h"
#include "VoidCommandClaim.h"
#include "VoidCommandComputer.h"
#include "VoidCommandDeploy.h"
#include "VoidCommandReclaim.h"
#include "VoidCommandScan.h"

using namespace std;


void sigpipeHandler(int x)
{
    signal(SIGPIPE, sigpipeHandler);  //reset the signal handler
    ResourceMaster::GetInstance()->Log(EMERGENCY, "SIGPIPE");
}

void VoidServerThread::add_command(std::shared_ptr<VoidCommand>  cmd)
{
    m_commandlist.push_back(cmd);
}

PGresult * VoidServerThread::DBExec(const std::string &query)
{
    ResourceMaster::GetInstance()->Log(DEBUG2, "<DBExec: " + PrepareForSQL(query) + ">");
    return PQexec(m_dbconn, query.c_str());
}

PlayerHandlePtr VoidServerThread::GetPlayer() const
{
    return m_player;
}

LoginHandlePtr VoidServerThread::GetLogin() const
{
    return m_login;
}



void VoidServerThread::HandleSystemMessage(const Message &msg)
{
    ResourceMaster::GetInstance()->Log(DEBUG2, "<System Message=" + msg.GetString());
    if(msg.GetString() == "SHUTDOWN")
    {
	Send(Color()->get(LIGHTRED) + endr + "**** " + Color()->get(BLACK, BG_RED) + "IMMEDIATE SYSTEM SHUTDOWN" + Color()->get(LIGHTRED) + " ****" + endr);
	ResourceMaster::GetInstance()->Log(DEBUG2, "<Shutdown in VST>");
	throw ShutdownException(false);
    }
    if(msg.GetString() == "KILL")
    {
	Send(Color()->get(LIGHTRED) + endr +  "This session has been kicked by a log in from the same user." + endr);
	throw ShutdownException(true);
    }
    if(msg.GetString() == "KICK")
    {
	Send(Color()->get(LIGHTRED) + endr +  "Thank you for calling TRACS. Goodbye..." + endr);
	throw ShutdownException(true);
    }
    if(msg.GetString() == "SHIPEXPLODE")
    {
	Send(Color()->get(BLACK, BG_RED) + "Your ship has been destroyed!!" + endr);
	Send(Color()->get(YELLOW) + "Your escape pod explodes forth from the wreckage carrying you with it!" + endr);

	throw ShipDestroyedException();
    }
    if(msg.GetString() == "PLAYERDEATH")
    {
	Send(Color()->get(BLACK, BG_RED) + "Your escape pod has been destroyed!!!!" + endr);
	Send(Color()->get(LIGHTRED) + "Your player is dead, and you will be disallowed from signing on for 24 hours." + endr);
	
	throw DeathException();
    }
}

void VoidServerThread::HandleLocalMessage(const Message &msg)
{
    ResourceMaster::GetInstance()->Log(DEBUG,"<MSG=" + msg.GetString() + ">");

    if(msg.GetType() == Message::SYSTEM)
	HandleSystemMessage(msg);

    switch(msg.GetType())
    {
    case Message::COMM:
	Send(endr + Color()->get(WHITE) + "# " + Color()->get(LIGHTCYAN) + msg.GetFrom() + Color()->get(WHITE) + ": " + msg.GetString());
	break;
    case Message::TRANSMIT:
	Send(endr + Color()->get(LIGHTGREEN) + "$ " + Color()->get(LIGHTCYAN) + msg.GetFrom() + Color()->get(LIGHTGREEN) + ": " + msg.GetString());
	break;
    case Message::ADMIN:
	Send(endr + Color()->get(GRAY) + " -- " + msg.GetString() + " -- ");
	break;
    case Message::BATTLE:
	Send(endr + Color()->get(LIGHTRED) + "++ " + Color()->get(WHITE) +  msg.GetString() + Color()->get(LIGHTRED) + " ++" + endr);
	break;
    default:
	break;
    }


}

void VoidServerThread::OpenLocalSocket()
{

    std::string sockname = "/tmp/void-" + IntToString((long)this);
    unlink(sockname.c_str());

    m_unixsocket = std::make_unique<UNIXDatagramSocket>(sockname);
    
    m_unixsocket->Create();
    m_unixsocket->Bind(0,sockname);
}

void VoidServerThread::CloseLocalSocket()
{
    m_unixsocket->Close();
    std::string sockname = "/tmp/void-" + IntToString((long)this);
    unlink(sockname.c_str());
}

bool VoidServerThread::PostCommand(const std::string &command,const std::string &arguments)
{

    return DoCommand(command,arguments,true);
}

bool VoidServerThread::DoCommand(const std::string &command, const std::string &arguments, bool frompost)
{

    bool claimed = false;



    for(auto iter =  m_commandlist.begin(); iter != m_commandlist.end(); 
	iter++)
    {
	if(*iter == NULL)
	{
	    ResourceMaster::GetInstance()->Log(EMERGENCY, "<COMMAND WAS NULL!>");
	}
	

	if( (*iter)->ClaimCommand(command))
	{
	
	    claimed = true;


	    if(!(*iter)->HandleCommand(command, arguments, frompost))
	    {
		Send(Color()->get(RED) + "Command syntax was incorrect." + endr + Color()->get(GREEN)
		     + " Correct Syntax: " + Color()->get(WHITE) + (*iter)->GetSyntax() + endr);
	    }
	    
	    break;
	}
    }



    return claimed;
}

void VoidServerThread::OpenDataBaseConnection()
{
  m_dbconn = PQsetdbLogin(NULL,NULL,NULL,NULL,ResourceMaster::GetInstance()->GetDatabaseName().c_str(),"void","tiTVPok?");


    if(PQstatus(m_dbconn) == CONNECTION_BAD)
    {
	std::cerr << PQerrorMessage(m_dbconn) << std::endl;

	throw DBException(PQerrorMessage(m_dbconn));
    }
}

void VoidServerThread::CloseDataBaseConnection()
{
    PQfinish(m_dbconn);
}


bool VoidServerThread::thread_init()
{
    ResourceMaster::GetInstance()->AddSocket(m_socket);
    ResourceMaster::GetInstance()->AddServerThread(this);
    OpenDataBaseConnection();
    OpenLocalSocket();


    return true;
}
void VoidServerThread::thread_destroy()
{
    try {
	ResourceMaster::GetInstance()->Log(DEBUG,"<Thread Destroy>");
	m_socket->Close();
	CloseDataBaseConnection();
	CloseLocalSocket();
	ResourceMaster::GetInstance()->RemoveSocket(m_socket);
	ResourceMaster::GetInstance()->RemoveServerThread(this);
    }
    catch ( SocketException e)
    {
	std::cout << "Caught socket exception." << std::endl;
	ResourceMaster::GetInstance()->Log(ERROR,"<thread_destroy SocketException>");
    }
    catch(DBException db)
    {
	std::cout << "Caught DB exception" << std::endl;
	ResourceMaster::GetInstance()->Log(ERROR,"<thread_destroy DBException>");
    }
}

bool     VoidServerThread::run()
{

    try
    {

	Service();

    }
    catch(SocketException &excep)
    {
	   
    }
    catch(DBException &ex)
    {
	// TODO: Print exception reason to stderr or log
	ResourceMaster::GetInstance()->Log(EMERGENCY, "Execute: " + ex.GetMessage());
	std::cerr << "DB ERROR!!!" << std::endl;

    }
    catch(MissingConfig& exmc)
    {
	ResourceMaster::GetInstance()->Log(EMERGENCY, "Missing Config: " + exmc.getKey());
	std::cerr << "Missing config: " << exmc.getKey() << std::endl;
    }
    catch(ShutdownException &exp)
    {
	return true;
    }
    catch(ShipDestroyedException dsh)
    {
	return true;
    }

    return false;
}


void VoidServerThread::SendClearScreen()
{
    Send(Color()->clear());
}





std::string VoidServerThread::DisplayNews()
{
    std::ostringstream os;
    PGresult *dbresult;
    int rows=0;
    os << endr << Color()->get(RED,BG_WHITE) << "    NEWS    " << endr;
    std::string query = "SELECT DPOSTED,SMESSAGE,BURGENT FROM news WHERE BINACTIVE=FALSE order by nkey asc;";
    
    dbresult = PQexec(m_dbconn, query.c_str());

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	PQclear(dbresult);
	os << Color()->get(BLACK, BG_RED) << "DB Error while reading news" << endr;
	return os.str();
    }

    rows = PQntuples(dbresult);

    for(int i=0;i<rows;i++)
    {
	os  << endr << Color()->get(LIGHTPURPLE) << PQgetvalue(dbresult,i,0) << endr
	    << Color()->get(WHITE) << PQgetvalue(dbresult,i,1) << endr;
    }

    PQclear(dbresult);
    return os.str();
    
    


}


std::string VoidServerThread::DisplayCommands()
{
    std::ostringstream os;

    os << Color()->get(BLACK,BG_WHITE) << "  System Commands  " << Color()->blackout() << endr;

    for(auto iter =  m_commandlist.begin(); iter != m_commandlist.end(); iter++)
    {
	os << Color()->get(PURPLE) << (*iter)->GetSyntax() << Color()->get(GRAY) << " " << (*iter)->GetDescription() << endr;
    }

    return os.str();
}


void VoidServerThread::Log(LOG_SEVERITY severity, const std::string& message) const {
    ResourceMaster::GetInstance()->Log(severity, message);
}




std::string VoidServerThread::ReceiveLine()
{
    const int timeout_secs = std::stoi(ResourceMaster::GetInstance()->GetConfig("idle_timeout_secs"));
    char buffer[65535];
    std::string instring(m_inputbuffer);
    bool gotstring = false;
    std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();

    while(!gotstring)
    {
	// timeout after 10s and check things out no matter what
	Socket::eSelectResult result = m_socket->Select(*m_unixsocket, 10);

	if(result == Socket::eSelectResult::THIS_SOCKET) {
	    std::string input;
	    int bytes = m_socket->Recv(buffer,65535, MSG_DONTWAIT);
	    if(bytes > 0){
		buffer[bytes] = '\0';
		input = buffer;
		
		auto it = input.find("\x0A");
		if(it != string::npos) {
		    instring += input.substr(0,it-1);
		    Log(AUDIT, "Got input: '" + instring + '\'');
		    m_inputbuffer = input.substr(it+1);
		    // Clean this up. Should be a util...
		    if(m_inputbuffer.back() == '\x0D'){
			m_inputbuffer = m_inputbuffer.substr(0, m_inputbuffer.size()-1);
		    }
		    gotstring = true;
		} else {
		    instring += input;
		}
	    }

	}
	else if(result == Socket::eSelectResult::OTHER_SOCKET) {

	    Message msg;
	    std::string from;
	    ResourceMaster::GetInstance()->Log(DEBUG,"<Received MSG...>");

	    from = msg.ReadFromSocket(m_unixsocket);
	    ResourceMaster::GetInstance()->Log(DEBUG,"<MSG From: " + from + ">");

	    HandleLocalMessage(msg);
	} else if(result == Socket::eSelectResult::TIMEOUT) {
	    std::chrono::time_point<std::chrono::system_clock> now_time = std::chrono::system_clock::now();
	    auto difference = std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time);

	    // TODO: Get timeout from config.
	    if(difference.count() >= timeout_secs) {
		throw SocketException(CONNTIMEOUT);
	    }
	}

    }


    return instring;
    
}

#if 0 
std::string VoidServerThread::ReceiveLine()
{
    bool done = false;
    std::string buffer;
    std::string line;
    int newline;

/* ===IDEAS===
 *
 * 1- Pass in a word which contains flags which specify which events you care about,
 *    such as SHIP_DESTROYED, PORT_DESTROYED, etc. Only return the code if the event happens,
 *    otherwise continue to loop to receive data. Also pass in a string for the prompt, so
 *    it can be redisplayed. This way, messages can be printed out and the prompt redisplayed.
 *    Also pass in the color to be used for the text entered at prompt. Then we can print
 *    the input buffer so far.
 *    Problem: How to determine who to send which events to.
 * 2- Don't return anything, let code check to make sure things it needs still exist in the db.
 * 3- Throw exceptions
 * 4- Return any and all codes, make the client loop until it gets success, and check explicitly
 *    for codes. Means they will have to reprompt.
 * 
 */

    while(!done)
    {
	buffer += Receive(false);


	if((newline = buffer.find("\x0A")) != std::string::npos)
	{
	    line = buffer.substr(0,newline -1);
	    done = true;
	}

	if(newline == std::string::npos)
	{
	    if((newline = buffer.find("\x0D")) != std::string::npos)
	    {
		line = buffer.substr(0,newline);
		done = true;
	    }
	}

    }

    return line;
}
#endif


bool VoidServerThread::Login()
{
  
    std::string loginid;
    std::string password;
    std::string query;

    std::ostringstream os("");
    PGresult *dbresult;

       
    Send(Color()->get(BLUE, BG_WHITE) + "Please Login:" + Color()->get(LIGHTBLUE) + ' ');

    loginid = ReceiveLine();

    ResourceMaster::GetInstance()->Log(AUDIT, "<Pre-login: login = " + loginid + ">");

    Send(Color()->get(RED, BG_WHITE) +  "    Password:" + Color()->get(BLACK) + ' ');

    password = ReceiveLine();


    os << "SELECT COUNT(*) FROM LOGIN WHERE SLOGIN = '";
    os << PrepareForSQL(loginid);
    os << "' and SPASSWORD = MD5('";
    os << PrepareForSQL(password);
    os << "');";

//    ResourceMaster::GetInstance()->Log(AUDIT,os.str());

    dbresult = PQexec(m_dbconn,os.str().c_str());
    ResultGuard rg(dbresult);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	throw e;
    }

    if(atoi(PQgetvalue(dbresult,0,0)) != 1)
    {
	return false;
    }

    Text logint(LoginHandle::FieldName(LoginHandle::LOGIN),loginid);
    PrimaryKey key(&logint); 


    for(std::vector<VoidServerThread*>::iterator i = ResourceMaster::GetInstance()->GetServerThreadsBegin();
	i != ResourceMaster::GetInstance()->GetServerThreadsEnd();
	i++)
    {
	// Skip yourself..
	if(*i == this) continue;

	if((*i)->GetLogin() != nullptr)
	{
	    
	    Text login_name = (*i)->GetLogin()->GetLogin();
	    
	    if (login_name == logint)
	    {
		// This login is already in use on the system.
		// Kill the other login so that we can proceed
		ResourceMaster::GetInstance()->Log(WARNING, "<LOGIN: " + loginid + " KILLED EXISTING LOGIN>");
		MessagePtr msg = std::make_shared<Message>(Message::SYSTEM, "KILL");
	       
		ResourceMaster::GetInstance()->
		    SendMessage((*i)->GetLocalSocket(),
				(*i)->GetPlayer()->GetName().GetAsString(),
				msg);

	    
	    }
	}
    }

    m_login = std::make_shared<LoginHandle>(m_dbconn, key, false);
    std::string ip = m_login->GetLastIP().GetAsString();

    ResourceMaster::GetInstance()->Log(DEBUG, "<LOGIN: " + loginid + " Logged In. Last IP = " + ip + ">");

    m_login->Lock();
    m_login->SetLastIP(std::string(m_socket->GetAddress()));
    m_login->SetLastLogin(Universe::GetToday(m_dbconn));
    m_login->SetLogins(m_login->GetLogins() + 1);
    m_login->Unlock();


    return true;

    
}

bool VoidServerThread::RegisterNewLogin()
{
    std::ostringstream os;
    SendClearScreen();
    os << Color()->get(BLACK,BG_WHITE) << "*** Register New Login Name ***" << endr;
    os << Color()->get(GREEN) << "Your Login is not necessarily the same as your player name." << endr << "It will be used to log you in to the system only." << endr;
    os << Color()->get(WHITE) << "Enter your desired login name: " << Color()->get(LIGHTBLUE) ;

    bool done = false;
    std::string loginid;
    std::string password, password2;
    std::string encpassword;
    PGresult *dbresult;
    
    while(!done)
    {
	Send(os.str());
	loginid = ReceiveLine();

	if(loginid == "quit")
	{
	    return false;
	}

	if(loginid.size() < 3) // TODO: Pull this from a config table
	{
	    Send(Color()->get(LIGHTRED) + "Please enter a login with 3 or more characters." + endr);
	    continue;
	}

	if(loginid.size() > 18) // TODO: Pull this from a config table
	{
	    Send(Color()->get(LIGHTRED) + "Please enter a login with fewer than 19 characters." + endr);
	    continue;
	}

	std::string query = "select count(*) from login where slogin = '" + PrepareForSQL(loginid) + "';";

	dbresult = PQexec(m_dbconn, query.c_str());

	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    DBException e("Error checking for existing login: " + std::string(PQresultErrorMessage(dbresult)));
	    PQclear(dbresult);
	    throw e;
	}

	if(atoi(PQgetvalue(dbresult,0,0)) == 1)
	{
	    Send(Color()->get(LIGHTRED) + endr + "Sorry, that login is in use. Try again, or answer 'quit' to give up." + endr);
	}
	else
	{
	    done = true;
	}

	PQclear(dbresult);
	
    }


    done = false;

    while(!done)
    {
	bool passdone = false;

	while(!passdone)
	{
		
	    Send(Color()->get(WHITE) + "Enter your new password:" + Color()->get(BLACK));
	    password = ReceiveLine();

	    if(password.size() < 6) // TODO: Pull from config table
	    {
		Send(Color()->get(LIGHTRED) + "Please enter a password with at least 6 characters." + endr);
	    }
	    else passdone = true;
		
	}
	

	Send(Color()->get(WHITE) + "Enter again to confirm:" + Color()->get(BLACK));
	password2 = ReceiveLine();

	if(password == password2) done = true;
	else Send(Color()->get(LIGHTRED) + "Sorry, passwords do not match. Try again." + endr);

	
    }

    std::string query = "select md5('" + PrepareForSQL(password) + "');";

    dbresult= PQexec(m_dbconn, query.c_str());

    encpassword = PQgetvalue(dbresult,0,0);
    PQclear( dbresult);



    SendWordWrapped(Color()->get(GREEN) + "Please enter a VALID e-mail address. The administrator of this game will" + endr + "be contacting you by this address and verifying it's validity. It will NOT be given out.",80);

    Send(endr + Color()->get(WHITE) + "E-mail address: " + Color()->get(LIGHTBLUE));
    std::string email = ReceiveLine();

        
    Text logint(LoginHandle::FieldName(LoginHandle::LOGIN),loginid);

    PrimaryKey key((Field*)&logint); 
    m_login= std::make_shared<LoginHandle>(m_dbconn, key, true);


    m_login->Lock();

    m_login->SetLogin(loginid);

    m_login->SetPassword(encpassword);
    m_login->SetLogins(0);
    m_login->SetEmail(email);
    m_login->SetFirstLogin(Universe::GetToday(m_dbconn));

    m_login->Insert();
    m_login->Unlock();


    Send(Color()->get(WHITE) + "You have successfully registered. Please log in." + endr);

    return true;
    
}

ShipHandlePtr VoidServerThread::CreateNewShip(int shiptype)
{
    Integer shipti(ShipTypeHandle::FieldName(ShipTypeHandle::NKEY),IntToString(shiptype));
    PrimaryKey stkey(&shipti);

    ShipTypeHandle shiptypeh(m_dbconn, stkey);

//    std::string shiptypename = Color()->get((FGColor)shiptypeh.GetForeColor().GetValue(),(BGColor)shiptypeh.GetBackColor().GetValue()) + shiptypeh.GetManufacturerName() + " " +(std::string)shiptypeh.GetName();

    std::string shiptypename = shiptypeh.GetShipTypeName(Color());

    bool done = false;
    std::string shipname;


    Send(Color()->get(BLUE, BG_WHITE) + "   *** Register New Ship ***   "+ Color()->get(BLACK) + endr);

    Send(Color()->get(GREEN) + "You stare at a shiny new " + shiptypename + Color()->get(WHITE,BG_BLACK) + " " + endr);

    while(!done)
    {
	Send(Color()->get(GREEN) + "What would you like to name it: " + Color()->get(LIGHTBLUE));
	shipname = ReceiveLine();

	if(shipname.size() < 3 || shipname.size() >= 30)
	{
	    Send(Color()->get(LIGHTRED) + "Please choose a name between 3 and 29 characters." + endr);
	    continue;
	}

        Send(Color()->get(GREEN) + "You want it to be called " + Color()->get(LIGHTBLUE)+  shipname + Color()->get(GREEN)+ " (Y/n)? ");

	std::string answer = ReceiveLine();

	if(CompStrings(answer, "yes"))
	{
	    done = true;
	}
	else done = false;
    }

    PGresult *dbresult;

    std::string query = "SELECT nextval('ship_nkey_seq');";

    dbresult = PQexec(m_dbconn, query.c_str());
    
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	DBException e(PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	
	throw e;
    }


    int shipnum = atoi(PQgetvalue(dbresult,0,0));


    Integer shipi(ShipHandle::FieldName(ShipHandle::NKEY),IntToString(shipnum));
    PrimaryKey shipkey(&shipi);

    ShipHandlePtr ship = std::make_shared<ShipHandle>(m_dbconn, shipkey, true);

    ship->Lock();
    ship->SetNkey(shipnum);
    ship->SetName(shipname);
    ship->SetTypeKey(shiptype);
    ship->SetOwner(m_player->GetName());
    ship->SetIsAllianceOwned(false);
    ship->SetHolds(shiptypeh.GetInitHolds());
    ship->SetShields((int)((double)shiptypeh.GetMaxShields() / 5.0));
    ship->Insert();
    ship->Unlock();


    PQclear(dbresult);

    std::ostringstream os;

    os <<Color()->get(GREEN) << "Your ship has been christened " << Color()->get(LIGHTBLUE) << shipname << endr;
    os <<Color()->get(GREEN) << "Ship registration number: " << Color()->get(WHITE) << shipnum << endr;

    Send(os.str());
    

    
    return ship;

}


void VoidServerThread::StartNewPlayer()
{
    Send(Color()->get(BLUE,BG_WHITE) + "   *** Create New Player ***   " + endr + endr);

    Send(Color()->get(GREEN) + "This will be what you are called in the game. It must be at least 3 characters, and no more than 15" + endr);
    
    bool done = false;
    std::string name;

    while(!done)
    {

	Send(Color()->get(WHITE) + "Choose a name: " + Color()->get(LIGHTBLUE));

	name = ReceiveLine();


	if(name.size() <3 || name.size() > 15)
	{
	    Send(Color()->get(LIGHTRED) + "Name bust be between 3 and 15 characters." + endr);
	    done = false;
	    continue;
	}




	PGresult *dbresult;
	std::string query = "SELECT COUNT(*) FROM Player WHERE " + PlayerHandle::FieldName(PlayerHandle::NAME) + "='" + PrepareForSQL(name) + "';";

	dbresult  = PQexec(m_dbconn, query.c_str());
	
	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    DBException e(PQresultErrorMessage(dbresult));
	    PQclear(dbresult);

	    throw e;
	}

	if(atoi(PQgetvalue(dbresult,0,0)) != 0)
	{
	    done = false;

	    Send(Color()->get(LIGHTRED) + "Sorry, that name is in use already. Please choose again." + endr);
	}
	else
	{
	


	    Send(Color()->get(GREEN) + "You want to be called "+ Color()->get(LIGHTBLUE) + name + Color()->get(GREEN) + " (Y/n)?");
	
	    std::string answer = ReceiveLine();

	    if(CompStrings(answer,"yes"))
	    {
		done = true;
	    }
	    else done = false;
	}

	PQclear(dbresult);

    }


    Text namet(PlayerHandle::FieldName(PlayerHandle::NAME),name);
    PrimaryKey key(&namet);

    m_player = std::make_shared<PlayerHandle>(m_dbconn, key, true);

    m_player->Lock();
    m_player->SetName(name);
    m_player->SetLogin(m_login->GetLogin());
    m_player->Insert();
    m_player->Unlock();

    std::ostringstream os;

    os << Color()->get(BLACK, BG_WHITE) << "The story so far..." << Color()->get(BLACK) << endr;
    os <<Color()->get(GREEN) << "You have just graduated from a highly accredited university on Earth." << endr;
    os << "Excited to start a career in space, you pack your bags and get ready to launch!" << endr;
    os << "But not before your parents kindly give you the graduation present you've always dreamed of...";
    os <<Color()->get(LIGHTGREEN) <<  "Your own space ship!!!" << endr;
    os <<Color()->get(BLACK,BG_WHITE) << "Press Enter..." << endr;

    Send(os.str());
    ReceiveLine(); 

    ShipHandlePtr ship  = CreateNewShip(0); // Creates the IM&V Explorer

    ship->Lock();
    ship->SetSector(0);
    ship->Unlock();

    m_player->Lock();
    m_player->SetCurrentShip(ship->GetNkey());
    m_player->SetCredits(10000); // TODO: Get from configuration
    m_player->SetPoints(0);
    m_player->Unlock();
    
    Send(Color()->get(BLACK,BG_WHITE) + "Press Enter..." + Color()->get(BLACK) + endr);
    ReceiveLine();
    Send(Color()->get(GREEN) + "With your new spaceship and 1000 credits to your name, you lift off from Earth!" + endr);

}


void VoidServerThread::ChoosePlayer()
{
    
    std::string query = "SELECT SNAME FROM Player WHERE klogin = '" + PrepareForSQL(m_login->GetLogin()) + "';";
    PGresult *dbresult;

    dbresult = PQexec(m_dbconn, query.c_str());
    ResultGuard rg(dbresult);


    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	throw e;
    }

    int num_players = PQntuples(dbresult);

    if(num_players == 0)
    {
	StartNewPlayer();
	return;
    }

    if(num_players == 1)
    {
	std::string playername = PQgetvalue(dbresult,0,0);
	Text logint(PlayerHandle::FieldName(PlayerHandle::NAME), playername);
	PrimaryKey key(&logint);
	
	m_player = std::make_shared<PlayerHandle>(m_dbconn, key, false);

	ResourceMaster::GetInstance()->Log(DEBUG, "Player: " + PrepareForSQL(m_player->GetName()) + " joins realm.");


	return;
	
    }				 
    else
    {
	// TODO: Player menu for future expansion of possabilities for multiple players
    }


    return;
}




void VoidServerThread::Send(const std::string &str)
{
    m_socket->Send(str.c_str(),str.size(), MSG_NOSIGNAL);
}

void VoidServerThread::SendWordWrapped(const std::string &str, int screen_width)
{
    ostringstream os;

    std::vector<std::string> words = WordsFromString(str);

    int size_so_far =0;

    for(std::vector<std::string>::iterator iter = words.begin();
	iter != words.end();
	iter++)
    {
	if(size_so_far + iter->size() >= screen_width 
	   && iter->size() < screen_width) // if you have a word bigger than screen width, you have to just print the stupid thing
	{
	    os << endr;
	    size_so_far =0;
	}
	
	os << *iter;
        os << ' ';

	size_so_far += iter->size() +1;
    }

    Send(os.str());

}


std::string VoidServerThread::CommandPrompt()
{
    Integer ship = m_player->GetCurrentShip();

 
    Integer shipkey(ShipHandle::FieldName(ShipHandle::NKEY), ship.GetAsString());
    PrimaryKey key(&shipkey);
    ShipHandle shiphandle(m_dbconn, key);

 

    
    std::string sector = shiphandle.GetSector().GetAsString();
    Send(Color()->get(RED) + "[" + Color()->get(WHITE) + sector +  Color()->get(RED) + "] "  +
	 Color()->get(PURPLE) + "(" +GetPlayer()->GetTurnsLeft().GetAsString() + ")"+
	 Color()->get(CYAN) + "Command:" + Color()->get(GRAY) + ' ');

 

    return ReceiveLine();
}



void VoidServerThread::SetTurnsLeft()
{
    std::string sql = "select extract( doy from dlastplay), extract (year from dlastplay), extract(doy from now()), extract(year from now()) from player where sname = '" + GetPlayer()->GetName().GetAsString() 
	+ "';";

    ResourceMaster * RM = ResourceMaster::GetInstance();

    int turns_per_day = atoi(RM->GetConfig("turns_per_day").c_str());

    bool fill_turns = false;

    PGresult *dbresult = DBExec(sql);
    ResultGuard rg(dbresult);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	throw e;
    }


    int play_doy = atoi(PQgetvalue(dbresult,0,0));
    int play_year = atoi(PQgetvalue(dbresult,0,1));
    int cur_doy = atoi(PQgetvalue(dbresult,0,2));
    int cur_year = atoi(PQgetvalue(dbresult,0,3));

//    ResourceMaster::GetInstance()->Log(DEBUG, "PlayDOY: " + IntToString(play_doy) + " CurDOY: " + IntToString(cur_doy));

    if(cur_year > play_year || cur_doy > play_doy)
	fill_turns = true;

    if(fill_turns)
    {
	Send(Color()->get(GREEN) + "Your turns are refreshed." + endr);
	GetPlayer()->Lock();
	GetPlayer()->SetTurnsLeft(turns_per_day); 
	GetPlayer()->Unlock();
    }
		   
}

class LoginGuard {
public:
    LoginGuard(const std::string& player):m_player(player){}
    ~LoginGuard() {
    }

private:
    std::string m_player;
};


void        VoidServerThread::Service()
{
    std::string line;
    std::ostringstream os;
	
    try{
	
	RegisterCommands();

	m_pColor = std::make_shared<ANSIColor>();

	ResourceMaster::GetInstance()->Log(DEBUG,"<Connection from:" + std::string(m_socket->GetAddress()) +  ">");

	signal(SIGPIPE, sigpipeHandler);

	std::string outstring;
	std::string instring;
	std::string input_buffer;

	int tries =0;



	SendClearScreen();
  

	os << Color()->get(LIGHTBLUE);
	os << endr << "Welcome to Void Revolution" <<  endr;
	os << Color()->get(WHITE);
#ifdef VERSION
	os << "Version " << VERSION;
#endif
	
	os << DisplayNews();
	os << Color()->get(GRAY) << "Press Enter." << endr;
	
	
	Send( os.str());
	
	std::string enter = ReceiveLine();
	

	while(!Login())
	{
	    Send(Color()->get(LIGHTRED) + "Wrong password, login not found, or already logged in" + endr);
	    Send(Color()->get(LIGHTBLUE) + "Start an account? (y/N):");

	    std::string answer = ReceiveLine();
	    
	    if(CompStrings(answer,"no"))
	    {
		tries++;
	    }
	    else if(CompStrings(answer,"yes"))
	    {
		if(!RegisterNewLogin())
		    return; // If they fail the registration process, just drop them
	    }

	    if(tries == 3) 
	    {
		ResourceMaster::GetInstance()->Log(DEBUG, "<Unsuccessful Login Recorded>");
		Send(Color()->get(LIGHTRED,BG_WHITE) + "Too many failed login attempts. Goodbye." + endr);
		return; // Drop carrier.
	    }
	}
	

	ChoosePlayer();
	
    }
    catch(const DBException &e) {
	std::string err = "<DB Error during login: " + PrepareForSQL(e.GetMessage()) + ">";
	ResourceMaster::GetInstance()->Log(EMERGENCY, err);
	return;
    }
    catch(SocketException &e)
    {
	ResourceMaster::GetInstance()->Log(ERROR, "Socket Error #" + IntToString(e.GetType()));
	return;
    }

	
    ResourceMaster::GetInstance()->SetThreadForPlayer(this,(std::string) GetPlayer()->GetName());
    LoginGuard lg((std::string) GetPlayer()->GetName());

	
    bool done = false;

    try {

	SetTurnsLeft();

	Send(Color()->get(YELLOW) + "Check your mail? (Y/n) :");
	std::string checkmail = ReceiveLine();
	LOWERCASE(checkmail);
	
	if(CompStrings(checkmail,"yes"))
	{
	    PostCommand("checkmail","");
	}
    }catch(SocketException &e) {
	ResourceMaster::GetInstance()->Log(ERROR, "Socket Error #" + IntToString(e.GetType()));
	return;
    }
	
    if(m_player->GetIsDead())
    {
	std::string checkdeadtime = "select extract(day from age(now(),dlastplay)) from player where sname = '" + m_player->GetName().GetAsString() + "';";
	    
	PGresult *dbresult= DBExec(checkdeadtime);
	ResultGuard rg(dbresult);
	    
	int days = atoi(PQgetvalue(dbresult,0,0));
	    
	    
	if(days < 1)
	{
	    Send(Color()->get(RED) + "Sorry, you are still dead. You will remain dead until 24 hours have passed since your death." + endr);
	    done = true;
	}
	else
	{
	    m_player->Lock();
	    m_player->SetIsDead(false);
	    m_player->Unlock();
	    try{
		Send(Color()->get(LIGHTGREEN) + "You have come back from the dead! You start with a new ship!" + endr);
	    }
	    catch(SocketException e)
	    {
		// An exception here means they dont get their new ship, so we have to set them as dead again
		m_player->Lock();
		m_player->SetIsDead(true);
		m_player->Unlock();
	    }
	    ShipHandlePtr ship = CreateNewShip(0);
	    ship->Lock();
	    ship->SetSector(0);
	    ship->Unlock();
		
	    m_player->Lock();
	    m_player->SetCurrentShip(ship->GetNkey());
	    m_player->Unlock();
		
	}
	    
    }
    

    // not an else, because it could have changed since the if
    if(!m_player->GetIsDead())
    {
	
	m_player->Lock();
	m_player->SetLastPlay(Universe::GetToday(m_dbconn));
	m_player->Unlock();
    
    }
    

    while(!done)
    {

	bool gotinput = false;
	int newline = 0;

	    
	while(!gotinput)
	{
		
	    try{
		Send(endr);
		line = CommandPrompt();
		gotinput = true;
		ResourceMaster::GetInstance()->Log(AUDIT, "<Got command: " + line + ">");
	    }
	    catch(DBException dbe)
	    {
		ResourceMaster::GetInstance()->Log(EMERGENCY,"<DB Exception:" + PrepareForSQL(dbe.GetMessage()) + ">");
		std::cerr << "DB Exception: " << dbe.GetMessage() << std::endl;
		done = true;
		break;
	    }
	    catch(SocketException e)
	    {
		ResourceMaster::GetInstance()->Log(EMERGENCY,"<Socket Exception:" + PrepareForSQL(IntToString(e.GetType())) + ">");
		done = true;
		gotinput = true;
		break;

	    }

	    catch(exception ex)
	    {
		done = true;
		break;
	    }
	    catch(DeathException de)
	    {
		ResourceMaster::GetInstance()->Log(DEBUG,"<Death Exception Caught>");
		Send(Color()->get(GREEN) + "You will now be disconnected." + endr);
		done = true;
		gotinput = true;
		break;
	    }
	    catch(ShipDestroyedException sde)
	    {
		    
	    }
	}
	    
	    
    
	try{


	    os.str("");

	    std::string command; 
	    std::string arguments;

	    std::string::size_type  space;

	    bool claimed;

	    space = line.find(" ");

	    if(space == std::string::npos)
		command = line;
	    else command = line.substr(0,space);

	    if(space != std::string::npos)
		arguments = line.substr(space +1, line.size() - space -1 );
	    else
		arguments = "";

		
	    std::transform (command.begin(), command.end(), command.begin(), ToLower());


	    if(isdigit(command[0]))
	    {
		// Number? Probably a sector number.
		// So change command to move, and arguments to this number..
		arguments = command;
		command = "move";
	    }




	    claimed = DoCommand(command, arguments, false);



	    if(!claimed)
	    {

		if(CompStrings(command, "news"))
		{
		    os << DisplayNews();
		}
		else if(CompStrings(command,"help") || command == "?")
		{
		    os << DisplayCommands(); 
		}
		else if(command == "bye"  || command == "quit" || command == "exit")
		{


			
		    os <<  "\e8Goodbye!!" << endr;

		    done = true;
		}
	    }

	       

	    Send(os.str());


        }
        catch (SocketException &excep)
        {


	    ResourceMaster::GetInstance()->Log(EMERGENCY,"<Socket Exception:" + PrepareForSQL(IntToString(excep.GetType())) + ">");
	    m_socket->Close();
            if (excep.GetType() != NOTCONN)
            {
                //syslog(LOG_ERR, "Socket Exception : %s", (const char *) excep);

//		std::cerr << "Socket Exception: " << (const char*)excep << std::endl;
		
            }
	    done = true;
	    break;
        }
	catch(ShutdownException ex)
	{
	    ResourceMaster::GetInstance()->Log(DEBUG,"<Shutdown Exception Caught>");
	    m_socket->Close();
	    done = true;
	}
	catch(DBException e)
	{
	    ResourceMaster::GetInstance()->Log(EMERGENCY,"<DB Exception:" + PrepareForSQL(e.GetMessage()) + ">");
	    std::cerr << "DB Exception: " << e.GetMessage() << std::endl;
	    done = true;
	    break;
	}
	catch(exception ex)
	{
	    done = true;
	    break;
	}
	catch(ControlException ce)
	{
	    
	}
    }


    if(m_login)
	ResourceMaster::GetInstance()->Log(DEBUG,"<LOGOUT: " + PrepareForSQL(m_login->GetLogin().GetAsString()) + ">");

    ResourceMaster::GetInstance()->Log(DEBUG,"<Done servicing:" + PrepareForSQL(std::string(m_socket->GetAddress())) +  ">");

}


void VoidServerThread::RegisterCommands()
{
    add_command(std::make_shared<VoidCommandDisplay>(this));
    add_command(std::make_shared<VoidCommandMove>(this));
    add_command(std::make_shared<VoidCommandPort>(this));
    add_command(std::make_shared<VoidCommandStatus>(this));
    add_command(std::make_shared<VoidCommandWho>(this));
    add_command(std::make_shared<VoidCommandHail>(this));
    add_command(std::make_shared<VoidCommandTransmit>(this));
    add_command(std::make_shared<VoidCommandDock>(this));
    add_command(std::make_shared<VoidCommandBeam>(this));
    add_command(std::make_shared<VoidCommandTow>(this));
    add_command(std::make_shared<VoidCommandCheckMail>(this));
    add_command(std::make_shared<VoidCommandMail>(this));
    add_command(std::make_shared<VoidCommandAttack>(this));
    add_command(std::make_shared<VoidCommandClaim>(this));
    add_command(std::make_shared<VoidCommandComputer>(this));
    add_command(std::make_shared<VoidCommandDeploy>(this));
    add_command(std::make_shared<VoidCommandReclaim>(this));
    add_command(std::make_shared<VoidCommandScan>(this));

}

VoidServerThread::VoidServerThread(TCPSocketPtr socket):m_socket(socket)
{
}

VoidServerThread::~VoidServerThread()
{

}
