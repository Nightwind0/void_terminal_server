#include <string>
#include <iostream>
#include "VoidServerThread.h"
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

const char * VoidServerThread::endr = "\n\r";


using namespace std;


void sigpipeHandler(int x)
{
    signal(SIGPIPE, sigpipeHandler);  //reset the signal handler
    ResourceMaster::GetInstance()->Log(EMERGENCY, "SIGPIPE");

}

void VoidServerThread::add_command(VoidCommand * cmd)
{
    m_commandlist.push_back(cmd);
}

PGresult * VoidServerThread::DBExec(const std::string &query)
{
    return PQexec(m_dbconn, query.c_str());
}

PlayerHandle * VoidServerThread::GetPlayer() const
{
    return m_player;
}

LoginHandle * VoidServerThread::GetLogin() const
{
    return m_login;
}

void VoidServerThread::HandleLocalMessage(const Message &msg)
{
    ResourceMaster::GetInstance()->Log(DEBUG,"<MSG=" + msg.GetString() + ">");


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
    default:
	break;
    }


}

void VoidServerThread::OpenLocalSocket()
{
      m_usocketid = socket(AF_UNIX, SOCK_DGRAM, 0);
      sockaddr_un addr; 

      std::string sockname = "/tmp/void-" + IntToString((int)this);
      unlink(sockname.c_str());
      strcpy(addr.sun_path, sockname.c_str()); 
      addr.sun_family = AF_UNIX; 
      bind(m_usocketid, (struct sockaddr *) &addr, strlen(addr.sun_path) + 
	    sizeof (addr.sun_family));
}

void VoidServerThread::CloseLocalSocket()
{
    close(m_usocketid);
    std::string sockname = "/tmp/void-" + IntToString((int)this);
    unlink(sockname.c_str());
}

bool VoidServerThread::PostCommand(const std::string &command,const std::string &arguments)
{

    return DoCommand(command,arguments,true);
}

