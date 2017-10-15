#include "ShipTypeHandle.h"
#include "void_util.h"
#include <cassert>


const FieldPair fields[] = {{ShipTypeHandle::NKEY, "nkey"},
			    {ShipTypeHandle::MANUFACTURER, "kmanufacturer"},
			    {ShipTypeHandle::NAME, "sname"},
			    {ShipTypeHandle::MAXMISSILES, "nmaxmissiles"},
			    {ShipTypeHandle::MAXSHIELDS, "nmaxshields"},
			    {ShipTypeHandle::MAXATTACK, "nmaxattack"},
			    {ShipTypeHandle::INITHOLDS, "nmaxholds"},
			    {ShipTypeHandle::MAXSENTINELS, "nmaxsentinels"},
			    {ShipTypeHandle::MAXTRACKERS, "nmaxtrackers"},
			    {ShipTypeHandle::MAXMINES, "nmaxmines"},
			    {ShipTypeHandle::MAXPEOPLE, "nmaxpeople"},
			    {ShipTypeHandle::MAXPROBES, "nmaxprobes"},
			    {ShipTypeHandle::TURNSPERSECTOR, "nturnspersector"},
			    {ShipTypeHandle::HASWARPDRIVE, "bwarpdrive"},
			    {ShipTypeHandle::ISCLOAKABLE, "bcloakable"},
			    {ShipTypeHandle::HASANALYZER, "banalyzer"},
			    {ShipTypeHandle::SCANDISTANCE, "nscandistance"},
			    {ShipTypeHandle::COST, "ncost"},
			    {ShipTypeHandle::FORECOLOR, "nforecolor"},
			    {ShipTypeHandle::BACKCOLOR, "nbackcolor"},
			    {ShipTypeHandle::TRANSRANGE, "ntransrange"},
			    {ShipTypeHandle::FORSALE, "bforsale"}};


std::string ShipTypeHandle::FieldName(int fieldnum) {
  for ( auto field : fields ) {
    if (field.first == fieldnum) {
      return field.second;
    }
  }
  assert(0);
  return "";
}

std::string ShipTypeHandle::GetFieldName(int fieldnum) const {
  return FieldName(fieldnum);
}

ShipTypeHandle::ShipTypeHandle(DatabaseConnPtr dbconn, const PrimaryKey &key):SerialObject(dbconn,key,false){
  for( auto field : fields ) {
    add_field(field.first, field.second);
  }
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

    pqxx::work work{*m_dbconn};
    pqxx::result dbresult = work.exec(query);

    m_manufacturer = dbresult[0][0].as<std::string>();

    work.commit();

    return m_manufacturer;
}

std::string ShipTypeHandle::GetShipTypeName(std::shared_ptr<ColorType> color)
{
    
    return color->get((FGColor)GetForeColor().GetValue(),(BGColor)GetBackColor().GetValue()) + GetManufacturerName() + " " +(std::string)GetName();
}
