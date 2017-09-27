#include "Universe.h"
#include "ResourceMaster.h"
#include <vector>
#include <set>
#include <deque>
#include <memory>
#include "libpq-fe.h"



std::vector<int> Universe::GetAdjacentSectors(int sector)
{
    
    std::vector<int> sectors;
    sectors.reserve(12);
    
    ResourceMaster * RM = ResourceMaster::GetInstance();
    
    std::copy(RM->GetEdgesBegin(sector), RM->GetEdgesEnd(sector), back_inserter(sectors));
    
    
    return sectors;

}

int Universe::GetNumSectors(PGconn * dbconn)
{
    return 5000; // TODO: Get from configuration table
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

std::deque<int> Universe::GetFlightPath(std::list<int> avoids, int fromsector, int sec)
{
    std::deque<int> path;
    std::set<int> closed;
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
	    std::vector<int> adj = Universe::GetAdjacentSectors(node->sector);

		
	    for(std::vector<int>::iterator i= adj.begin();
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
