#ifndef VOID_LOGIN_HANDLE_H
#define VOID_LOGIN_HANDLE_H

#include "PrimaryKey.h"
#include <memory>
#include "Resource.h"
#include "SerialObject.h"

class LoginHandle : public SerialObject
{
 private:
  static const std::string FIELD_NAMES[];


 public:
 LoginHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew = false):SerialObject(dbconn,key,isnew){}
    ~LoginHandle(){CloseDownObject();}


    ResourceType GetType()const{return ResourceType::LOGIN;};
    enum FIELDS{LOGIN,PASSWORD,FIRSTLOGIN,LASTLOGIN,LASTIP,EMAIL,DISABLED,LOGINS};
    virtual std::string GetFieldName(int fieldnum)const;
    static std::string FieldName(int fieldnum){ return FIELD_NAMES[fieldnum]; }
    virtual void LoadFromDB();
    Text GetLogin()const;
    Text GetPassword()const;
    Timestamp GetFirstLogin()const;
    Timestamp GetLastLogin()const;
    Text GetLastIP()const;
    Text GetEmail()const;
    Boolean GetDisabled()const;
    Integer GetLogins()const;
/*
    void SetLogin(const Text &name);
    void SetPassword(const Text &password);
    void SetFirstLogin(const Timestamp &date);
    void SetLastLogin(const Timestamp &date);
    void SetLastIP(const Text &ip);
    void SetDisabled(const Boolean &b);
    void SetLogins(const Integer &logins);
*/


    void SetLogin(const std::string &);
    void SetPassword(const std::string &);
    void SetFirstLogin(const std::string &);
    void SetLastLogin(const std::string &);
    void SetLastIP(const std::string &);
    void SetDisabled(const bool &);
    void SetLogins(const int &);
    void SetEmail(const std::string &);
    
//    virtual void ForceDBUpdate()=0;
  
    virtual std::string DBTable()const { return "Login";};
};

using LoginHandlePtr = std::shared_ptr<LoginHandle>;

#endif
