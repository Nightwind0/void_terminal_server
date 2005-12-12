#include <string>
#include "void_util.h"
#include "ToolSet.h"
#include <sstream>
#include "ResourceMaster.h"
#include <cmath>

ToolSet::ToolSet(PGconn * dbconn):m_dbconn(dbconn)
{
}

ToolSet::~ToolSet()
{
}

double ToolSet::g_random(int missiles) const
{
    double stddev = 1.75; /// @todo get from config table

    // the multiplier is actually the mean damage for each missile
    double multiplier = 2; /// @todo get from config table
    

    double total = 0;
    for(int i=0;i<missiles;i++)
    {
        total += g_rand() *stddev  + multiplier;
    }
                                                                                
    return total;
}




double ToolSet::g_rand() const
{    
    double x1, x2, w, y1, y2;
    
    do {
	x1 = 2.0 * ((double)random() / (double)RAND_MAX) - 1.0;
	x2 = 2.0 * ((double)random() / (double)RAND_MAX) - 1.0;
	w = x1 * x1 + x2 * x2;

    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    
    
    return y1;
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
    
