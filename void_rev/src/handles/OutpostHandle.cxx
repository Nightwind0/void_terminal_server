
#include "OutpostHandle.h"
#include "void_util.h"
#include "ResourceMaster.h"
#include <cmath>

#define PGV(d,x,y) PQgetvalue(d,x,y),PQgetisnull(d,x,y)?true:false 


const char * OutpostHandle::FIELD_NAMES[] = {"sname","ksector","bspecial","bbuyplasma","bbuymetals","bbuycarbon","kdiscoverer","klastvisitor","ndaystocompletion","dlastvisit", "nplasmaprice","nmetalsprice","ncarbonprice"};


//  enum FIELDS{NAME,SECTOR,SPECIAL,BUYPLASMA,BUYMETALS,BUYCARBON,BUYRATE,SELLRATE,DISCOVERER,LASTVISITOR,DAYSTOCOMPLETION, LASTVISIT

std::string OutpostHandle::GetFieldName(int fieldnum)const { return FIELD_NAMES[fieldnum];}



void OutpostHandle::LoadFromDB()
{
}



Text OutpostHandle::GetName()const
{
    return GetText(NAME);
}

Integer OutpostHandle::GetSector()const
{
    return GetInteger(SECTOR);
}
Boolean OutpostHandle::BuysPlasma()const
{
    return GetBoolean(BUYPLASMA);
}
Boolean OutpostHandle::BuysMetals()const
{
    return GetBoolean(BUYMETALS);
}
Boolean OutpostHandle::BuysCarbon()const
{
    return GetBoolean(BUYCARBON);
}

Text OutpostHandle::GetDiscoverer()const
{
    return GetText(DISCOVERER);
}
Text OutpostHandle::GetLastVisitor()const
{
    return GetText(LASTVISITOR);
}

Timestamp OutpostHandle::GetLastVisit()const
{
    return GetTimestamp(LASTVISIT);
}

Integer OutpostHandle::GetDaysToCompletion()const
{
    return GetInteger(DAYSTOCOMPLETION);
}

Integer OutpostHandle::GetPlasmaPrice() const
{
    return GetInteger(PLASMAPRICE);
}

Integer OutpostHandle::GetMetalsPrice() const
{
    return GetInteger(METALSPRICE);
}

Integer OutpostHandle::GetCarbonPrice() const
{
    return GetInteger(CARBONPRICE);
}

void OutpostHandle::SetMetalsPrice(double price)
{
    SetField(METALSPRICE, new Float(GetFieldName(METALSPRICE),DoubleToString(price)));
}

void OutpostHandle::SetPlasmaPrice(double price)
{
    SetField(PLASMAPRICE, new Float(GetFieldName(PLASMAPRICE),DoubleToString(price)));
}

void OutpostHandle::SetCarbonPrice(double price)
{
    SetField(CARBONPRICE, new Float(GetFieldName(CARBONPRICE),DoubleToString(price)));
}


Boolean OutpostHandle::IsSpecial()const
{
    return GetBoolean(SPECIAL);
}


void OutpostHandle::SetDiscoverer(const std::string &discoverer)
{
        SetField(DISCOVERER, new Text(GetFieldName(DISCOVERER),discoverer));
}
void OutpostHandle::SetLastVisitor(const std::string &visitor)
{
    SetField(LASTVISITOR, new Text(GetFieldName(LASTVISITOR),visitor));
}
void OutpostHandle::SetDaysToCompletion(const int days)
{
    SetField(DAYSTOCOMPLETION, new Integer(GetFieldName(DAYSTOCOMPLETION),IntToString(days)));
}
void OutpostHandle::SetLastVisit(const std::string &ts  )
{
    SetField(LASTVISIT, new Timestamp(GetFieldName(LASTVISIT),ts));
}

int OutpostHandle::GetBuyRateAfterTime(unsigned int minutes, int current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    float rate_cap = atof(RM->GetConfig("buyrate_cap").c_str());
    float gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    float stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
 
    RM->Log(DEBUG,"Price: " + IntToString(current_price) + " Rate cap: " + DoubleToString(rate_cap) + " gap_ratio: " + DoubleToString(gap_ratio) + " Minutes: " + IntToString(minutes) );

    int result =   (int)floor( rate_cap - ((rate_cap - current_price) * pow(gap_ratio, minutes / stock_recovery_minutes)) );

    RM->Log(DEBUG,"Result equals: " + IntToString(result));

    return result;
}


int OutpostHandle::GetSellRateAfterTime(unsigned int minutes, int current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    float gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    float stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
    float rate_floor = atof(RM->GetConfig("sellrate_floor").c_str());
    
    return (int)ceil((current_price - rate_floor) * pow(gap_ratio, minutes / stock_recovery_minutes) + rate_floor);
}

int OutpostHandle::GetBuyRateAfterPurchase(unsigned int stock, int current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    float r = atof(RM->GetConfig("buy_price_delta_per_unit").c_str());
    float i = atof(RM->GetConfig("stock_unit").c_str());
    float f = atof(RM->GetConfig("sellrate_floor").c_str());
    
    int result = (int)floor((current_price - f) * pow(r,stock/i) + f);
    

    return result;
}

int OutpostHandle::GetSellRateAfterSale(unsigned int stock, int current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    float r = atof(RM->GetConfig("sell_price_delta_per_unit").c_str());
    float i = atof(RM->GetConfig("stock_unit").c_str());
    float f = atof(RM->GetConfig("sellrate_floor").c_str());
    
    int result = (int)ceil((current_price - f) * pow(r,stock/i) + f);
    

    return result;
}



#if 0

void OutpostHandle::SetSellRate(float  rate)
{
    SetField(SELLRATE, new Float(GetFieldName(SELLRATE),DoubleToString(rate)));
}

void OutpostHandle::SetBuyRate(float rate)
{
    SetField(SELLRATE, new Float(GetFieldName(BUYRATE),DoubleToString(rate)));
}


float OutpostHandle::GetBuyRateAfterPurchase( unsigned int units_purchased )
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    
}

float OutpostHandle::GetSellRateAfterSale( unsigned int units_sold )
{
    
}

#endif
