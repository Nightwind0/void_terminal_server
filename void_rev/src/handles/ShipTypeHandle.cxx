#include "ShipTypeHandle.h"
#include "void_util.h"


//enum FIELDS{NKEY,KMANUFACTURER,SNAME,MAXMISSILES,MAXSHIELDS,MAXATTACK,MAXHOLDS,INITHOLDS,MAXSENTINELS,MAXTRACKERS,MAXMINES,MAXPEOPLE,MAXPROBES,TURNSPERSECTOR,HASWARPDRIVE, ISCLOAKABLE,HASANALYZER,SCANDISTANCE,COST,FORECOLOR,BACKCOLOR};

const char * ShipTypeHandle::FIELD_NAMES[] = {"nkey","kmanufacturer","sname","nmaxmissiles","nmaxshields","nmaxattack","nmaxholds","ninitholds","nmaxsentinels","nmaxtrackers","nmaxmines","nmaxpeople","nmaxprobes","nturnspersector","bwarpdrive","bcloakable","banalyzer","nscandistance","ncost","nforecolor","nbackcolor", "ntransrange","bforsale"};


std::string ShipTypeHandle::GetFieldName(int fieldnum)const
{
    return FIELD_NAMES[fieldnum];
}
void ShipTypeHandle::LoadFromDB(){} // We never call this, since we don't make new ship types, we will never lock and therefore never use this


Integer ShipTypeHandle::GetNkey()const
{
    return GetInteger(NKEY);
}
Integer ShipTypeHandle::GetManufacturer()const
{
    return GetInteger(MANUFACTURER);
}
Text    ShipTypeHandle::GetName()const
{
    return GetText(NAME);
}
Integer ShipTypeHandle::GetMaxMissiles()const
{
    return GetInteger(MAXMISSILES);
}
Integer ShipTypeHandle::GetMaxShields()const
{
    return GetInteger(MAXSHIELDS);
}
Integer ShipTypeHandle::GetMaxAttack()const
{
    return GetInteger(MAXATTACK);
}
Integer ShipTypeHandle::GetMaxHolds()const
{
    return GetInteger(MAXHOLDS);
}
Integer ShipTypeHandle::GetInitHolds()const
{
    return GetInteger(INITHOLDS);
}
Integer ShipTypeHandle::GetMaxSentinels()const
{
    return GetInteger(MAXSENTINELS);
}
Integer ShipTypeHandle::GetMaxTrackers()const
{
    return GetInteger(MAXTRACKERS);
}
Integer ShipTypeHandle::GetMaxMines()const
{
    return GetInteger(MAXMINES);
}
Integer ShipTypeHandle::GetMaxPeople()const
{
    return GetInteger(MAXPEOPLE);
}
Integer ShipTypeHandle::GetMaxProbes()const
{
    return GetInteger(MAXPROBES);
}
Integer ShipTypeHandle::GetTurnsPerSector()const
{
    return GetInteger(TURNSPERSECTOR);
}
Boolean ShipTypeHandle::GetHasWarpDrive()const
{
    return GetBoolean(HASWARPDRIVE);
}
Boolean ShipTypeHandle::GetIsCloakable()const
{
    return GetBoolean(ISCLOAKABLE);
}
Boolean ShipTypeHandle::GetHasAnalyzer()const
{
    return GetBoolean(HASANALYZER);
}
Integer ShipTypeHandle::GetScanDistance()const
{
    return GetInteger(SCANDISTANCE);
}
Integer ShipTypeHandle::GetCost()const
{
    return GetInteger(COST);
}
Integer ShipTypeHandle::GetForeColor()const
{
    return GetInteger(FORECOLOR);
}
Integer ShipTypeHandle::GetBackColor()const
{
    return GetInteger(BACKCOLOR);
}

Boolean ShipTypeHandle::GetForSale()const
{
    return GetBoolean(FORSALE);
}

Integer ShipTypeHandle::GetTransRange()const
{
    return GetInteger(TRANSRANGE);
}
std::string ShipTypeHandle::GetManufacturerName()
{

    if(m_manufacturer != "") return m_manufacturer;

    std::string query = "SELECT sname FROM SHIPMANUFACTURER WHERE nkey = '" + GetManufacturer().GetAsString() + "';";

    PGresult *dbresult;

    dbresult = PQexec(m_dbconn, query.c_str());

    m_manufacturer = PQgetvalue(dbresult,0,0);

    PQclear(dbresult);

    return m_manufacturer;

    
}

std::string ShipTypeHandle::GetShipTypeName(ColorType *color)
{
    
    return color->get((FGColor)GetForeColor().GetValue(),(BGColor)GetBackColor().GetValue()) + GetManufacturerName() + " " +(std::string)GetName();
}
