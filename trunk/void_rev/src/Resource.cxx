

#include <vector>
#include <algorithm>
#include "PrimaryKey.h"

#include "Resource.h"
#include <iostream>

//enum RESOURCE_TYPE{LOGIN,PLAYER,SPACESHIP,PLANET,OUTPOST,SECTOR};

/*
    MutEx m_mutex;
    int m_refcount;
    RESOURCE_TYPE m_type;
    PrimaryKey m_key;
*/


static const char * RESOURCE_TYPE_STRINGS[] = {"LOGIN","PLAYER","SPACESHIP","PLANET","OUTPOST","SECTOR"};

//Provided for the map<> only..
Resource::Resource()
{
}

    
Resource::Resource(RESOURCE_TYPE type, const PrimaryKey &key):m_type(type),m_key(key)
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

std::string TypeToString(RESOURCE_TYPE type)
{



    switch(type)
    {
    case LOGINTYPE:
	return std::string("LOGIN");
    case PLAYERTYPE:
	return std::string("PLAYER");
    case SHIPTYPE:
	return std::string("SHIP");
    case SHIPTYPETYPE:
	return std::string("SHIPTYPE");
    case OUTPOSTTYPE:
	return std::string("OUTPOST");
    
    default:
	return std::string("UNKNOWN");
    }
}
