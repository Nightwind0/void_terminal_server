
#include "OutpostHandle.h"
#include "void_util.h"
#include "ResourceMaster.h"
#include <cmath>

#define PGV(d,x,y) PQgetvalue(d,x,y),PQgetisnull(d,x,y)?true:false 


const char * OutpostHandle::FIELD_NAMES[] = {"sname","ksector","bspecial","bbuyplasma","bbuymetals","bbuycarbon","kdiscoverer","klastvisitor","ndaystocompletion","dlastvisit", "fplasmaprice","fmetalsprice","fcarbonprice"};


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

Float OutpostHandle::GetPlasmaPrice() const
{
    return GetFloat(PLASMAPRICE);
}

Float OutpostHandle::GetMetalsPrice() const
{
    return GetFloat(METALSPRICE);
}

Float OutpostHandle::GetCarbonPrice() const
{
    return GetFloat(CARBONPRICE);
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

double OutpostHandle::GetBuyRateAfterTime(unsigned int minutes, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    int delay = atoi(RM->GetConfig("price_change_delay").c_str());

    if( minutes < delay ) return current_price;
    else minutes -= delay;

    double rate_cap = atof(RM->GetConfig("buyrate_cap").c_str());
    double gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    double stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
 
    double result =   rate_cap - ((rate_cap - current_price) * pow(gap_ratio, (double)minutes / stock_recovery_minutes)) ;

    RM->Log(AUDIT,"#BUY P=" + DoubleToString(current_price) + "  I=" + IntToString(minutes) + " P'=" + DoubleToString(result)) ;

    return result;
}


double OutpostHandle::GetSellRateAfterTime(unsigned int minutes, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    int delay = atoi(RM->GetConfig("price_change_delay").c_str());

    if( minutes < delay ) return current_price;
    else minutes -= delay;

    double gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    double stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
    double rate_floor = atof(RM->GetConfig("sellrate_floor").c_str());
    

    double result = (current_price - rate_floor) * pow(gap_ratio, (double)minutes / stock_recovery_minutes) + rate_floor;

    RM->Log(AUDIT,"#SELL P=" + DoubleToString(current_price) + "  I=" + IntToString(minutes) + " P'=" + DoubleToString(result)) ;

    return result;
}

double OutpostHandle::GetBuyRateAfterPurchase(unsigned int stock, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    double r = atof(RM->GetConfig("buy_price_delta_per_unit").c_str());
    double i = atof(RM->GetConfig("stock_unit").c_str());
    double f = atof(RM->GetConfig("sellrate_floor").c_str());
    
    double result = (current_price - f) * pow(r,(double)stock/i) + f;

    RM->Log(AUDIT,"#PCH P=" + DoubleToString(current_price) + "  S=" + IntToString(stock) + " P'=" + DoubleToString(result)) ;
    

    return result;
}

double OutpostHandle::GetSellRateAfterSale(unsigned int stock, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    double r = atof(RM->GetConfig("sell_price_delta_per_unit").c_str());
    double i = atof(RM->GetConfig("stock_unit").c_str());
    double f = atof(RM->GetConfig("sellrate_floor").c_str());
    
    double result = (current_price - f) * pow(r,(double)stock/i) + f;
    
    RM->Log(AUDIT,"#SLE P=" + DoubleToString(current_price) + "  S=" + DoubleToString(stock) + " P'=" + DoubleToString(result)) ;

    return result;
}



#if 0

void OutpostHandle::SetSellRate(double  rate)
{
    SetField(SELLRATE, new Double(GetFieldName(SELLRATE),DoubleToString(rate)));
}

void OutpostHandle::SetBuyRate(double rate)
{
    SetField(SELLRATE, new Double(GetFieldName(BUYRATE),DoubleToString(rate)));
}


double OutpostHandle::GetBuyRateAfterPurchase( unsigned int units_purchased )
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    
}

double OutpostHandle::GetSellRateAfterSale( unsigned int units_sold )
{
    
}

#endif
