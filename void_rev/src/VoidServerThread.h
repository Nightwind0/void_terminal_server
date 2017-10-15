#ifndef VOID_SERVER_THREAD_H
#define VOID_SERVER_THREAD_H


#include "thread.h"
#include <pqxx/pqxx>
#include <vector>
#include <string>
#include "LoginHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include <list>
#include <set>
#include "VoidCommand.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "Message.h"
#include "Event.h"

class VoidServerThread: public Thread
{
	

public:

    VoidServerThread(TCPSocketPtr socket);
    ~VoidServerThread();

	
    PlayerHandlePtr GetPlayer()const;
    LoginHandlePtr  GetLogin()const;
    DatabaseConnPtr GetDatabaseConn() const { return m_dbconn; }
    /* This creates the prepared statement if it doesn't exist, otherwise its a no-op
     */
    void EnsurePreparedStatement(const std::string& unique_id, const std::string& statement);
    pqxx::result DBExec(const std::string &sql);
	
    bool PostCommand(const std::string &command, const std::string &arguments); 
	
    void Send(const std::string& str);
    void SendClearScreen();
    void SendWordWrapped(const std::string &str, size_t screen_width);
    std::shared_ptr<ColorType> Color()const{ return m_pColor; }
    std::string ReceiveLine();
	
    UNIXDatagramSocketPtr GetLocalSocket() const { return m_unixsocket; }
    ShipHandlePtr CreateNewShip(int shiptype);	

protected:
    bool DoCommand(const std::string &command, const std::string &arguments, bool frompost);
    std::set<std::string> m_prepared_statements;
    std::vector<std::shared_ptr<VoidCommand>> m_commandlist; 
    void add_command(std::shared_ptr<VoidCommand> pcmd);
    
    std::shared_ptr<pqxx::connection_base> m_dbconn;
    TCPSocketPtr m_socket;
    UNIXDatagramSocketPtr  m_unixsocket;

    LoginHandlePtr m_login;
    PlayerHandlePtr m_player;
    std::string m_inputbuffer;

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


    void SetTurnsLeft();
    bool ReadMail();

    std::shared_ptr<ColorType> m_pColor;

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
    
    void RegisterCommands();

};

#endif
