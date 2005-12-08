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
    static const char * FIELD_NAMES[];


 public:
    OutpostHandle(PGconn *dbconn, const PrimaryKey &key, bool isnew = false):SerialObject(dbconn,key,isnew){}
    ~OutpostHandle(){CloseDownObject();}


    RESOURCE_TYPE GetType()const{return OUTPOSTTYPE;};

    enum FIELDS{NAME,SECTOR,SPECIAL,BUYPLASMA,BUYMETALS,BUYCARBON,BUYRATE,SELLRATE,DISCOVERER,LASTVISITOR,DAYSTOCOMPLETION, LASTVISIT};

    virtual std::string GetFieldName(int fieldnum)const;
    static std::string FieldName(int fieldnum){return FIELD_NAMES[fieldnum];}
    virtual void LoadFromDB();


    Text GetName()const;
    Integer GetSector()const;
    Boolean BuysPlasma()const;
    Boolean BuysMetals()const;
    Boolean BuysCarbon()const;
    Float GetBuyRate()const;

    float GetBuyRateAfterTime(unsigned int minutes);
    float GetSellRateAfterTime(unsigned int minutes);

    float GetBuyRateAfterPurchase( unsigned int units_purchased );
    float GetSellRateAfterSale( unsigned int units_sold );

    Float GetSellRate()const;
    Text GetDiscoverer()const;
    Text GetLastVisitor()const;
    Timestamp GetLastVisit()const;
    Integer GetDaysToCompletion()const;
    Boolean IsSpecial()const;


    void SetDiscoverer(const std::string &discoverer);
    void SetLastVisitor(const std::string &visitor);
    void SetDaysToCompletion(const int days);
    void SetLastVisit(const std::string &ts);
    void SetSellRate(float rate);
    void SetBuyRate(float rate);
  
    virtual std::string DBTable()const { return "Outpost";};



};




#endif
