#include "PlanetClassHandle.h"
#include <cassert>

const FieldPair fields[] = {
			    {PlanetClassHandle::NAME, "sname"},
			    {PlanetClassHandle::PLASMAMULTIPLIER, "fplasmamultiplier"},
			    {PlanetClassHandle::METALSMULTIPLIER, "fmetalsmultiplier"},
			    {PlanetClassHandle::CARBONMULTIPLIER, "fcarbonmultiplier"},
			    {PlanetClassHandle::HABITABLE, "bhabitable"},
			    {PlanetClassHandle::MAXCIVLEVEL, "nmaxcivlevel"},
			    {PlanetClassHandle::DAILYGROWTH, "fdailygrowth"}
};

std::string PlanetClassHandle::FieldName(int fieldnum) {
  for ( auto field : fields ) {
    if (field.first == fieldnum) {
      return field.second;
    }
  }
  assert(0);
  return "";
}

void PlanetClassHandle::LoadFromDB() {
}

std::string PlanetClassHandle::GetFieldName(int fieldnum)const{
  return FieldName(fieldnum);
}

PlanetClassHandle::PlanetClassHandle(DatabaseConnPtr dbconn, const PrimaryKey &key):SerialObject(dbconn,key,false){
  for( auto field : fields ) {
    add_field(field.first, field.second);
  }
}


Text    PlanetClassHandle::GetName()const{
  return GetText(NAME);
}

Float   PlanetClassHandle::GetPlasmaMultiplier()const{
  return GetFloat(PLASMAMULTIPLIER);
}

Float   PlanetClassHandle::GetMetalsMultiplier()const{
  return GetFloat(METALSMULTIPLIER);
}

Float   PlanetClassHandle::GetCarbonMultiplier()const{
  return GetFloat(CARBONMULTIPLIER);
}

Boolean PlanetClassHandle::GetHabitable()const{
  return GetBoolean(HABITABLE);
}

Integer PlanetClassHandle::GetMaxCivLevel()const{
  return GetInteger(MAXCIVLEVEL);
}

Float   PlanetClassHandle::GetDailyGrowth()const{
  return GetFloat(DAILYGROWTH);
}
