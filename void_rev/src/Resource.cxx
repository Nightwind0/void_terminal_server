

#include <vector>
#include <algorithm>
#include "PrimaryKey.h"

#include "Resource.h"
#include <iostream>
#include <sstream>


static const std::string RESOURCE_TYPE_STRINGS[] = {"LOGIN","PLAYER","SHIP", "SHIPTYPE","PLANET","OUTPOST","SECTOR""TERRITORY", "PLANETCLASS"};

//Provided for the map<> only..
Resource::Resource()
{
}

    
Resource::Resource(ResourceType type, const PrimaryKey &key):m_type(type),m_key(key)
{
    m_refcount = 0;

}

std::string Resource::GenerateID() const
{
    std::ostringstream heinous;

//Only calculate this crap once. I mean geeze.
   

    std::vector<FieldPtr> vFields;

    vFields.reserve(m_key.FieldCount());
  
    heinous << TypeToString(m_type);

    copy(m_key.GetFieldsBegin(),m_key.GetFieldsEnd(),back_inserter(vFields));
    sort(vFields.begin(),vFields.end());


    for(std::vector<FieldPtr>::const_iterator i = vFields.begin();
	i != vFields.end();
	i++)
    {
      heinous << (*i)->GetAsString();
      heinous << '.';
    }

    return heinous.str();
}

bool Resource::operator==(const Resource &other) const
{
    return GenerateID() == other.GenerateID();
}


bool Resource::operator<(const Resource &other) const
{
    // Comparing these is fun. You start with the resource type as a string...

    return GenerateID() < other.GenerateID();
 
}

std::string TypeToString(ResourceType type)
{



    switch(type)
    {
    case ResourceType::LOGIN:
	return std::string("LOGIN");
    case ResourceType::PLAYER:
	return std::string("PLAYER");
    case ResourceType::SHIP:
	return std::string("SHIP");
    case ResourceType::SHIPTYPE:
	return std::string("SHIPTYPE");
    case ResourceType::OUTPOST:
	return std::string("OUTPOST");
    
    default:
	return std::string("UNKNOWN");
    }
}
