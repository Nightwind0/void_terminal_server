
#ifndef VOID_COMM_TOOLS_H
#define VOID_COMM_TOOLS_H

#include "ToolSet.h"
#include "Socket.h"
#include <list>
#include <string>

class Message;
using MessagePtr = std::shared_ptr<Message>;

class CommTools : public ToolSet
{
public:
    CommTools(PGconn *dbconn, DatagramSocketPtr pLocalSocket);
    virtual ~CommTools();
    void SendMsgToSector( const std::string &str, int sec, const std::string &exceptplayer);
    std::list<std::string> get_players_in_sector(int sector);
    void SendMessage( const std::string &to, MessagePtr pMessage );
private:
    DatagramSocketPtr m_pLocalSocket;
};

#endif
