
#ifndef VOID_RESOURCE_H
#define VOID_RESOURCE_H


#include <string>
#include "PrimaryKey.h"
#include "libpq-fe.h"
#include "Mutex.h"


enum RESOURCE_TYPE{LOGINTYPE=0,PLAYERTYPE,SHIPTYPE,SHIPTYPETYPE,PLANETTYPE,OUTPOSTTYPE,SECTORTYPE, TERRITORYTYPE, SERIALOBJECTTYPE};

class Resource
{
private:
    NormalMutex m_mutex;
    int m_refcount;
    RESOURCE_TYPE m_type;
    PrimaryKey m_key;



    
public:


    Resource();
    Resource(RESOURCE_TYPE type, const PrimaryKey &key);
    void Lock(){ m_refcount++; m_mutex.Lock();}
    void Unlock(){m_mutex.Unlock(); m_refcount--; }
    bool NoCount()const{ return (m_refcount == 0);}
    std::string Resource::GenerateID() const;

    bool operator==(const Resource &other) const;
    bool operator<(const Resource &other) const;

};


std::string TypeToString(RESOURCE_TYPE type);


#endif
