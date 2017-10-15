
#ifndef VOID_COMMAND_BEAM
#define VOID_COMMAND_BEAM

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include "ShipListBehavior.h"
#include <string>

using std::string;

class VoidCommandBeam : public VoidCommand, private ShipListBehavior
{
 public:
    VoidCommandBeam(VoidServerThread *thread);
    virtual ~VoidCommandBeam();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    std::list<int> GetValidShipList(Sector cur_sector, size_t beamrange, int cur_ship);

    VoidCommandBeam();
    bool CommandBeam(const std::string &arguments);

};

#endif
