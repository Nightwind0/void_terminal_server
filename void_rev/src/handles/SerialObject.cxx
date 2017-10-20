#include "SerialObject.h"
#include <string>
#include <sstream>
#include <pqxx/pqxx>
#include "void_util.h"
#include "ResourceMaster.h"

using namespace std;

const std::string kSetFieldNullStmt{"setFieldNull"};
const std::string kUpdateFieldStmt{"updateField"};
const std::string kCheckForRecordStmt{"checkForRecord"};

void SerialObject::create_prepared_statements() {
  //    std::string updatestr = "UPDATE " + DBTable() + " SET " + GetFieldName(fieldnum) + " = null " + " WHERE " + CreateWhereClause() + ";";
    
}

std::string SerialObject::CreateWhereClause() const
{
  std::ostringstream os;


   if(m_key.FieldCount() == 1)
     os << (*m_key.GetFieldsBegin())->name() << " = " << m_dbconn->quote((*m_key.GetFieldsBegin())->GetAsString());
    else
    {
	for(vector<FieldPtr>::const_iterator i = m_key.GetFieldsBegin();
	    i != m_key.GetFieldsEnd();
		i++)
	{
	    vector<FieldPtr>::const_iterator j = i + 1;
	    os << (*i)->name() << "=" << m_dbconn->quote((*i)->GetAsString()) ;
	    if(j== m_key.GetFieldsEnd())
	    {
	      os << ";";
	    }
	    else os << " AND ";
	}
    }

   return os.str();
}

void SerialObject::SetFieldNull(int fieldnum)
{
    if(!m_lock) return; // TODO: Throw error

    if(m_new) return;
    
    std::string updatestr = "UPDATE " + DBTable() + " SET " + GetFieldName(fieldnum) + " = null " + " WHERE " + CreateWhereClause() + ";";
    

    ResourceMaster::GetInstance()->Log(AUDIT,updatestr);
    pqxx::work work{*m_dbconn};
    pqxx::result result = work.exec(updatestr);
    work.commit();
}

void SerialObject::UpdateFieldToDB(int fieldnum)
{

    if(!m_lock) return; // TODO: Throw error

    if(m_new) return;

    FieldPtr field = m_fields[fieldnum];

    string str;

    str = "UPDATE " + DBTable() + " SET " + field->name() + " = " + m_dbconn->quote(field->GetAsString()) + " WHERE " + CreateWhereClause() + ";";

    ResourceMaster::GetInstance()->Log(AUDIT,str);

    pqxx::work work{*m_dbconn};
    pqxx::result result = work.exec(str);
    work.commit();
}


bool SerialObject::RecordExistsInDB()const
{
    string str = "SELECT COUNT(0) FROM " + DBTable() + " WHERE " + CreateWhereClause() + ";";

    pqxx::read_transaction work{*m_dbconn};
    pqxx::result r = work.exec(str);
    work.commit();

    if(r.size() != 1)
    {
	throw DBException("RecordExistsInDB: no tuples came back for count");
    }

    int count = r[0][0].as<int>();
    
    if(count == 1)
    {
	return true;
    }
    else if(count == 0)
    {
	return false;
    }
    else throw new DBException("RecordExistsInDB found more than 1 tuples matching this object in the db");
}
void SerialObject::SetNew()
{
    m_new = true;
}



void SerialObject::Lock()
{
    if(m_lock) return;

    m_lock = true;

    ResourceMaster::GetInstance()->RegisterResource(GetType(),m_key);
    
}

void SerialObject::Unlock()
{
    if(!m_lock) return;

    if(m_new)
    {
	
    }

    m_lock = false;

    ResourceMaster::GetInstance()->ReleaseResource(GetType(),m_key);
}


SerialObject::SerialObject(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew):m_dbconn(dbconn),m_key(key),m_new(isnew),m_lock(false)
{

}

void SerialObject::CloseDownObject()
{
    Insert(); // Only happens if we're locked AND new
    Unlock();
}

SerialObject::~SerialObject()
{

}


