#ifndef VOID_COMMAND
#define VOID_COMMAND


#include <string>
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "void_util.h"


class VoidServerThread;

using std::string;

class VoidCommand
{
 public:
    VoidCommand(VoidServerThread *thread);
    virtual ~VoidCommand();

    virtual string GetCommandString()const=0;
    virtual string GetDescription()const=0;
    virtual string GetSyntax()const=0;
    
    virtual bool ClaimCommand(const string &command)=0;
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost)=0;
    
 private:
    VoidCommand(){}
    VoidServerThread *m_pthread;

 protected:

    std::shared_ptr<ColorType>  Color()const;
    void Send(const std::string &str)const;
    std::string ReceiveLine()const;

    PlayerHandlePtr get_player() const;

    bool isValidPlayer(const std::string& playername)const;
    void log(LOG_SEVERITY severity, const std::string& message)const;

    ShipHandlePtr create_handle_to_current_ship(PlayerHandlePtr player) const;
    PlayerHandlePtr create_handle_to_player_in_ship(int ship)const;

    virtual bool move_player_to_sector(Sector sector);
    static const char * endr;
    // static const char * blackout;
    VoidServerThread *get_thread() const;


    std::string get_config(const std::string &key) const;

    std::string get_today() const;

};
    
#endif