bool VoidServerThread::DoCommand(const std::string &command, const std::string &arguments, bool frompost)
{

    bool claimed = false;



    for(std::list<VoidCommand*>::iterator iter =  m_commandlist.begin(); iter != m_commandlist.end(); 
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
    m_dbconn = PQsetdbLogin(NULL,NULL,NULL,NULL,"void","void","tiTVPok?");


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


void *      VoidServerThread::Execute()
{
    while(bServerActive)
    {
        TCPSocket   *pcClientSocket = NULL;
        try
        {



            KillSurplusThreads();
            pcClientSocket = AcceptClient();
	    ResourceMaster::GetInstance()->AddSocket(pcClientSocket);
	    ResourceMaster::GetInstance()->AddServerThread(this);
            CreateExtraThreads();
	    OpenDataBaseConnection();
	    OpenLocalSocket();
            ServiceClient(pcClientSocket);
	    CloseDataBaseConnection();
	    CloseLocalSocket();
	    ResourceMaster::GetInstance()->RemoveSocket(pcClientSocket);
	    ResourceMaster::GetInstance()->RemoveServerThread(this);
            delete pcClientSocket;
        }
        catch (KillThreadExceptions &excep)
        {
            switch (excep)
            {
                case errFatalError:     delete pcClientSocket;
                case errServerDown:
                case errTooManyThreads: return NULL;
                case errClientError:    delete pcClientSocket;
            }
        }
	catch(DBException &ex)
	{
	    // TODO: Print exception reason to stderr or log
	    ResourceMaster::GetInstance()->Log(EMERGENCY, "Execute: " + ex.GetMessage());
	    std::cerr << "DB ERROR!!!" << std::endl;
	    delete pcClientSocket;
	}
    }

}


void VoidServerThread::SendClearScreen()
{

    std::string clear_string = "\e7" + Color()->get(WHITE) + Color()->clear();


    m_socket->SendData(clear_string.c_str(), clear_string.size());
    
}





std::string VoidServerThread::DisplayNews()
{
    std::ostringstream os;
    PGresult *dbresult;
    int rows=0;
    os << endr << Color()->get(RED,BG_WHITE) << "    NEWS    " << endr;
    std::string query = "SELECT DPOSTED,SMESSAGE,BURGENT FROM news WHERE BINACTIVE=FALSE;";
    
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

    for(std::list<VoidCommand*>::iterator iter =  m_commandlist.begin(); iter != m_commandlist.end(); iter++)
    {
	os << Color()->get(PURPLE) << (*iter)->GetSyntax() << Color()->get(GRAY) << " " << (*iter)->GetDescription() << endr;
    }

    return os.str();
}








std::string VoidServerThread::Receive(bool block)
{

    int  iBytesTransferred;
    char pcBuffer[65535];
    std::string instring;
    bool gotstring = false;
//    iBytesTransferred = m_socket->RecvData(pcBuffer, 65535, (block)?MSG_DONTWAIT:0);


    while(!gotstring)
    {


	FD_ZERO(&m_readfs);
	FD_SET(m_usocketid,&m_readfs);
	FD_SET(m_socket->GetSockID(), &m_readfs);
	
	int selectval = select(max(m_usocketid,m_socket->GetSockID())+1,&m_readfs,NULL,NULL,NULL);
		
	if(FD_ISSET(m_socket->GetSockID(), &m_readfs))
	{

	    
	    iBytesTransferred = m_socket->RecvData(pcBuffer,65535,0);
	    
	    pcBuffer[iBytesTransferred] = 0;
	    
	    instring = pcBuffer;
	    
	    gotstring = true;

	}

	else if(FD_ISSET(m_usocketid,&m_readfs))
	{

	    Message msg;
	    std::string from;
	    ResourceMaster::GetInstance()->Log(DEBUG,"<Received MSG...>");

	    from = msg.ReadFromSocket(m_usocketid);
	    ResourceMaster::GetInstance()->Log(DEBUG,"<MSG From: " + from + ">");

	    HandleLocalMessage(msg);
	}

    }


    return instring;
    
}

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

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	throw e;
    }

    if(atoi(PQgetvalue(dbresult,0,0)) != 1)
    {
	PQclear(dbresult);
	return false;
    }

    PQclear(dbresult);

    Text logint(LoginHandle::FieldName(LoginHandle::LOGIN),loginid);
    PrimaryKey key(&logint); 




    for(std::vector<VoidServerThread*>::iterator i = ResourceMaster::GetInstance()->GetServerThreadsBegin();
	i != ResourceMaster::GetInstance()->GetServerThreadsEnd();
	i++)
    {
	// Skip yourself..
	if(*i == this) continue;

	if((*i)->GetLogin() != NULL)
	{
	    
	    Text login_name = (*i)->GetLogin()->GetLogin();
	    
	    if (login_name == logint)
	    {
		Send(Color()->get(LIGHTRED) + "Sorry, you are already logged in." + endr);
		ResourceMaster::GetInstance()->Log(DEBUG, "<LOGIN : " + loginid + " tried to log in multiple times.>");
		return false;
	    }
	}
    }

    m_login= new LoginHandle(m_dbconn, key, false);
    std::string ip = m_login->GetLastIP().GetAsString();

    ResourceMaster::GetInstance()->Log(DEBUG, "<LOGIN: " + loginid + " Logged In. Last IP = " + ip + ">");

    m_login->Lock();
    m_login->SetLastIP(std::string(m_socket->RemoteIPAddress().GetAddressString()));
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



    Send(Color()->get(GREEN) + "Please enter a VALID e-mail address. The administrator of this game will" + endr + "be contacting you by this address and verifying it's validity.");

    Send(endr + Color()->get(WHITE) + "E-mail address: " + Color()->get(LIGHTBLUE));
    std::string email = ReceiveLine();

        
    Text logint(LoginHandle::FieldName(LoginHandle::LOGIN),loginid);

    PrimaryKey key((Field*)&logint); 
    m_login= new LoginHandle(m_dbconn, key, true);


    m_login->Lock();

    m_login->SetLogin(loginid);

    m_login->SetPassword(encpassword);
    m_login->SetLogins(0);
    m_login->SetEmail(email);
    m_login->SetFirstLogin(Universe::GetToday(m_dbconn));

    m_login->Insert();
    m_login->Unlock();

    delete m_login; // This should both insert and lock
    Send(Color()->get(WHITE) + "You have successfully registered. Please log in." + endr);

    return true;
    
}

