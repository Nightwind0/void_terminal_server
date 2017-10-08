#include "CommTools.h"
#include "Message.h"
#include "ResourceMaster.h"


CommTools::CommTools(DatabaseConnPtr dbconn, DatagramSocketPtr pSocket):ToolSet(dbconn),m_pLocalSocket(pSocket)
{
}

CommTools::~CommTools()
{
}

std::list<std::string> CommTools::get_players_in_sector(int sector)
{
    std::list<std::string> playerlist;
    std::string query = "select player.sname from player,ship where ship.ksector = '" + IntToString(sector)
	+  "' and player.kcurrentship = ship.nkey;";

    pqxx::result dbresult = DBExec(query);

    for(auto row : dbresult)
    {
      playerlist.push_back(row[0].as<std::string>());
    }

    return playerlist;
}

void CommTools::SendMsgToSector(const std::string &str, int sec, const std::string &exceptplayer)
{
  MessagePtr msg = std::make_shared<Message>();
    msg->SetType(Message::BATTLE);
    msg->SetFrom("SYSTEM");


    msg->SetString(str);

    std::list<std::string> players = get_players_in_sector(sec);

    for(std::list<std::string>::iterator iter = players.begin();
	iter != players.end(); iter++)
    {
	if(*iter != exceptplayer)
	    ResourceMaster::GetInstance()->SendMessage(m_pLocalSocket, *iter, msg);
    }
    
    
}

void CommTools::SendMessage( const std::string &to, MessagePtr  pMessage )
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    RM->SendMessage( m_pLocalSocket, to, pMessage);
}
