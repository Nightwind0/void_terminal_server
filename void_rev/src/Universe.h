#ifndef VOID_UNIVERSE_H
#define VOID_UNIVERSE_H

#include <vector>
#include <string>
#include "libpq-fe.h"
#include <list>
#include <deque>

class Universe
{
 public:
    Universe();
    ~Universe();

    static std::vector<int> GetAdjacentSectors(int sector);
    static int GetNumSectors(PGconn *dbconn);
    static std::string GetToday(PGconn *dbconn) ;
    static std::deque<int> GetFlightPath(std::list<int> avoids, int fromsector, int tosector) ;

 private:
    struct PathNode
    {
	int sector;
        PathNode * parent;
    };

};


#endif

