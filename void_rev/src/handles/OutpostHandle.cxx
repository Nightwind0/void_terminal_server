
#include "OutpostHandle.h"
#include "void_util.h"
#include "ResourceMaster.h"
#include <cmath>
#include <algorithm>


const std::string OutpostHandle::FIELD_NAMES[] = {"sname","ksector","bspecial","bbuyplasma","bbuymetals","bbuycarbon","kdiscoverer","klastvisitor","ndaystocompletion","dlastvisit", "fplasmaprice","fmetalsprice","fcarbonprice"};


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

#if 1 
void OutpostHandle::SetMetalsPriceMultiplier(double price)
{
    SetField(METALSPRICE, std::make_shared<Float>(GetFieldName(METALSPRICE),DoubleToString(price)));
}

void OutpostHandle::SetPlasmaPriceMultiplier(double price)
{
    SetField(PLASMAPRICE, std::make_shared<Float>(GetFieldName(PLASMAPRICE),DoubleToString(price)));
}

void OutpostHandle::SetCarbonPriceMultiplier(double price)
{
    SetField(CARBONPRICE, std::make_shared<Float>(GetFieldName(CARBONPRICE),DoubleToString(price)));
}
#endif

Boolean OutpostHandle::IsSpecial()const
{
    return GetBoolean(SPECIAL);
}


void OutpostHandle::SetDiscoverer(const std::string &discoverer)
{
  SetField(DISCOVERER, discoverer);
}
void OutpostHandle::SetLastVisitor(const std::string &visitor)
{
  SetField(LASTVISITOR, visitor);
}
void OutpostHandle::SetDaysToCompletion(const int days)
{
  SetField(DAYSTOCOMPLETION, days);
}
void OutpostHandle::SetLastVisit(const std::string &ts  )
{
  SetField(LASTVISIT, ts);
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
