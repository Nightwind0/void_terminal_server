#include "PlayerHandle.h"
#include "SerialObject.h"
#include "void_util.h"


#define PGV(d,x,y) PQgetvalue(d,x,y),PQgetisnull(d,x,y)?true:false 
//    enum FIELDS{NAME, LOGIN, ISMOB, MOB, CREDITS, TURNSLEFT, POINTS, ALLIANCE, CURRENTSHIP, FIRSTPLAY, LASTPLAY}

const char * PlayerHandle::FIELD_NAMES[] = {"sname","klogin","bmob","kmob","ncredits","nturnsleft","npoints","kalliance","kcurrentship", "dfirstplay", "dlastplay"};

 std::string PlayerHandle::GetFieldName(int fieldnum)const
{
    return FIELD_NAMES[fieldnum];
}

void PlayerHandle::LoadFromDB()
{

    PGresult *dbresult;
    std::string query = "select sname,klogin,bmob,kmob,ncredits,nturnsleft,npoints,kalliance,kcurrentship,dfirstplay,dlastplay from player where " + CreateWhereClause() + ";";

    dbresult  = PQexec(m_dbconn,query.c_str());
    
    if(PQntuples(dbresult) != 1)
    {
	throw DBException("PlayerHandle::LoadFromDB: ntuples != 1");
    }


    m_fields[NAME] = new Text(FIELD_NAMES[NAME],PQgetvalue(dbresult,0,0),PQgetisnull(dbresult,0,0)?true:false);
    m_fields[LOGIN] = new Text(FIELD_NAMES[LOGIN],PQgetvalue(dbresult,0,1),PQgetisnull(dbresult,0,1)?true:false);
    m_fields[ISMOB] = new Boolean(FIELD_NAMES[ISMOB],PGV(dbresult,0,2));
    m_fields[MOB] = new Text(FIELD_NAMES[MOB],PGV(dbresult,0,3));
    m_fields[CREDITS] = new Integer(FIELD_NAMES[CREDITS],PGV(dbresult,0,4));
    m_fields[TURNSLEFT] = new Integer(FIELD_NAMES[TURNSLEFT],PGV(dbresult,0,5));
    m_fields[POINTS] = new Integer(FIELD_NAMES[POINTS],PGV(dbresult,0,6));
    m_fields[ALLIANCE] = new Text(FIELD_NAMES[ALLIANCE],PGV(dbresult,0,7));
    m_fields[CURRENTSHIP] = new Integer(FIELD_NAMES[CURRENTSHIP],PGV(dbresult,0,8));
    m_fields[FIRSTPLAY] = new Timestamp(FIELD_NAMES[FIRSTPLAY],PGV(dbresult,0,9));
    m_fields[LASTPLAY] = new Timestamp(FIELD_NAMES[LASTPLAY],PGV(dbresult,0,10));

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
        SetField(NAME, new Text(GetFieldName(NAME),name));
}
void PlayerHandle::SetLogin(const std::string &login)
{
    SetField(LOGIN, new Text(GetFieldName(LOGIN),login));
}
void PlayerHandle::SetFirstPlay(const std::string &date)
{
    SetField(FIRSTPLAY, new Timestamp(GetFieldName(FIRSTPLAY),date));
}
void PlayerHandle::SetLastPlay(const std::string &date)
{
    SetField(LASTPLAY, new Timestamp(GetFieldName(LASTPLAY),date));
}

void PlayerHandle::SetMob(const std::string &mob)
{
    SetField(MOB, new Text(GetFieldName(MOB), mob));
}

void PlayerHandle::SetIsMob(const bool &ismob)
{
    SetField(ISMOB, new Boolean(GetFieldName(ISMOB),BooleanToString(ismob)));
}


void PlayerHandle::SetCredits(const int &credits)
{
    SetField(CREDITS, new Integer(GetFieldName(CREDITS), IntToString(credits)));
}

void PlayerHandle::SetPoints(const int &points)
{
    SetField(POINTS, new Integer(GetFieldName(POINTS),IntToString(points)));
}
void PlayerHandle::SetTurnsLeft(const int &turns)
{
    SetField(TURNSLEFT, new Integer(GetFieldName(TURNSLEFT),IntToString(turns)));
}

void PlayerHandle::SetAlliance(const std::string &alliance)
{
    SetField(ALLIANCE, new Text(GetFieldName(ALLIANCE),alliance));
}

void PlayerHandle::SetCurrentShip(const int &ship)
{
    SetField(CURRENTSHIP, new Integer(GetFieldName(CURRENTSHIP), IntToString(ship)));
}
