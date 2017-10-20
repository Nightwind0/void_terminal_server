#include "ShipHandle.h"
#include "ShipTypeHandle.h"
#include <string>
#include "SerialObject.h"
#include "Resource.h"
#include "void_util.h"
#include <cassert>


const FieldPair fields[] = {{ShipHandle::NKEY,"nkey"},
			    {ShipHandle::NAME,"sname"},
			    {ShipHandle::TYPE,"ktype"},
			    {ShipHandle::ISALLIANCEOWNED, "ballianceowned"},
			    {ShipHandle::OWNER, "kowner"},
			    {ShipHandle::ALLIANCE, "kalliance"},
			    {ShipHandle::ISPERSONAL, "bpersonal"},
			    {ShipHandle::SENTINELS,"nsentinels"},
			    {ShipHandle::MISSILES, "nmissiles"},
			    {ShipHandle::MINES,"nmines"},
			    {ShipHandle::TRACKERS,"ntrackers"},
			    {ShipHandle::SHIELDS,"nshields"},
			    {ShipHandle::PLASMA,"nplasma"},
			    {ShipHandle::METALS,"nmetals"},
			    {ShipHandle::CARBON,"ncarbon"},
			    {ShipHandle::PEOPLE,"npeople"},
			    {ShipHandle::DEBRIS,"ndebris"},
			    {ShipHandle::EXPLOSIVES,"nexplosives"},
			    {ShipHandle::PROBES,"nprobes"},
			    {ShipHandle::HOLDS,"nholds"},
			    {ShipHandle::ISCLOAKED,"bcloaked"},
			    {ShipHandle::SECTOR,"ksector"},
			    {ShipHandle::TOWSHIP,"ktowship"}};

std::string ShipHandle::FieldName(int fieldnum) {
  for(auto field : fields) {
    if(field.first == fieldnum) {
      return field.second;
    }
  }
  return "";
}

std::string ShipHandle::GetFieldName(int fieldnum) const {
  return FieldName(fieldnum);
}

ShipHandle::ShipHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew):SerialObject(dbconn,key,isnew){
  for(auto field : fields){
    add_field(field.first,field.second);
  }
}
void ShipHandle::LoadFromDB()
{
    // screw this method man... I'm not gonna write it. blah
}


unsigned int ShipHandle::GetHoldsUsed()const{
  return GetPeople() + GetPlasma() + GetMetals() + GetCarbon();
}

unsigned int ShipHandle::GetHoldsFree()const{
  return GetHolds() - GetHoldsUsed();
}

ShipTypeHandlePtr  ShipHandle::GetShipTypeHandle()const
{
  std::shared_ptr<Integer> shiptypenum = std::make_shared<Integer>(ShipTypeHandle::FieldName(ShipTypeHandle::NKEY), GetTypeKey().GetAsString());
    PrimaryKey key(shiptypenum);

    ShipTypeHandlePtr handle= std::make_shared<ShipTypeHandle>(m_dbconn, key);

    return handle;
}

ShipHandlePtr ShipHandle::HandleFromNkey(DatabaseConnPtr dbconn, const int ship)
{
  std::shared_ptr<Integer> shipi = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(ship));

    PrimaryKey key(shipi);

    return std::make_shared<ShipHandle>(dbconn, key);
}


