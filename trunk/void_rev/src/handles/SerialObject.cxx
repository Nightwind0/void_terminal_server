#include "SerialObject.h"
#include <string>
#include <sstream>
#include "libpq-fe.h"
#include "void_util.h"
#include "ResourceMaster.h"

using namespace std;



std::string SerialObject::CreateWhereClause() const
{
    string str = "";

   if(m_key.FieldCount() == 1)
	str = (*m_key.GetFieldsBegin())->name() + " = \'" + PrepareForSQL((*m_key.GetFieldsBegin())->GetAsString()) + "\'";
    else
    {
	for(vector<Field*>::const_iterator i = m_key.GetFieldsBegin();
	    i != m_key.GetFieldsEnd();
		i++)
	{
	    vector<Field*>::const_iterator j = i + 1;
	    str += (*i)->name() + "=\'" +  (*i)->GetAsString() ;
	    if(j== m_key.GetFieldsEnd())
	    {
		str += "\';";
	    }
	    else str += "\' AND ";


	}
    }

   return str;

}

void SerialObject::SetFieldNull(int fieldnum)
{

    if(!m_lock) return; // TODO: Throw error

    if(m_new) return;



    PGresult *dbresult;
    
    std::string str = "UPDATE " + DBTable() + " SET " + GetFieldName(fieldnum) + " = null " + " WHERE " + CreateWhereClause() + ";";

    ResourceMaster::GetInstance()->Log(DEBUG2,str);
     
    dbresult = PQexec(m_dbconn, str.c_str());

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	throw  DBException(PQresultErrorMessage(dbresult));
    }

    PQclear(dbresult);
}

void SerialObject::UpdateFieldToDB(int fieldnum)
{

    if(!m_lock) return; // TODO: Throw error

    if(m_new) return;

    Field *field = m_fields[fieldnum];

    PGresult *dbresult;
    string str;

    str = "UPDATE " + DBTable() + " SET " + field->name() + "=\'" + PrepareForSQL(field->GetAsString()) + "\' WHERE " + CreateWhereClause() + ";";

    ResourceMaster::GetInstance()->Log(DEBUG2,str);
     
    dbresult = PQexec(m_dbconn, str.c_str());

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	throw  DBException(PQresultErrorMessage(dbresult));
    }

    PQclear(dbresult);



}


bool SerialObject::RecordExistsInDB()const
{
    string str = "SELECT COUNT(0) FROM " + DBTable() + " WHERE " + CreateWhereClause() + ";";

    PGresult *dbresult;

    dbresult = PQexec(m_dbconn,str.c_str());

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	throw DBException(PQresultErrorMessage(dbresult));
    }

    if(PQntuples(dbresult) != 1)
    {
	throw DBException("RecordExistsInDB: no tuples came back for count");
    }

    if(atoi(PQgetvalue(dbresult,0,0)) == 1)
    {
	return true;
    }
    else if(atoi(PQgetvalue(dbresult,0,0)) == 0)
    {
	return false;
    }
    else throw new DBException("RecordExistsInDB found more than 1 tuples matching this object in the db");


    PQclear(dbresult);

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


SerialObject::SerialObject(PGconn *dbconn, const PrimaryKey &key, bool isnew):m_dbconn(dbconn),m_key(key),m_new(isnew)
{
    m_lock = false;

}

void SerialObject::CloseDownObject()
{
    Insert(); // Only happens if we're locked AND new
    Unlock();

    for(std::map<int,Field*>::iterator i = m_fields.begin();
	i != m_fields.end();
	i++)
    {
	if(i->second != NULL)
	    delete (i->second);
    }
}

SerialObject::~SerialObject()
{
 




}


void SerialObject::Insert()
{
    if(m_lock && m_new)
    {
	PGresult *dbresult;
	std::string stmnt = "insert into " + DBTable() + " (";
	std::string vls;

	for(std::map<int,Field*>::iterator i = m_fields.begin();i != m_fields.end(); i++)
	{
	    stmnt += i->second->name();
	    vls += "\'" + PrepareForSQL(i->second->GetAsString()) + "\'";

	    i++;

	    if(i != m_fields.end())
		vls += ',';

	    if(i != m_fields.end())
		stmnt += ',';

	    --i;
	}
	stmnt += ") VALUES (" + vls + ");";

	ResourceMaster::GetInstance()->Log(DEBUG2,stmnt);
	dbresult = PQexec(m_dbconn,stmnt.c_str());
	
	if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
	{
	    throw DBException(PQresultErrorMessage(dbresult));
	}
	PQclear(dbresult);

	m_new = false;
    }
}



void SerialObject::DeleteFromDB()
{


    if(!m_lock) return; // TODO: Throw error for trying this crap
    string str = "DELETE FROM " + DBTable() + " WHERE " + CreateWhereClause() + ";";

    PGresult *dbresult = PQexec(m_dbconn, str.c_str());

    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	throw DBException(PQresultErrorMessage(dbresult));
    }

    PQclear(dbresult);

 
}




std::string SerialObject::GetField(int field, bool *b)const
{
    std::string str;
    std::string query = "SELECT " ;
    query += GetFieldName(field) ;
    query += " FROM " ;
    query += DBTable() ;
    query += " WHERE ";
    query +=  CreateWhereClause() + ";";

	PGresult *dbresult;

	ResourceMaster::GetInstance()->Log(DEBUG2, query);


	dbresult = PQexec(m_dbconn, query.c_str());

	if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
	{
	    std::string error = "SerialObject::GetField:" + std::string(PQresultErrorMessage(dbresult));
	    DBException e(error);
	    PQclear(dbresult);
	    throw e;
	}

	if(PQgetisnull(dbresult,0,0)) *b = true;
	else *b = false;

	str = PQgetvalue(dbresult,0,0);
	PQclear(dbresult);

	return std::string(str);
}

Timestamp SerialObject::GetTimestamp(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
	return *(Timestamp*)m_fields[field];
    }
   else
    {
	bool isnull;

	m_fields[field] = new Timestamp (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *(Timestamp*)m_fields[field];
    }
}

Integer SerialObject::GetInteger(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
	return *(Integer*)m_fields[field];
    }
   else
    {
	bool isnull;

	m_fields[field] = new Integer (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *(Integer*)m_fields[field];
    }
}

Text SerialObject::GetText(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
	return *(Text*)m_fields[field];
    }
   else
    {
	bool isnull;

	m_fields[field] = new Text (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *(Text*)m_fields[field];
    }
}

Float SerialObject::GetFloat(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
	return *(Float*)m_fields[field];
    }
   else
    {
	bool isnull;

	m_fields[field] = new Float (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *(Float*)m_fields[field];
    }
}

Boolean SerialObject::GetBoolean(int field)const
{
   if(m_lock && m_fields.count(field))
    {
	//If we've locked the object we can safely pull from cached values since they cannot have been changed.
	return *(Boolean*)m_fields[field];
    }
    else
    {
	bool isnull;

	m_fields[field] = new Boolean (GetFieldName(field), GetField(field, &isnull));

	if(isnull) m_fields[field]->SetNull();

	return *(Boolean*)m_fields[field];
    }
}







void SerialObject::SetField(int fieldnum, Field *field)
{
    if(!m_lock) return;

    if(m_fields.count(fieldnum)) delete m_fields[fieldnum];
    
    m_fields[fieldnum]= field;
    UpdateFieldToDB(fieldnum);
}



