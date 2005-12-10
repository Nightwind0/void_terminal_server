
#ifndef VOID_COMM_TOOLS_H
#define VOID_COMM_TOOLS_H

#include "ToolSet.h"
#include "Socket.h"
#include <list>
#include <string>

class Message;

class CommTools : public ToolSet
{
public:
    CommTools(PGconn *dbconn, DatagramSocket * pLocalSocket);
    virtual ~CommTools();
    void SendMsgToSector( const std::string &str, int sec, const std::string &exceptplayer);
    std::list<std::string> get_players_in_sector(int sector);
    void SendMessage( const std::string &to, Message * pMessage );
private:
    DatagramSocket * m_pLocalSocket;
};

#endif