ShipHandle* VoidServerThread::CreateNewShip(int shiptype)
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

    std::string query = "SELECT MAX(NKEY)+1 FROM Ship;";

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

    ShipHandle *ship = new ShipHandle(m_dbconn, shipkey, true);

    ship->Lock();
    ship->SetNkey(shipnum);
    ship->SetName(shipname);
    ship->SetTypeKey(shiptype);
    ship->SetOwner(m_player->GetName());
    ship->SetIsAllianceOwned(false);
    ship->SetHolds(shiptypeh.GetInitHolds());
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

    Send(Color()->get(GREEN) + "This will be what you are called in the game. It must be at least 3 characters, and no more than 25" + endr);
    
    bool done = false;
    std::string name;

    while(!done)
    {

	Send(Color()->get(WHITE) + "Choose a name: " + Color()->get(LIGHTBLUE));

	name = ReceiveLine();


	if(name.size() <3 || name.size() > 25)
	{
	    Send(Color()->get(LIGHTRED) + "Name bust be between 3 and 25 characters." + endr);
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

    m_player = new PlayerHandle(m_dbconn, key, true);

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

    ShipHandle * ship;

    ship = CreateNewShip(0); // Creates the IM&V Explorer

    ship->Lock();
    ship->SetSector(0);
    ship->Unlock();

    m_player->Lock();
    m_player->SetCurrentShip(ship->GetNkey());
    m_player->SetCredits(10000); // TODO: Get from configuration
    m_player->Unlock();
    
    Send(Color()->get(BLACK,BG_WHITE) + "Press Enter..." + Color()->get(BLACK) + endr);
    ReceiveLine();
    Send(Color()->get(GREEN) + "With your new spaceship and 1000 credits to your name, you lift off from Earth!" + endr);


    delete ship;
    

}


void VoidServerThread::ChoosePlayer()
{
    
    std::string query = "SELECT SNAME FROM Player WHERE klogin = '" + PrepareForSQL(m_login->GetLogin()) + "';";
    PGresult *dbresult;

    dbresult = PQexec(m_dbconn, query.c_str());


    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	PQclear(dbresult);
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
	PQclear(dbresult);
	Text logint(PlayerHandle::FieldName(PlayerHandle::NAME), playername);
	PrimaryKey key(&logint);
	
	m_player = new PlayerHandle(m_dbconn, key, false);

	ResourceMaster::GetInstance()->Log(DEBUG, "Player: " + PrepareForSQL(m_player->GetName()) + " joins realm.");

	m_player->Lock();
	m_player->SetLastPlay(Universe::GetToday(m_dbconn));
	m_player->Unlock();

	ShipHandle *ship;


	if(m_player->GetCurrentShip().IsNull())
	{
	    ship = NULL;

	    ResourceMaster::GetInstance()->Log(ERROR, "<Player has no ship>");
	    // TODO: Here they would get a new ship. For now, they just lose I guess
	    return;
	}

	Integer shipi(ShipHandle::FieldName(ShipHandle::NKEY),IntToString(m_player->GetCurrentShip()));
	PrimaryKey shipkey(&shipi);

	ship = new ShipHandle(m_dbconn, shipkey,false);

	return;
	
    }				 
    else
    {
	// TODO: Player menu for future expansion of possabilities for multiple players
    }

    PQclear(dbresult);
    return;

    
}




