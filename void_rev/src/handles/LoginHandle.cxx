#include "LoginHandle.h"
#include <libpq-fe.h>
#include <string>
#include <map>
#include "void_util.h"
#include "ResourceMaster.h"


const std::string LoginHandle::FIELD_NAMES[] = {"slogin","spassword","dfirstlogin","dlastlogin","slastip","semail","bdisabled","nlogins"};

void LoginHandle::LoadFromDB()
{
  std::string query = "select slogin,spassword,dfirstlogin,dlastlogin,slastip,semail,bdisabled,nlogins from login where " + CreateWhereClause() + ";";

  ResourceMaster::GetInstance()->Log(AUDIT, query);
  pqxx::work work{*m_dbconn};
  pqxx::result r = work.exec(query);
  
    
  if(r.size() != 1){
    throw DBException("LoginHandle::LoadFromDB: ntuples != 1");
  }


  m_fields[LOGIN] = std::make_shared<Text>(FIELD_NAMES[LOGIN],r[0][0].as<std::string>());
  m_fields[PASSWORD] = std::make_shared<Text>(FIELD_NAMES[PASSWORD],r[0][1].as<std::string>());
  m_fields[FIRSTLOGIN] = std::make_shared<Timestamp>(FIELD_NAMES[FIRSTLOGIN],r[0][2].as<std::string>(), r[0][2].is_null());
  m_fields[LASTLOGIN] = std::make_shared<Timestamp>(FIELD_NAMES[LASTLOGIN],r[0][3].as<std::string>(),r[0][3].is_null());
  m_fields[LASTIP] = std::make_shared<Text>(FIELD_NAMES[LASTIP],r[0][4].as<std::string>(""), r[0][4].is_null());
  m_fields[EMAIL] = std::make_shared<Text>(FIELD_NAMES[EMAIL],r[0][5].as<std::string>(""));
  m_fields[DISABLED] = std::make_shared<Boolean>(FIELD_NAMES[DISABLED],r[0][6].as<std::string>());
  m_fields[LOGINS] = std::make_shared<Integer>(FIELD_NAMES[LOGINS],r[0][7].as<std::string>());
  work.commit();
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
  SetField(LOGIN, std::make_shared<Text>(GetFieldName(LOGIN),login));
}


void LoginHandle::SetPassword(const std::string &password)
{
  SetField(PASSWORD, std::make_shared<Text>(GetFieldName(PASSWORD),password));
}

void LoginHandle::SetFirstLogin(const std::string &date)
{
  SetField(FIRSTLOGIN, std::make_shared<Timestamp>(GetFieldName(FIRSTLOGIN),date));
}
void LoginHandle::SetLastLogin(const std::string &date)
{
  SetField(LASTLOGIN, std::make_shared<Timestamp>(GetFieldName(LASTLOGIN),date));
}

void LoginHandle::SetEmail(const std::string &email)
{
  SetField(EMAIL, std::make_shared<Text>(GetFieldName(EMAIL),email));
}

void LoginHandle::SetLastIP(const std::string &ip)
{
  SetField(LASTIP, std::make_shared<Text>(GetFieldName(LASTIP),ip));
}

void LoginHandle::SetDisabled(const bool &b)
{
  SetField(DISABLED, std::make_shared<Boolean>(GetFieldName(DISABLED),BooleanToString(b)));
}

void LoginHandle::SetLogins(const int &logins)
{
  SetField(LOGINS, std::make_shared<Integer>(GetFieldName(LOGINS),IntToString(logins)));
}
