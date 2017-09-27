#ifndef VOID_SERIAL_OBJECT_H
#define VOID_SERIAL_OBJECT_H

#include "PrimaryKey.h"

#include "Resource.h"
#include <libpq-fe.h>
#include "Field.h"
#include <map>



class SerialObject
{
 protected:
    mutable std::map<int,Field*> m_fields;
    PGconn *m_dbconn;
    PrimaryKey m_key;
    bool m_new;
    bool m_lock;

    std::string CreateWhereClause() const;
    void UpdateFieldToDB(int field);
    void SetFieldNull(int field);

    virtual std::string GetField(int field, bool *b)const;
    // Timestamp GetTimeStamp(PGresult *dbresult,int row, int col);
    virtual Timestamp GetTimestamp (int field)const;
    virtual Integer GetInteger(int field)const;
    virtual  Float GetFloat(int field)const;
    virtual Text GetText(int field)const;
    virtual Boolean GetBoolean(int field)const;
    virtual void SetField(int fieldnum, Field *field);


    bool RecordExistsInDB()const;

     virtual void CloseDownObject(); // don't fucking call this from the base class, it'll kill you.


 public:
    SerialObject(PGconn *dbconn, const PrimaryKey &key, bool isnew = false);
    virtual ~SerialObject();

    void SetDBConn(PGconn *dbconn){ m_dbconn = dbconn; }
    virtual ResourceType GetType()const{ return ResourceType::SERIALOBJECT;};
    virtual std::string GetFieldName(int field)const =0;

    virtual void LoadFromDB()=0;
    void DeleteFromDB();
//    virtual void ForceDBUpdate()=0;
    //   virtual void ForceDBInsert()=0;
    void SetNew();
    void Lock();
    void Unlock();
    virtual void Insert(); 

    virtual std::string DBTable()const = 0;

    bool Exists()const{ return RecordExistsInDB(); }

};

#endif
