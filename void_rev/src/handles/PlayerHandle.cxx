#include "PlayerHandle.h"
#include "SerialObject.h"
#include "void_util.h"



const char * PlayerHandle::FIELD_NAMES[] = {"sname","klogin","bmob","kmob","ncredits","nturnsleft","npoints","kalliance","kcurrentship", "dfirstplay", "dlastplay", "bdead"};


PlayerHandle::PlayerHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew):SerialObject(dbconn,key,isnew)
{
 
  
}

std::string PlayerHandle::GetFieldName(int fieldnum)const
{
    return FIELD_NAMES[fieldnum];
}

void PlayerHandle::LoadFromDB()
{


    std::string query = "select sname,klogin,bmob,kmob,ncredits,nturnsleft,npoints,kalliance,kcurrentship,dfirstplay,dlastplay from player where " + CreateWhereClause() + ";";
    pqxx::work work{*m_dbconn};
    pqxx::result r = work.exec(query);
    
    if(r.size() != 1)
    {
	throw DBException("PlayerHandle::LoadFromDB: ntuples != 1");
    }


    m_fields[NAME] = std::make_shared<Text>(FIELD_NAMES[NAME],r[0][0].as<std::string>(), r[0][0].is_null());
    m_fields[LOGIN] = std::make_shared<Text>(FIELD_NAMES[LOGIN],r[0][1].as<std::string>(), r[0][1].is_null());
    m_fields[ISMOB] = std::make_shared<Boolean>(FIELD_NAMES[ISMOB],r[0][2].as<std::string>(), r[0][2].is_null());
    m_fields[MOB] = std::make_shared<Text>(FIELD_NAMES[MOB],r[0][3].as<std::string>(), r[0][3].is_null());
    m_fields[CREDITS] = std::make_shared<Integer>(FIELD_NAMES[CREDITS],r[0][4].as<std::string>(), r[0][4].is_null());
    m_fields[TURNSLEFT] = std::make_shared<Integer>(FIELD_NAMES[TURNSLEFT],r[0][5].as<std::string>(), r[0][5].is_null());
    m_fields[POINTS] = std::make_shared<Integer>(FIELD_NAMES[POINTS],r[0][6].as<std::string>(), r[0][6].is_null());
    m_fields[ALLIANCE] = std::make_shared<Text>(FIELD_NAMES[ALLIANCE],r[0][7].as<std::string>(), r[0][7].is_null());
    m_fields[CURRENTSHIP] = std::make_shared<Integer>(FIELD_NAMES[CURRENTSHIP],r[0][8].as<std::string>(), r[0][8].is_null());
    m_fields[FIRSTPLAY] = std::make_shared<Timestamp>(FIELD_NAMES[FIRSTPLAY],r[0][9].as<std::string>(), r[0][9].is_null());
    m_fields[LASTPLAY] = std::make_shared<Timestamp>(FIELD_NAMES[LASTPLAY],r[0][10].as<std::string>(), r[0][10].is_null());
    work.commit();
}

Text PlayerHandle::GetName()const
{
    return GetText(NAME);
}

Text PlayerHandle::GetLogin()const
{
    return GetText(LOGIN);
}
Timestamp PlayerHandle::GetFirstPlay()const
{
    return GetTimestamp(FIRSTPLAY);
}

Timestamp PlayerHandle::GetLastPlay()const
{
    return GetTimestamp(LASTPLAY);
}
Boolean PlayerHandle::GetIsMob()const
{
    return GetBoolean(ISMOB);
}
Boolean PlayerHandle::GetIsDead()const
{
    return GetBoolean(ISDEAD);
}

Integer PlayerHandle::GetCredits()const
{
    return GetInteger(CREDITS);
}
Integer PlayerHandle::GetTurnsLeft()const
{
    return GetInteger(TURNSLEFT);
}
Integer PlayerHandle::GetCurrentShip()const
{
    return GetInteger(CURRENTSHIP);
}
Integer PlayerHandle::GetPoints()const
{
    return GetInteger(POINTS);
}

Text PlayerHandle::GetMob()const
{
    return GetText(MOB);
}

Text PlayerHandle::GetAlliance()const
{
    return GetText(ALLIANCE);
}


void PlayerHandle::SetName(const std::string &name)
{
        SetField(NAME, std::make_shared<Text>(GetFieldName(NAME),name));
}
void PlayerHandle::SetLogin(const std::string &login)
{
    SetField(LOGIN, std::make_shared<Text>(GetFieldName(LOGIN),login));
}
void PlayerHandle::SetFirstPlay(const std::string &date)
{
  SetField(FIRSTPLAY, std::make_shared<Timestamp>(GetFieldName(FIRSTPLAY),date));
}
void PlayerHandle::SetLastPlay(const std::string &date)
{
  SetField(LASTPLAY, std::make_shared<Timestamp>(GetFieldName(LASTPLAY),date));
}

void PlayerHandle::SetMob(const std::string &mob)
{
    SetField(MOB, std::make_shared<Text>(GetFieldName(MOB), mob));
}

void PlayerHandle::SetIsMob(const bool &ismob)
{
  SetField(ISMOB, std::make_shared<Boolean>(GetFieldName(ISMOB),BooleanToString(ismob)));
}

void PlayerHandle::SetIsDead(const bool &isdead)
{
  SetField(ISDEAD, std::make_shared<Boolean>(GetFieldName(ISDEAD),BooleanToString(isdead)));
}


void PlayerHandle::SetCredits(const int &credits)
{
    SetField(CREDITS, std::make_shared<Integer>(GetFieldName(CREDITS), IntToString(credits)));
}

void PlayerHandle::SetPoints(const int &points)
{
    SetField(POINTS, std::make_shared<Integer>(GetFieldName(POINTS),IntToString(points)));
}
void PlayerHandle::SetTurnsLeft(const int &turns)
{
    SetField(TURNSLEFT, std::make_shared<Integer>(GetFieldName(TURNSLEFT),IntToString(turns)));
}

void PlayerHandle::SetAlliance(const std::string &alliance)
{
    SetField(ALLIANCE, std::make_shared<Text>(GetFieldName(ALLIANCE),alliance));
}

void PlayerHandle::SetCurrentShip(const int &ship)
{
    SetField(CURRENTSHIP, std::make_shared<Integer>(GetFieldName(CURRENTSHIP), IntToString(ship)));
}


void PlayerHandle::AddSectorFlag(const eSectorFlags &flag, Sector sector)
{
  std::string name = GetName();
  pqxx::work work{*m_dbconn};
  work.prepared("setSectorFlag")(static_cast<int>(flag))(static_cast<int>(sector))(name).exec();
  work.commit();
}

void PlayerHandle::ClearSectorFlag(const eSectorFlags &flag, Sector sector)
{
  std::string name = GetName();
  pqxx::work work{*m_dbconn};
  work.prepared("clearSectorFlag")(static_cast<int>(flag))(static_cast<int>(sector))(name).exec();
  work.commit();
}

unsigned int PlayerHandle::GetSectorFlags(Sector sector)
{
  std::string name = GetName();
  pqxx::work work{*m_dbconn};
  pqxx::result r = work.prepared("getSectorFlags")(static_cast<int>(sector))(name).exec();
  work.commit();
  if(r.size() < 1){
    return 0;
  } else {
    return r[0][0].as<int>();
  }
}
