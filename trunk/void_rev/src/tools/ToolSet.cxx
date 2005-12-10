#include <string>
#include "void_util.h"
#include "ToolSet.h"
#include <sstream>
#include "ResourceMaster.h"

ToolSet::ToolSet(PGconn * dbconn):m_dbconn(dbconn)
{
}

ToolSet::~ToolSet()
{
}

void ToolSet::LogEvent(const Event &event )
{
    std::ostringstream stmt;
    stmt <<  "insert into eventlog values(now(), '" << event.GetActor() << "','" << event.GetSubject() << "','"
	 << (int)event.GetType() << "','" << PrepareForSQL(event.GetMessage()) << "','"
	 << event.GetShipType() <<"','" << PrepareForSQL(event.GetShipName()) << "');";
    
    PGresult * dbresult = DBExec(stmt.str());
    
    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {
	PQclear(dbresult);
	throw DBException(PQerrorMessage(m_dbconn));
    }
}

PGresult * ToolSet::DBExec(const std::string &stmt)
{
    ResourceMaster::GetInstance()->Log(DEBUG2, "~DBExec: " + PrepareForSQL(stmt) + "~");

    return PQexec(m_dbconn, stmt.c_str());
}
    
