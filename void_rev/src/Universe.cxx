#include "Universe.h"
#include "ResourceMaster.h"
#include <vector>
#include <set>
#include <deque>
#include <memory>
#include "libpq-fe.h"



std::vector<Sector> Universe::GetAdjacentSectors(Sector sector)
{
    
    std::vector<Sector> sectors;
    sectors.reserve(12);
    
    ResourceMaster * RM = ResourceMaster::GetInstance();
    
    std::copy(RM->GetEdgesBegin(sector), RM->GetEdgesEnd(sector), back_inserter(sectors));
    
    
    return sectors;

}

std::vector<StardockData> Universe::GetStardockData(PGconn * dbconn)
{
  std::vector<StardockData> docks;
  PGresult *dbresult = PQexec(dbconn, "SELECT nsector, sstardockname FROM Sectors WHERE bstardock = 't';");
  ResultGuard rg(dbresult);

  for(auto i = 0; i< PQntuples(dbresult); i++) {
    docks.emplace_back(std::make_tuple(atoi(PQgetvalue(dbresult,i,0)), PQgetvalue(dbresult,i,1)));
  }
  return docks;
}

int Universe::GetNumSectors(PGconn * dbconn)
{
  PGresult *dbresult = PQexec(dbconn, "SELECT COUNT(0) FROM Sectors;");
  std::string count = PQgetvalue(dbresult,0,0);
  PQclear(dbresult);

  return std::stoi(count);
}

std::string Universe::GetToday(PGconn * dbconn)
{
    std::string now;
    PGresult *dbresult;

    dbresult = PQexec(dbconn, "SELECT now();");

    now = PQgetvalue(dbresult,0,0);

    PQclear(dbresult);

    return now;
}

std::deque<Sector> Universe::GetFlightPath(const std::set<Sector>& avoids, Sector fromsector, Sector sec)
{
    std::deque<Sector> path;
    std::set<Sector> closed;
    std::deque<std::shared_ptr<PathNode>> open;
 
    std::shared_ptr<PathNode> startNode = std::make_shared<PathNode>();
  
    startNode->sector = fromsector;
    startNode->parent = nullptr;
    
    open.push_back(startNode);
    
    bool done = false;
    
    while (!open.empty() && !done)
    {
	
       std::shared_ptr<PathNode> node = open.front();
       open.pop_front();
	
	if (node->sector == sec) 
	{
	    // path found!
	    while (node->parent != nullptr)
	    {
		path.push_front(node->sector);
//		      node->parent = node;
		node = node->parent;
	    }
	    
	    done = true;
	    
	    
	}
	else
	{
	    closed.insert(node->sector);
		
	    // add neighbors to the open list
	    std::vector<Sector> adj = Universe::GetAdjacentSectors(node->sector);

		
	    for(std::vector<Sector>::iterator i= adj.begin();
		i != adj.end();
		i++)
	    {
	      std::shared_ptr<PathNode> nextnode = std::make_shared<PathNode>();
		nextnode->sector = *i;
		nextnode->parent = nullptr;
		    

		bool isclosed = false;
		if(closed.find(*i) != closed.end()) isclosed = true;

		for(auto j = open.begin();
		    j != open.end();
		    j++)
		{
		    if((*j)->sector == *i)
		    {
			isclosed = true;
			break;
		    }
		}

		if(!isclosed)
		{
		    nextnode->parent = node;
		    if(nextnode->sector != sec)
			open.push_back(nextnode);
		    else open.push_front(nextnode);

			
		}

			
	    }
		
	}//if
    }//while
    


    return path;

}
