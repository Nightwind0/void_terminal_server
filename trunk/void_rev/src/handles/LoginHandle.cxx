#include "LoginHandle.h"
#include <libpq-fe.h>
#include <string>
#include <map>
#include "void_util.h"

#define PGV(d,x,y) PQgetvalue(d,x,y),PQgetisnull(d,x,y)?true:false 



 const char * LoginHandle::FIELD_NAMES[] = {"slogin","spassword","dfirstlogin","dlastlogin","slastip","semail","bdisabled","nlogins"};

void LoginHandle::LoadFromDB()
{
    PGresult *dbresult;
    std::string query = "select slogin,spassword,dfirstlogin,dlastlogin,slastip,semail,bdisabled,nlogins from login where " + CreateWhereClause() + ";";

    dbresult  = PQexec(m_dbconn,query.c_str());
    
    if(PQntuples(dbresult) != 1)
    {
	throw DBException("LoginHandle::LoadFromDB: ntuples != 1");
    }


    m_fields[LOGIN] = new Text(FIELD_NAMES[LOGIN],PQgetvalue(dbresult,0,0),PQgetisnull(dbresult,0,0)?true:false);
    m_fields[PASSWORD] = new Text(FIELD_NAMES[PASSWORD],PQgetvalue(dbresult,0,1),PQgetisnull(dbresult,0,1)?true:false);
    m_fields[FIRSTLOGIN] = new Timestamp(FIELD_NAMES[FIRSTLOGIN],PQgetvalue(dbresult,0,2),PQgetisnull(dbresult,0,2)?true:false);
    m_fields[LASTLOGIN] = new Timestamp(FIELD_NAMES[LASTLOGIN],PQgetvalue(dbresult,0,3),PQgetisnull(dbresult,0,3)?true:false);
    m_fields[LASTIP] = new Text(FIELD_NAMES[LASTIP],PGV(dbresult,0,4));
    m_fields[EMAIL] = new Text(FIELD_NAMES[EMAIL],PGV(dbresult,0,5));
    m_fields[DISABLED] = new Boolean(FIELD_NAMES[DISABLED],PGV(dbresult,0,6));
    m_fields[LOGINS] = new Integer(FIELD_NAMES[LOGINS],PGV(dbresult,0,7));

}


std::string LoginHandle::GetFieldName(int field)const
{
    return FIELD_NAMES[field];
}

Text LoginHandle::GetLogin()const
{
    return GetText(LOGIN);

    
}
Text LoginHandle::GetPassword()const
{
    return GetText(PASSWORD);
}
Timestamp LoginHandle::GetFirstLogin()const
{
    return GetTimestamp(FIRSTLOGIN);

}
Timestamp LoginHandle::GetLastLogin()const
{
    return GetTimestamp(LASTLOGIN);
}
Text LoginHandle::GetLastIP()const
{
    return GetText(LASTIP);
}
Text LoginHandle::GetEmail()const
{
    return GetText(EMAIL);
}
Boolean LoginHandle::GetDisabled()const
{
    return GetBoolean(DISABLED);
}
Integer LoginHandle::GetLogins()const
{
    return GetInteger(LOGINS);
}


/*
void LoginHandle::SetLogin(const Text &login)
{
   
    SetField(LOGIN,new Text(login));
}
*/

void LoginHandle::SetLogin(const std::string &login)
{
    SetField(LOGIN, new Text(GetFieldName(LOGIN),login));
}


void LoginHandle::SetPassword(const std::string &password)
{
    SetField(PASSWORD, new Text(GetFieldName(PASSWORD),password));
}

void LoginHandle::SetFirstLogin(const std::string &date)
{
    SetField(FIRSTLOGIN, new Timestamp(GetFieldName(FIRSTLOGIN),date));
}
void LoginHandle::SetLastLogin(const std::string &date)
{
    SetField(LASTLOGIN, new Timestamp(GetFieldName(LASTLOGIN),date));
}

void LoginHandle::SetEmail(const std::string &email)
{
    SetField(EMAIL, new Text(GetFieldName(EMAIL),email));
}

void LoginHandle::SetLastIP(const std::string &ip)
{
    SetField(LASTIP, new Text(GetFieldName(LASTIP),ip));
}

void LoginHandle::SetDisabled(const bool &b)
{
    SetField(DISABLED, new Boolean(GetFieldName(DISABLED),BooleanToString(b)));
}

void LoginHandle::SetLogins(const int &logins)
{
    SetField(LOGINS, new Integer(GetFieldName(LOGINS),IntToString(logins)));
}