void VoidServerThread::Send(const std::string &str)
{
    m_socket->SendData(str.c_str(),str.size());
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
    Send(Color()->get(RED) + "[" + Color()->get(WHITE) + sector +  Color()->get(RED) + "] " + Color()->get(CYAN) + "Command:" + Color()->get(GRAY) + ' ');

    return ReceiveLine();
}

 
void        VoidServerThread::ServiceClient(TCPSocket *pcClientSocket)
{


    m_socket = pcClientSocket;

    m_pColor = new ANSIColor;

    ResourceMaster::GetInstance()->Log(DEBUG,"<Connection from:" + std::string(m_socket->RemoteIPAddress().GetAddressString()) +  ">");

    signal(SIGPIPE, sigpipeHandler);

    std::string outstring;
    std::string instring;
    std::string input_buffer;
    std::string line;
    std::ostringstream os;
    int tries =0;



    SendClearScreen();
  

    os << Color()->get(LIGHTBLUE);
    os << endr << "Welcome to Void Revolution" <<  endr;
    os << Color()->get(WHITE);
    os << "Version " << VOID_VER_MAJOR << '.' << VOID_VER_MINOR << '.' << VOID_VER_INCR << '.' << VOID_VER_INCRMINOR << endr;

    os << DisplayNews();
    os << Color()->get(GRAY) << "Press Enter." << endr;

    try{

    Send( os.str());

    std::string enter = ReceiveLine();

    if(enter == "VOIDGUI")
    {
	delete m_pColor;
	m_pColor = new GUIColor;
    }



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
    catch(const DBException &e)
    {
	std::string err = "<DB Error during login: " + PrepareForSQL(e.GetMessage()) + ">";
	ResourceMaster::GetInstance()->Log(EMERGENCY, err);
	return;
    }
    catch(SocketException &e)
    {
	ResourceMaster::GetInstance()->Log(ERROR, std::string((const char *)e));
	return;
    }


    ResourceMaster::GetInstance()->SetThreadForPlayer(this,(std::string) GetPlayer()->GetName());
      


    bool done = false;

    while(!done && bServerActive)
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
		catch(SocketException e)
		{
		    if (e == SocketException::errNotConnected)
		    {
			ResourceMaster::GetInstance()->Log(ERROR,"<Connection timeout>");
		    return ;
		    }
		    if(e == SocketException::errWouldBlock)
		    {
			std::cerr << "WOULD BLOCK" << std::endl;
		    }
		    
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
		
            if (excep != SocketException::errNotConnected)
            {
                //syslog(LOG_ERR, "Socket Exception : %s", (const char *) excep);
		ResourceMaster::GetInstance()->Log(EMERGENCY,"<Socket Exception:" + PrepareForSQL(std::string((const char*)excep)) + ">");
		std::cerr << "Socket Exception: " << (const char*)excep << std::endl;
                throw errFatalError;
            }
            return;
        }
	catch(DBException e)
	{
	    ResourceMaster::GetInstance()->Log(EMERGENCY,"<DB Exception:" + PrepareForSQL(e.GetMessage()) + ">");
	    std::cerr << "DB Exception: " << e.GetMessage() << std::endl;
	    return;
	}
	catch(exception ex)
	{
	}
    }


    if(m_login)
	ResourceMaster::GetInstance()->Log(DEBUG,"<LOGOUT: " + PrepareForSQL(m_login->GetLogin().GetAsString()) + ">");

    ResourceMaster::GetInstance()->Log(DEBUG,"<Done servicing:" + PrepareForSQL(std::string(m_socket->RemoteIPAddress().GetAddressString())) +  ">");

    ResourceMaster::GetInstance()->RemoveThreadForPlayer((std::string)m_player->GetName());

    delete m_pColor;
    delete m_login;
    delete m_player;

}

VoidServerThread::VoidServerThread() : TCPServerThread()
{
    m_player = NULL;
    m_login = NULL;
    add_command(new VoidCommandDisplay(this));
    add_command(new VoidCommandMove(this));
    add_command(new VoidCommandPort(this));
    add_command(new VoidCommandStatus(this));
    add_command(new VoidCommandWho(this));
    add_command(new VoidCommandHail(this));
    add_command(new VoidCommandTransmit(this));
    add_command(new VoidCommandDock(this));
    add_command(new VoidCommandBeam(this));
    add_command(new VoidCommandTow(this));



}

VoidServerThread::~VoidServerThread()
{
    delete m_player;
    delete m_login;
    delete m_pColor;

    for(std::list<VoidCommand*>::iterator iter = m_commandlist.begin();
	iter != m_commandlist.end();
	iter++)
    {
	delete *iter;
    }
   
}
