
#include "OutpostHandle.h"
#include "void_util.h"


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
