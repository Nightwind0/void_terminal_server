#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "VoidCommandTransmit.h"
#include <list>


using std::string;


VoidCommandTransmit::VoidCommandTransmit(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandTransmit::~VoidCommandTransmit()
{
}

string VoidCommandTransmit::GetCommandString()const
{
    return "Transmit";
}
string VoidCommandTransmit::GetDescription()const
{
    return "Send a message to everyone in your sector.";
}
string VoidCommandTransmit::GetSyntax()const
{
    return "$/transmit <message>";
}
    
bool VoidCommandTransmit::ClaimCommand(const string &command)
{
    if(CompStrings(command,"transmit") || command == "$")
	return true;
    else return false;
}

bool VoidCommandTransmit::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    
    if(!Transmit(arguments))
    {
	get_thread()->Send(Color()->get(RED) + "Your transmission did not go through." + endr);
    }
    

    return true;
}
    
std::list<std::string> VoidCommandTransmit::get_players_in_sector(int sector)
{
    std::list<std::string> playerlist;
    std::string query = "select player.sname from player,ship where (bmob = 'F' or bmob is null) and ship.ksector = '" + IntToString(sector)
	+  "' and player.kcurrentship = ship.nkey;";

    PGresult *dbresult = get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Get players in sector error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }

    int numplayers = PQntuples(dbresult);

    for(int i=0;i<numplayers;i++)
    {
	playerlist.push_back(PQgetvalue(dbresult,i,0));
    }

    PQclear(dbresult);

    return playerlist;
}


bool VoidCommandTransmit::Transmit(std::string msg)
{
    ShipHandle * ship = create_handle_to_current_ship(get_thread()->GetPlayer());
    int sector = (int)ship->GetSector();

    delete ship;

    std::string myname = (std::string)get_thread()->GetPlayer()->GetName();
    
    MessagePtr message = std::shared_ptr<Message>();
    std::string fromname = (std::string)get_thread()->GetPlayer()->GetName();

    message->SetFrom(fromname);
    message->SetType(Message::TRANSMIT);
    message->SetString(msg + endr);

    

    std::list<std::string> playerlist = get_players_in_sector(sector);

    for(std::list<std::string>::iterator iter = playerlist.begin();
	iter != playerlist.end();
	iter++)
    {
	if(*iter != myname)
	    ResourceMaster::GetInstance()->SendMessage(get_thread()->GetLocalSocket(),*iter, message);
    }


    return true;
}

 
