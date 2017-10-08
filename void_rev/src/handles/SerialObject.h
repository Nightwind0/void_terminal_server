#ifndef VOID_SERIAL_OBJECT_H
#define VOID_SERIAL_OBJECT_H

#include "PrimaryKey.h"
#include "void_util.h"
#include "Resource.h"
#include <pqxx/pqxx>
#include "Field.h"
#include <map>
#include <memory>


class SerialObject
{
 public:
    SerialObject(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew = false);
    virtual ~SerialObject();

    virtual ResourceType GetType()const{ return ResourceType::SERIALOBJECT;};
    virtual std::string GetFieldName(int field)const =0;

    virtual void LoadFromDB()=0;
    void DeleteFromDB();
    void SetNew();
    void Lock();
    void Unlock();
    virtual void Insert(); 

    virtual std::string DBTable() const = 0;

    bool Exists()const{ return RecordExistsInDB(); }
 protected:
    void create_prepared_statements();
    
    mutable std::map<int,FieldPtr> m_fields;
    DatabaseConnPtr  m_dbconn;
    PrimaryKey m_key;
    bool m_new;
    bool m_lock;

    std::string CreateWhereClause() const;
    void UpdateFieldToDB(int field);
    void SetFieldNull(int field);

    virtual std::string GetField(int field, bool *b)const;

    virtual Timestamp GetTimestamp (int field)const;
    virtual Integer GetInteger(int field)const;
    virtual Float GetFloat(int field)const;
    virtual Text GetText(int field)const;
    virtual Boolean GetBoolean(int field)const;
    virtual void SetField(int fieldnum, FieldPtr field);

    bool RecordExistsInDB()const;
    virtual void CloseDownObject(); // don't fucking call this from the base class, it'll kill you.
};

#endif
