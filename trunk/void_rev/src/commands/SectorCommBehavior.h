#ifndef VOID_SECTOR_COMMBEHAVIOR_H
#define VOID_SECTOR_COMMBEHAVIOR_H

#include "ShipHandle.h"
#include "VoidServerThread.h"
#include <list>
#include <string>

class SectorCommBehavior
{
 public:
    SectorCommBehavior(VoidServerThread *t);
    ~SectorCommBehavior();
 protected:
    void SendMsgToSector( const std::string &str, int sec, const std::string &exceptplayer);
    std::list<std::string> get_players_in_sector(int sector);

 private:
    VoidServerThread *m_thread;
};

#endif 