void SerialObject::Insert()
{
    if(m_lock && m_new)
    {
      std::ostringstream stmnt;
      stmnt << "insert into " << DBTable() << " (";
      std::ostringstream vls;

	for(std::map<int,FieldPtr>::iterator i = m_fields.begin();i != m_fields.end(); i++)
	{
	  stmnt << i->second->name();
	  vls <<  m_dbconn->quote(i->second->GetAsString());

	    i++;

	    if(i != m_fields.end())
	      vls << ',';

	    if(i != m_fields.end())
	      stmnt << ',';

	    --i;
	}
	stmnt << ") VALUES (" << vls.str() << ");";

	ResourceMaster::GetInstance()->Log(AUDIT,stmnt.str());
	pqxx::work work{*m_dbconn};
	work.exec(stmnt.str());
	work.commit();
	m_new = false;
    }
}



void SerialObject::DeleteFromDB()
{
    if(!m_lock) return; // TODO: Throw error for trying this crap
    string str = "DELETE FROM " + DBTable() + " WHERE " + CreateWhereClause() + ";";

    pqxx::work work{*m_dbconn};
    work.exec(str);
    work.commit();
}




std::string SerialObject::GetField(int field, bool *b)const
{
    std::ostringstream query;
    query << "SELECT ";
    query << GetFieldName(field) ;
    query << " FROM " ;
    query << DBTable() ;
    query << " WHERE ";
    query <<  CreateWhereClause() << ";";

    pqxx::read_transaction work{*m_dbconn};

    ResourceMaster::GetInstance()->Log(AUDIT, query.str());
    pqxx::result r = work.exec(query.str());

    work.commit();
    if(r[0][0].is_null()) {
      *b = true;
      return "";
    } else {
      *b = false;
      return r[0][0].as<std::string>();
    }
}

Timestamp SerialObject::GetTimestamp(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
      return *dynamic_pointer_cast<Timestamp>(m_fields[field]);
    }
   else
    {
	bool isnull;

	m_fields[field] = std::make_shared<Timestamp> (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *dynamic_pointer_cast<Timestamp>(m_fields[field]);
    }
}

Integer SerialObject::GetInteger(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
      return *dynamic_pointer_cast<Integer>(m_fields[field]);
    }
   else
    {
	bool isnull;

	m_fields[field] = std::make_shared<Integer> (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *dynamic_pointer_cast<Integer>(m_fields[field]);
    }
}

Text SerialObject::GetText(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
      return *dynamic_pointer_cast<Text>(m_fields[field]);
    }
   else
    {
	bool isnull;

	m_fields[field] = std::make_shared<Text> (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *dynamic_pointer_cast<Text>(m_fields[field]);
    }
}

Float SerialObject::GetFloat(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
      return *dynamic_pointer_cast<Float>(m_fields[field]);
    }
   else
    {
	bool isnull;

	m_fields[field] = std::make_shared<Float> (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *dynamic_pointer_cast<Float>(m_fields[field]);
    }
}

Boolean SerialObject::GetBoolean(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
      return *dynamic_pointer_cast<Boolean>(m_fields[field]);
    }
    else
    {
	bool isnull;

	m_fields[field] = std::make_shared<Boolean> (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *dynamic_pointer_cast<Boolean>(m_fields[field]);
    }
}







void SerialObject::SetField(int fieldnum, FieldPtr field)
{
    if(!m_lock) return;

    m_fields[fieldnum]= field;
    UpdateFieldToDB(fieldnum);
}


void SerialObject::SetField(int fieldnum, const std::string& value){
  SetField(fieldnum, std::make_shared<Text>(GetFieldName(fieldnum), value));
}

void SerialObject::SetField(int fieldnum, double value){
  SetField(fieldnum, std::make_shared<Float>(GetFieldName(fieldnum),std::to_string(value)));
}

void SerialObject::SetField(int fieldnum, int value){
  SetField(fieldnum, std::make_shared<Integer>(GetFieldName(fieldnum),std::to_string(value)));
}

void SerialObject::SetField(int fieldnum, bool value){
  std::string svalue = "f";
  if(value) svalue = "t";
  SetField(fieldnum, std::make_shared<Boolean>(GetFieldName(fieldnum), svalue));
}


void SerialObject::add_field(int id, const std::string name)
{
  m_field_names[id] = name;
}

std::string SerialObject::GetFieldName(int field)const
{
  return m_field_names[field];
}
