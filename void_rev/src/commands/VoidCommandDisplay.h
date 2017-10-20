
#ifndef VOID_COMMAND_DISPLAY
#define VOID_COMMAND_DISPLAY


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandDisplay : public VoidCommand
{
 public:
    VoidCommandDisplay(VoidServerThread *thread);
    virtual ~VoidCommandDisplay();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 protected:
    std::string DisplayStardockInSector(Sector sector, size_t& count);
    std::string DisplayShipsInSector(Sector sector, bool showcloaked, size_t& count);
    std::string DisplayOutpostsInSector(Sector sector, size_t& count);
    std::string DisplayPlanetsInSector(Sector sector, size_t& count);
    std::string DisplaySector(Sector sector, bool showcloaked, bool& should_stop);
    std::string DisplaySentinelsInSector(Sector sector, size_t& count);

    VoidCommandDisplay();

};
    
#endif
