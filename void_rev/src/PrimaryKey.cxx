#include "PrimaryKey.h"
#include <vector>
#include "ResourceMaster.h"

using namespace std;


PrimaryKey::~PrimaryKey()
{
    if(!m_fields.size()) return;

    for(std::vector<Field*>::iterator i = m_fields.begin();
	i != m_fields.end();
	i++)
    {
	if(*i != 0)
	{
//	    ResourceMaster::GetInstance()->Log(AUDIT,"~PrimaryKey: deleteing " + (*i)->name() + ";" + (*i)->GetAsString());
	 
	    Field *field = *i;
	    delete field;
//	    m_fields.erase(i);
	}
    }
}

PrimaryKey::PrimaryKey()
{
}


PrimaryKey::PrimaryKey(Field *field)
{
    m_fields.push_back(field->clone());
}

PrimaryKey::PrimaryKey(Field *field1, Field *field2)
{
    m_fields.push_back(field1->clone());
    m_fields.push_back(field2->clone());

}

PrimaryKey::PrimaryKey(Field *field1, Field *field2, Field *field3)
{
    m_fields.push_back(field1->clone());
    m_fields.push_back(field2->clone());
    m_fields.push_back(field3->clone());
}


const PrimaryKey & PrimaryKey::operator=(const PrimaryKey &key)
{
    ResourceMaster::GetInstance()->Log(AUDIT,"PrimaryKey: Operator=");

    if(this == &key) return *this;

    m_fields.clear();

    for(std::vector<Field*>::const_iterator i = key.GetFieldsBegin();
	i != key.GetFieldsEnd();
	i++)
    {

	m_fields.push_back((*i)->clone());
    }    

    return *this;
}

PrimaryKey::PrimaryKey(const PrimaryKey &key)
{

    for(std::vector<Field*>::const_iterator i = key.GetFieldsBegin();
	i != key.GetFieldsEnd();
	i++)
    {

	m_fields.push_back((*i)->clone());
    }    
}


    
void PrimaryKey::SetFields(const vector<Field*> &fields)
{
    m_fields = fields;
}

std::vector<Field*>::const_iterator PrimaryKey::GetFieldsBegin() const
{
    return m_fields.begin();
}

std::vector<Field*>::const_iterator PrimaryKey::GetFieldsEnd() const
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


    std::vector<Field*>::const_iterator j = other.GetFieldsBegin();
    for(std::vector<Field*>::const_iterator i = m_fields.begin();
	i != m_fields.end() && j != other.GetFieldsEnd();
	i++, j++)
    {
	if(**i != **j)
	    return false;
    }

    return true;

}
