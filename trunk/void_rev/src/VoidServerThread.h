#ifndef VOID_SERVER_THREAD_H
#define VOID_SERVER_THREAD_H


#include "thread.h"
#include "libpq-fe.h"
#include <vector>
#include <string>
#include "LoginHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include <list>
#include "VoidCommand.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "Message.h"

class VoidServerThread: public Thread
{



 public:

    VoidServerThread(TCPSocket *socket);
    ~VoidServerThread();


    PGconn * GetDBConn() const { return m_dbconn; }

    PlayerHandle *GetPlayer()const;
    LoginHandle  *GetLogin()const;

    PGresult *DBExec(const std::string &sql);

    bool PostCommand(const std::string &command, const std::string &arguments); 

    void Send(const std::string& str);
    void SendClearScreen();
    void SendWordWrapped(const std::string &str, int screen_width);
    ColorType *Color()const{ return m_pColor; }
    std::string Receive(bool block);
    std::string ReceiveLine();

    UNIXDatagramSocket * GetLocalSocket() const { return m_unixsocket; }
//    PlayerHandle * CreatePlayerHandle(const PrimaryKey &key, bool lock)const;
//    ShipHandle * CreateShipHandle(const PrimaryKey &key, bool lock)const;

    ShipHandle * CreateNewShip(int shiptype);	




 protected:


    bool DoCommand(const std::string &command, const std::string &arguments, bool frompost);

    std::list<VoidCommand *> m_commandlist; 
    void add_command(VoidCommand *pcmd);
    
    PGconn *m_dbconn;
    TCPSocket *m_socket;
    UNIXDatagramSocket * m_unixsocket;

    LoginHandle *m_login;
    PlayerHandle *m_player;


    static const char *  blackout;

    virtual bool thread_init();
    virtual void thread_destroy();
    virtual bool run();

    void OpenDataBaseConnection();
    void CloseDataBaseConnection();
    void OpenLocalSocket();
    void CloseLocalSocket();
    void HandleSystemMessage(const Message &msg);
    void HandleLocalMessage(const Message &msg);


    ColorType * m_pColor;

    std::string CommandPrompt();

    bool Login();
    bool RegisterNewLogin();
    void ChoosePlayer();
    void StartNewPlayer();

    bool MoveToSector(int sector);

    void DisplayStatus();
    void Service();


    std::string DisplayCommands();
    std::string DisplayNews();


    static const char * endr;

};

#endif