#include "Universe.h"
#include "ResourceMaster.h"
#include <vector>
#include <set>
#include <deque>
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
    std::deque<PathNode*> open;
    std::vector<PathNode*> todelete;
    
	

    PathNode *startNode = new PathNode;
    todelete.push_back(startNode);
    startNode->sector = fromsector;
    startNode->parent = NULL;
    
    open.push_back(startNode);
    
    bool done = false;
    
    while (!open.empty() && !done)
    {
	
	PathNode *node = open.front();
	open.pop_front();
	
	if (node->sector == sec) 
	{
	    // path found!
	    while (node->parent != NULL)
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
		PathNode *nextnode = new PathNode;
		todelete.push_back(nextnode);
		nextnode->sector = *i;
		nextnode->parent = NULL;
		    

		bool isclosed = false;
		if(closed.find(*i) != closed.end()) isclosed = true;

		for(std::deque<PathNode*>::iterator j = open.begin();
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
    
    for(std::vector<PathNode*>::iterator i = todelete.begin();
	i != todelete.end();
	i++)
    {
	if(*i != NULL)
	{
	    delete *i;
	}
    }

    return path;

}
