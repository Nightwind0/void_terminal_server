#include "PlanetHandle.h"

#include <cassert>

const FieldPair fields[] = {{PlanetHandle::NAME, "sname"},
			    {PlanetHandle::SECTOR, "ksector"},
			    {PlanetHandle::POPULATION, "fpopulation"},
			    {PlanetHandle::SIZE, "fsize"},
			    {PlanetHandle::CIVLEVEL, "ncivlevel"},
			    {PlanetHandle::CIVSTARTDATE, "dcivstart"},
			    {PlanetHandle::STARTCONSTRUCTIONDATE, "dstartconstruction"},
			    {PlanetHandle::FLAGS, "nflags"},
			    {PlanetHandle::OWNER, "kowner"},
			    {PlanetHandle::ALLIANCE, "kalliance"},
			    {PlanetHandle::LASTVISITOR, "klastvisitor"},
			    {PlanetHandle::LASTVISITDATE, "dlastvisit"},
			    {PlanetHandle::CREATIONDATE, "dcreation"},
			    {PlanetHandle::CARBON, "fcarbon"},
			    {PlanetHandle::METALS, "fmetals"},
			    {PlanetHandle::PLASMA, "fplasma"},
			    {PlanetHandle::CLASS, "kclass"}
};

			    
std::string PlanetHandle::FieldName(int fieldnum) {
for ( auto field : fields ) {
if (field.first == fieldnum) {
return field.second;
}
}
assert(0);
return "";
}
			    
std::string PlanetHandle::GetFieldName(int fieldnum) const {
return FieldName(fieldnum);
}

PlanetHandle::PlanetHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew)
    :SerialObject(dbconn,key,isnew){
for( auto field : fields) {
add_field(field.first,field.second);
}
}

void PlanetHandle::LoadFromDB() {
}

Integer PlanetHandle::GetSector()const{
    return GetInteger(SECTOR);
}

Text PlanetHandle::GetName()const{
    return GetText(NAME);
}

Float        PlanetHandle::GetSize()const{
    return GetFloat(SIZE);
}

Float       PlanetHandle::GetPopulation()const{
    return GetFloat(POPULATION);
}

Integer      PlanetHandle::GetCivLevel()const{
    return GetInteger(CIVLEVEL);
}

Timestamp    PlanetHandle::GetCivStartDate()const{
    return GetTimestamp(CIVSTARTDATE);
}

Timestamp    PlanetHandle::GetStartConstructionDate()const{
    return GetTimestamp(STARTCONSTRUCTIONDATE);
}

Integer      PlanetHandle::GetFlags()const{
    return GetInteger(FLAGS);
}

Text         PlanetHandle::GetOwner()const{
    return GetText(OWNER);
}

Timestamp    PlanetHandle::GetLastVisitDate()const{
  return GetTimestamp(LASTVISITDATE);
}

Text         PlanetHandle::GetAlliance()const{
    return GetText(ALLIANCE);
}

Text         PlanetHandle::GetLastVisitor()const{
    return GetText(LASTVISITOR);
}

Timestamp    PlanetHandle::GetCreationDate()const{
    return GetTimestamp(CREATIONDATE);
}

Float        PlanetHandle::GetCarbon()const{
    return GetFloat(CARBON);
}

Float        PlanetHandle::GetMetals()const{
    return GetFloat(METALS);
}

Float        PlanetHandle::GetPlasma()const{
    return GetFloat(PLASMA);
}


void         PlanetHandle::SetSector(Sector s){
    SetField(SECTOR,(int)s);
}

void         PlanetHandle::SetPopulation(int pop){
    SetField(POPULATION, pop);
}

void         PlanetHandle::SetCivLevel(int level){
    SetField(CIVLEVEL, level);
}

void         PlanetHandle::SetCivStartDateNow(){

}

void        PlanetHandle::SetLastVisitDate(const std::string& ts){
  SetField(LASTVISITDATE, ts);
}

void         PlanetHandle::SetOwner(PlayerHandlePtr owner){
    SetField(OWNER, owner->GetName());
}

void         PlanetHandle::SetAlliance(const std::string& alliance){
    SetField(ALLIANCE, alliance);
}

void         PlanetHandle::SetLastVisitor(PlayerHandlePtr visitor){
    SetField(LASTVISITOR, visitor->GetName());
}

void         PlanetHandle::SetCarbon(double carbon){
    SetField(CARBON, carbon);
}

void         PlanetHandle::SetMetals(double metals){
    SetField(METALS,metals);
}

void         PlanetHandle::SetPlasma(double plasma){
    SetField(PLASMA,plasma);
}

Text         PlanetHandle::GetClass() const {
    return GetText(CLASS);
}


PlanetClassHandlePtr  PlanetHandle::GetPlanetClassHandle() const{
    std::shared_ptr<Text> planetclassname = std::make_shared<Text>(PlanetClassHandle::FieldName(PlanetClassHandle::NAME), GetClass());
    PrimaryKey key(planetclassname);
    PlanetClassHandlePtr handle = std::make_shared<PlanetClassHandle>(m_dbconn, key);
    return handle;
}
