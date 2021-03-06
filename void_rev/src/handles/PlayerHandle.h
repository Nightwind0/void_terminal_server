#ifndef VOID_PLAYER_HANDLE_H
#define VOID_PLAYER_HANDLE_H

#include "SerialObject.h"
#include <libpq-fe.h>
#include "Resource.h"
#include "PrimaryKey.h"
#include "void_util.h"
#include <string>

class PlayerHandle : public SerialObject
{
 private:
    static const char * FIELD_NAMES[];


 public:
    PlayerHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew = false);
    ~PlayerHandle(){CloseDownObject();}


    ResourceType GetType()const{return ResourceType::PLAYER;};

    enum FIELDS{NAME, LOGIN, MOB, CREDITS, TURNSLEFT, POINTS, ALLIANCE, CURRENTSHIP, FIRSTPLAY, LASTPLAY, ISDEAD};

    virtual std::string GetFieldName(int fieldnum)const;
    static std::string FieldName(int fieldnum){return FIELD_NAMES[fieldnum];}
    virtual void LoadFromDB();
    Text GetName()const;
    Text GetLogin()const;
    Timestamp GetFirstPlay()const;
    Timestamp GetLastPlay()const;
    Integer GetCredits()const;
    Integer GetTurnsLeft()const;
    Integer GetCurrentShip()const;
    Integer GetPoints()const;
    Text GetMob()const;
    Text GetAlliance()const;
    Boolean GetIsDead()const;


    void SetName(const std::string &);
    void SetLogin(const std::string &);
    void SetFirstPlay(const std::string &);
    void SetLastPlay(const std::string &);
    void SetMob(const std::string &); 
    void SetIsMob(const bool &);
    void SetCredits(const int &);
    void SetPoints(const int &);
    void SetTurnsLeft(const int &);
    void SetAlliance(const std::string &);
    void SetCurrentShip(const int &);
    void SetIsDead(const bool &);

    void AddSectorFlag(const eSectorFlags &flag, Sector sector);
    void ClearSectorFlag(const eSectorFlags &flag, Sector sector);
    unsigned int GetSectorFlags(Sector sector);
  
    virtual std::string DBTable()const { return "Player";};
 private:
    


};


using PlayerHandlePtr = std::shared_ptr<PlayerHandle>;


#endif
