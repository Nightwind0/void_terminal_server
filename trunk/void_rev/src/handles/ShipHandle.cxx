#include "ShipHandle.h"
#include "ShipTypeHandle.h"
#include <string>
#include "SerialObject.h"
#include "Resource.h"
#include "void_util.h"


#define PGV(d,x,y) PQgetvalue(d,x,y),PQgetisnull(d,x,y)?true:false 


const char * ShipHandle::FIELD_NAMES[] = {"nkey","sname","ktype", "ballianceowned","kowner","kalliance","bpersonal","nsentinels", "nmissiles","nmines","ntrackers","nshields","nplasma","nmetals","ncarbon","npeople","ndebris","nexplosives","nprobes","nholds","bcloaked","ksector", "ktowship"};

//    enum FIELDS{NKEY,NAME,TYPE,ISALLIANCEOWNED,OWNER,ALLIANCE,ISPERSONAL,SENTINELS,MISSILES,MINES,TRACKERS,SHIELDS,PLASMA,METAL,CARBON,PEOPLE,DEBRIS,EXPLOSIVES,PROBES,HOLDS, ISCLOAKED,SECTOR};

std::string ShipHandle::GetFieldName(int fieldnum)const { return FIELD_NAMES[fieldnum];}

void ShipHandle::LoadFromDB()
{
    // screw this method man... I'm not gonna write it. blah
}

ShipTypeHandle  ShipHandle::GetShipTypeHandle()const
{
    Integer shiptypenum(ShipTypeHandle::FieldName(ShipTypeHandle::NKEY), GetTypeKey().GetAsString());
    PrimaryKey key(&shiptypenum);

    ShipTypeHandle handle(m_dbconn, key);

    return handle;
}

ShipHandle ShipHandle::HandleFromNkey(PGconn *dbconn, const int ship)
{
    Integer shipi(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(ship));

    PrimaryKey key(&shipi);

   return ShipHandle(dbconn, key);
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
    SetField(NKEY,new Integer(GetFieldName(NKEY),IntToString(nkey)));
}
void ShipHandle::SetName(const std::string &name)
{
    SetField(NAME, new Text(GetFieldName(NAME),name));
}
void ShipHandle::SetTypeKey(const int &type)
{
    SetField(TYPE, new Integer(GetFieldName(TYPE),IntToString(type)));
}
void ShipHandle::SetIsAllianceOwned(const bool &b)
{
    SetField(ISALLIANCEOWNED, new Boolean(GetFieldName(ISALLIANCEOWNED),BooleanToString(b)));
}
void ShipHandle::SetOwner(const std::string &name)
{
    SetField(OWNER, new Text(GetFieldName(OWNER),name));
}
void ShipHandle::SetAlliance(const std::string &alliance)
{
    SetField(ALLIANCE, new Text(GetFieldName(ALLIANCE), alliance));
}

void ShipHandle::SetIsPersonal(const bool &b)
{
    SetField(ISPERSONAL, new Boolean(GetFieldName(ISPERSONAL),BooleanToString(b)));
}
void ShipHandle::SetSentinels(const int &sentinels)
{
    SetField(SENTINELS, new Integer(GetFieldName(SENTINELS), IntToString(sentinels)));
}
void ShipHandle::SetMissiles(const int &missiles)
{
    SetField(MISSILES, new Integer(GetFieldName(MISSILES), IntToString(missiles)));
}
void ShipHandle::SetMines(const int &mines)
{
    SetField(MINES, new Integer(GetFieldName(MINES), IntToString(mines)));
}
void ShipHandle::SetTrackers(const int &trackers)
{
    SetField(TRACKERS, new Integer(GetFieldName(TRACKERS), IntToString(trackers)));
}
void ShipHandle::SetTow(const int &trackers)
{
    SetField(TOWSHIP, new Integer(GetFieldName(TOWSHIP), IntToString(trackers)));
}
void ShipHandle::BreakTow()
{
//    SetField(TOWSHIP, new Integer(GetFieldName(TRACKERS), "",true));
    SetFieldNull(TOWSHIP);
    
}

void ShipHandle::SetShields(const int &shields)
{
    SetField(SHIELDS, new Integer(GetFieldName(SHIELDS), IntToString(shields)));
}
void ShipHandle::SetPlasma(const int &plasma)
{
    SetField(PLASMA, new Integer(GetFieldName(PLASMA), IntToString(plasma)));
}
void ShipHandle::SetMetals(const int &metal)
{
    SetField(METALS, new Integer(GetFieldName(METALS), IntToString(metal)));
}
void ShipHandle::SetCarbon(const int &carbon)
{
    SetField(CARBON, new Integer(GetFieldName(CARBON), IntToString(carbon)));
}
void ShipHandle::SetPeople(const int &people)
{
    SetField(PEOPLE, new Integer(GetFieldName(PEOPLE), IntToString(people)));
}
void ShipHandle::SetDebris(const int &debris)
{
    SetField(DEBRIS, new Integer(GetFieldName(DEBRIS), IntToString(debris)));
}
void ShipHandle::SetExplosives(const int &explosives)
{
    SetField(EXPLOSIVES, new Integer(GetFieldName(EXPLOSIVES), IntToString(explosives)));
}
void ShipHandle::SetProbes(const int &probes)
{
    SetField(PROBES, new Integer(GetFieldName(PROBES), IntToString(probes)));
}
void ShipHandle::SetHolds(const int &holds)
{
    SetField(HOLDS, new Integer(GetFieldName(HOLDS), IntToString(holds)));
}
void ShipHandle::SetIsCloaked(const bool &b)
{
    SetField(ISCLOAKED, new Boolean(GetFieldName(ISCLOAKED), BooleanToString(b)));
}
void ShipHandle::SetSector(const int &sector)
{
    SetField(SECTOR, new Integer(GetFieldName(SECTOR), IntToString(sector)));
}
