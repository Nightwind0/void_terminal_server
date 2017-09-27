

#include <vector>
#include <algorithm>
#include "PrimaryKey.h"

#include "Resource.h"
#include <iostream>


static const char * RESOURCE_TYPE_STRINGS[] = {"LOGIN","PLAYER","SPACESHIP","PLANET","OUTPOST","SECTOR"};

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
   
    std::string heinous = "";


//Only calculate this crap once. I mean geeze.
   

    std::vector<Field*> vFields;

    vFields.reserve(m_key.FieldCount());
  

    heinous.reserve(100); // Otherwise it may have to resize a bunch which is costly
//    std::cerr <<(int) m_type << std::endl;
    heinous += TypeToString(m_type);

    copy(m_key.GetFieldsBegin(),m_key.GetFieldsEnd(),back_inserter(vFields));
    sort(vFields.begin(),vFields.end());


    for(std::vector<Field*>::const_iterator i = vFields.begin();
	i != vFields.end();
	i++)
    {
	heinous += (*i)->GetAsString();
	heinous += '.';
    }

    return heinous;
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
