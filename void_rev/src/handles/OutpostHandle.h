#ifndef VOID_OUTPOST_HANDLE_H
#define VOID_OUTPOST_HANDLE_H

#include "SerialObject.h"
#include <libpq-fe.h>
#include "Resource.h"
#include "PrimaryKey.h"
#include <string>
#include "ShipTypeHandle.h"

class OutpostHandle : public SerialObject
{
 private:
  static const std::string FIELD_NAMES[];
  
 public:
 OutpostHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew = false):SerialObject(dbconn,key,isnew){}
    ~OutpostHandle(){CloseDownObject();}


    ResourceType GetType()const{return ResourceType::OUTPOST;};

    enum FIELDS{NAME,SECTOR,SPECIAL,BUYPLASMA,BUYMETALS,BUYCARBON,DISCOVERER,LASTVISITOR,DAYSTOCOMPLETION, LASTVISIT, PLASMAPRICE, METALSPRICE,CARBONPRICE};

    virtual std::string GetFieldName(int fieldnum)const;
    static std::string FieldName(int fieldnum){return FIELD_NAMES[fieldnum];}
    virtual void LoadFromDB();


    Text GetName()const;
    Integer GetSector()const;
    Boolean BuysPlasma()const;
    Boolean BuysMetals()const;
    Boolean BuysCarbon()const;
    Float GetMetalsPrice() const;
    Float GetCarbonPrice() const;
    Float GetPlasmaPrice() const;
    Text GetDiscoverer()const;
    Text GetLastVisitor()const;
    Timestamp GetLastVisit()const;
    Integer GetDaysToCompletion()const;
    Boolean IsSpecial()const;


    void SetDiscoverer(const std::string &discoverer);
    void SetLastVisitor(const std::string &visitor);
    void SetDaysToCompletion(const int days);
    void SetLastVisit(const std::string &ts);
    void SetMetalsPriceMultiplier(double price);
    void SetPlasmaPriceMultiplier(double price);
    void SetCarbonPriceMultiplier(double price);

  
    virtual std::string DBTable()const { return "Outpost";};



};

using OutpostHandlePtr = std::shared_ptr<OutpostHandle>;


#endif
