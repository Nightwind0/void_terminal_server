#include "PrimaryKey.h"
#include <vector>
#include "ResourceMaster.h"

using namespace std;


PrimaryKey::~PrimaryKey()
{

}

PrimaryKey::PrimaryKey()
{
}


PrimaryKey::PrimaryKey(FieldPtr field)
{
    m_fields.push_back(field);
}

PrimaryKey::PrimaryKey(FieldPtr field1, FieldPtr field2)
{
    m_fields.push_back(field1);
    m_fields.push_back(field2);

}

PrimaryKey::PrimaryKey(FieldPtr field1, FieldPtr field2, FieldPtr field3)
{
    m_fields.push_back(field1);
    m_fields.push_back(field2);
    m_fields.push_back(field3);
}

PrimaryKey::PrimaryKey(const std::string &dbname, int field)
  :PrimaryKey(std::make_shared<Integer>(dbname, std::to_string(field))){
      
}
  
PrimaryKey::PrimaryKey(const std::string &dbname, const std::string& field):PrimaryKey(std::make_shared<Text>(dbname, field)){
  
}

PrimaryKey::PrimaryKey(const std::string& dbname,double field):PrimaryKey(std::make_shared<Float>(dbname, std::to_string(field))){
}


const PrimaryKey & PrimaryKey::operator=(const PrimaryKey &key)
{
    ResourceMaster::GetInstance()->Log(AUDIT,"PrimaryKey: Operator=");

    if(this == &key) return *this;

    m_fields.clear();

    for(std::vector<FieldPtr>::const_iterator i = key.GetFieldsBegin();
	i != key.GetFieldsEnd();
	i++)
    {

	m_fields.push_back(*i);
    }    

    return *this;
}

PrimaryKey::PrimaryKey(const PrimaryKey &key)
{

    for(std::vector<FieldPtr>::const_iterator i = key.GetFieldsBegin();
	i != key.GetFieldsEnd();
	i++)
    {

	m_fields.push_back(*i);
    }    
}


    
void PrimaryKey::SetFields(const vector<FieldPtr> &fields)
{
    m_fields = fields;
}

std::vector<FieldPtr>::const_iterator PrimaryKey::GetFieldsBegin() const
{
    return m_fields.begin();
}

std::vector<FieldPtr>::const_iterator PrimaryKey::GetFieldsEnd() const
{
    return m_fields.end();
}

int PrimaryKey::FieldCount() const
{

    return m_fields.size();
}
   
    
bool PrimaryKey::operator==(PrimaryKey &other) const
{

    if(other.FieldCount() != FieldCount())
	return false;


    std::vector<FieldPtr>::const_iterator j = other.GetFieldsBegin();
    for(std::vector<FieldPtr>::const_iterator i = m_fields.begin();
	i != m_fields.end() && j != other.GetFieldsEnd();
	i++, j++)
    {
	if(**i != **j)
	    return false;
    }

    return true;

}
