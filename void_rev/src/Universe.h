#ifndef VOID_UNIVERSE_H
#define VOID_UNIVERSE_H

#include <vector>
#include <string>
#include "libpq-fe.h"
#include <set>
#include <memory>
#include <vector>
#include <deque>
#include "void_util.h"


class Universe
{
 public:
    Universe();
    ~Universe();

    static std::vector<Sector> GetAdjacentSectors(Sector sector);
    static int GetNumSectors(PGconn *dbconn);
    static std::string GetToday(PGconn *dbconn) ;
    static std::vector<StardockData> GetStardockData(PGconn * dbconn);
    static std::deque<Sector> GetFlightPath(const std::set<Sector>& avoids, Sector fromsector, Sector tosector) ;

 private:
    struct PathNode
    {
	int sector;
      std::shared_ptr<PathNode> parent;
    };

};


#endif