Integer ShipHandle::GetNkey()const
{
    return GetInteger(NKEY);
}
Text ShipHandle::GetName()const
{
    return GetText(NAME);
}
Integer ShipHandle::GetTypeKey()const
{
    return GetInteger(TYPE);
}
Boolean ShipHandle::GetIsAllianceOwned()const
{
    return GetBoolean(ISALLIANCEOWNED);
}
Text ShipHandle::GetOwner()const
{
    return GetText(OWNER);
}
Text ShipHandle::GetAlliance()const
{
    return GetText(ALLIANCE);
}
Boolean ShipHandle::GetIsPersonal()const
{
    return GetBoolean(ISPERSONAL);
}
Integer ShipHandle::GetSentinels()const
{
    return GetInteger(SENTINELS);
}
Integer ShipHandle::GetMissiles()const
{
    return GetInteger(MISSILES);
}
Integer ShipHandle::GetMines()const
{
    return GetInteger(MINES);
}
Integer ShipHandle::GetTrackers()const
{
    return GetInteger(TRACKERS);
}
Integer ShipHandle::GetShields()const
{
    return GetInteger(SHIELDS);
}
Integer ShipHandle::GetPlasma()const
{
    return GetInteger(PLASMA);
}
Integer ShipHandle::GetMetals()const
{
    return GetInteger(METALS);
}
Integer ShipHandle::GetCarbon()const
{
    return GetInteger(CARBON);
}
Integer ShipHandle::GetPeople()const
{
    return GetInteger(PEOPLE);
}
Integer ShipHandle::GetDebris()const
{
    return GetInteger(DEBRIS);
}
Integer ShipHandle::GetExplosives()const
{
    return GetInteger(EXPLOSIVES);
}
Integer ShipHandle::GetProbes()const
{
    return GetInteger(PROBES);
}
Integer ShipHandle::GetHolds()const
{
    return GetInteger(HOLDS);
}
Integer ShipHandle::GetTow()const
{
    return GetInteger(TOWSHIP);
}
Boolean ShipHandle::GetIsCloaked()const
{
    return GetBoolean(ISCLOAKED);
}
Integer ShipHandle::GetSector()const
{
    return GetInteger(SECTOR);
}

void ShipHandle::SetNkey(const int &nkey)
{
  SetField(NKEY, nkey);
}
void ShipHandle::SetName(const std::string &name)
{
  SetField(NAME, name);
}
void ShipHandle::SetTypeKey(const int &type)
{
  SetField(TYPE, type);
}
void ShipHandle::SetIsAllianceOwned(const bool &b)
{
  SetField(ISALLIANCEOWNED, b);
}
void ShipHandle::SetOwner(const std::string &name)
{
  SetField(OWNER, name);
}
void ShipHandle::SetAlliance(const std::string &alliance)
{
  SetField(ALLIANCE, alliance);
}

void ShipHandle::SetIsPersonal(const bool &b)
{
  SetField(ISPERSONAL, b);
}
void ShipHandle::SetSentinels(const int &sentinels)
{
  SetField(SENTINELS, sentinels);
}
void ShipHandle::SetMissiles(const int &missiles)
{
  SetField(MISSILES, missiles);
}
void ShipHandle::SetMines(const int &mines)
{
  SetField(MINES, mines);
}
void ShipHandle::SetTrackers(const int &trackers)
{
  SetField(TRACKERS, trackers);
}
void ShipHandle::SetTow(const int &trackers)
{
  SetField(TOWSHIP, trackers);
}
void ShipHandle::BreakTow()
{
//    SetField(TOWSHIP, std::make_shared<Integer>(GetFieldName(TRACKERS), "",true));
    SetFieldNull(TOWSHIP);
    
}

void ShipHandle::SetShields(const int &shields)
{
  SetField(SHIELDS, shields);
}
void ShipHandle::SetPlasma(const int &plasma)
{
  SetField(PLASMA, plasma);
}
void ShipHandle::SetMetals(const int &metal)
{
  SetField(METALS, metal);
}
void ShipHandle::SetCarbon(const int &carbon)
{
  SetField(CARBON, carbon);
}
void ShipHandle::SetPeople(const int &people)
{
  SetField(PEOPLE, people);
}
void ShipHandle::SetDebris(const int &debris)
{
  SetField(DEBRIS, debris);
}
void ShipHandle::SetExplosives(const int &explosives)
{
  SetField(EXPLOSIVES, explosives);
}
void ShipHandle::SetProbes(const int &probes)
{
  SetField(PROBES, probes);
}
void ShipHandle::SetHolds(const int &holds)
{
  SetField(HOLDS, holds);
}
void ShipHandle::SetIsCloaked(const bool &b)
{
  SetField(ISCLOAKED, b);
}
void ShipHandle::SetSector(const int &sector)
{
  SetField(SECTOR, sector);
}
