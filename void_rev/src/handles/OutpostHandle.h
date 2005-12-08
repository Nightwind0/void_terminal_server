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

    enum FIELDS{NAME,SECTOR,SPECIAL,BUYPLASMA,BUYMETALS,BUYCARBON,DISCOVERER,LASTVISITOR,DAYSTOCOMPLETION, LASTVISIT, PLASMAPRICE, METALSPRICE,CARBONPRICE};

    virtual std::string GetFieldName(int fieldnum)const;
    static std::string FieldName(int fieldnum){return FIELD_NAMES[fieldnum];}
    virtual void LoadFromDB();


    Text GetName()const;
    Integer GetSector()const;
    Boolean BuysPlasma()const;
    Boolean BuysMetals()const;
    Boolean BuysCarbon()const;
    Integer GetMetalsPrice() const;
    Integer GetCarbonPrice() const;
    Integer GetPlasmaPrice() const;
    Text GetDiscoverer()const;
    Text GetLastVisitor()const;
    Timestamp GetLastVisit()const;
    Integer GetDaysToCompletion()const;
    Boolean IsSpecial()const;


    void SetDiscoverer(const std::string &discoverer);
    void SetLastVisitor(const std::string &visitor);
    void SetDaysToCompletion(const int days);
    void SetLastVisit(const std::string &ts);
    void SetMetalsPrice(double price);
    void SetPlasmaPrice(double price);
    void SetCarbonPrice(double price);

    static int GetBuyRateAfterTime(unsigned int minutes, int current_price);
    static int GetSellRateAfterTime(unsigned int minutes, int current_price);
    static int GetBuyRateAfterPurchase(unsigned int stock, int current_price);
    static int GetSellRateAfterSale(unsigned int stock, int current_price);
    virtual std::string DBTable()const { return "Outpost";};



};




#endif
