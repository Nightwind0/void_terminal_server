
#include "OutpostHandle.h"
#include "void_util.h"
#include "ResourceMaster.h"
#include <cmath>

#define PGV(d,x,y) PQgetvalue(d,x,y),PQgetisnull(d,x,y)?true:false 


const char * OutpostHandle::FIELD_NAMES[] = {"sname","ksector","bspecial","bbuyplasma","bbuymetals","bbuycarbon","fbuyrate","fsellrate","kdiscoverer","klastvisitor","ndaystocompletion","dlastvisit"};


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
Float OutpostHandle::GetBuyRate()const
{
    return GetFloat(BUYRATE);
}
Float OutpostHandle::GetSellRate()const
{
    return GetFloat(SELLRATE);
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

void OutpostHandle::SetSellRate(float  rate)
{
    SetField(SELLRATE, new Float(GetFieldName(SELLRATE),DoubleToString(rate)));
}

void OutpostHandle::SetBuyRate(float rate)
{
    SetField(SELLRATE, new Float(GetFieldName(BUYRATE),DoubleToString(rate)));
}

float OutpostHandle::GetBuyRateAfterTime(unsigned int minutes)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    float rate_cap = atof(RM->GetConfig("buyrate_cap").c_str());
    float gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    float stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
    float fbuyrate = GetBuyRate();

    return  pow(rate_cap - ((rate_cap - fbuyrate) * gap_ratio), minutes / stock_recovery_minutes);
}

float OutpostHandle::GetSellRateAfterTime(unsigned int minutes)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    float gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    float stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
    float rate_floor = atof(RM->GetConfig("sellrate_floor").c_str());
    float fsellrate = GetSellRate();
    
    return pow( (fsellrate - rate_floor) * gap_ratio, minutes / stock_recovery_minutes) + rate_floor;
}


float OutpostHandle::GetBuyRateAfterPurchase( unsigned int units_purchased )
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    
}

float OutpostHandle::GetSellRateAfterSale( unsigned int units_sold )
{
    
}
