#include <string>
#include "void_util.h"
#include "ToolSet.h"
#include <sstream>
#include "ResourceMaster.h"
#include <cmath>

ToolSet::ToolSet(DatabaseConnPtr dbconn):m_dbconn(dbconn)
{
}

ToolSet::~ToolSet()
{
}

double ToolSet::g_random(int missiles, float mean_per, float deviation) const
{

    // the multiplier is actually the mean damage for each missile
    double multiplier = 2; /// @todo get from config table
    

    double total = 0;
    for(int i=0;i<missiles;i++)
    {
        total += g_rand() *deviation  + mean_per;
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
  pqxx::work work{*m_dbconn};

  std::ostringstream stmt;
  stmt <<  "insert into eventlog values(now(), " << work.quote(event.GetActor()) << "," << work.quote(event.GetSubject()) << ","
       << (int)event.GetType() << ',' << work.quote(event.GetMessage()) << ","
       << event.GetShipType() <<"," << work.quote(event.GetShipName()) << ");";
  
  work.exec(stmt.str());
  work.commit();
}

pqxx::result ToolSet::DBExec(const std::string &stmt)
{
    pqxx::work work{*m_dbconn};
    ResourceMaster::GetInstance()->Log(AUDIT, "~DBExec: " + work.esc(stmt) + "~");
    pqxx::result r = work.exec(stmt);
    work.commit();
    return r;
}
    
