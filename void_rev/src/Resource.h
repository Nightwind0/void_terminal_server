
#ifndef VOID_RESOURCE_H
#define VOID_RESOURCE_H


#include <string>
#include "PrimaryKey.h"
#include "libpq-fe.h"
#include "Mutex.h"


enum class ResourceType : unsigned char {
  LOGIN=0,
    PLAYER,
    SHIP,
    SHIPTYPE,
    PLANET,
    OUTPOST,
    SECTOR,
    TERRITORY,
    SERIALOBJECT
};

class Resource
{
private:
    NormalMutex m_mutex;
    int m_refcount;
    ResourceType m_type;
    PrimaryKey m_key;

    
public:


    Resource();
    Resource(ResourceType type, const PrimaryKey &key);
    void Lock(){ m_refcount++; m_mutex.Lock();}
    void Unlock(){m_mutex.Unlock(); m_refcount--; }
    bool NoCount()const{ return (m_refcount == 0);}
    std::string GenerateID() const;

    bool operator==(const Resource &other) const;
    bool operator<(const Resource &other) const;

};


std::string TypeToString(ResourceType type);


#endif
