#ifndef VOID_UNIVERSE_H
#define VOID_UNIVERSE_H

#include <vector>
#include <string>
#include <pqxx/pqxx>
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
    static int GetNumSectors(pqxx::connection_base&);
    static std::string GetToday(pqxx::connection_base&) ;
    static std::vector<StardockData> GetStardockData(pqxx::connection_base&);
    static std::deque<Sector> GetFlightPath(const std::set<Sector>& avoids, Sector fromsector, Sector tosector) ;

 private:
    struct PathNode
    {
      Sector sector;
      std::shared_ptr<PathNode> parent;
    };
    static int m_numsectors;
};


#endif

