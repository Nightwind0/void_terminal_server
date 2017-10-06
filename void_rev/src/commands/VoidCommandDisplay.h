
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
    std::string DisplayStardockInSector(Sector sector);
    std::string DisplayShipsInSector(Sector sector, bool showcloaked=false);
    std::string DisplayOutpostsInSector(Sector sector);
    std::string DisplaySector(Sector sector, bool showcloaked=false);
    std::string DisplaySentinelsInSector(Sector sector);

    VoidCommandDisplay();

};
    
#endif
