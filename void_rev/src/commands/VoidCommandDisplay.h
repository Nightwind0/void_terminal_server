
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
    
 private:
    std::string DisplayStardockInSector(int sector);
    std::string DisplayShipsInSector(int sector);
    std::string DisplayOutpostsInSector(int sector);
    std::string DisplaySector(int sector);
    VoidCommandDisplay();

};
    
#endif
