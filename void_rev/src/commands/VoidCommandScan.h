
#ifndef VOID_COMMAND_SCAN
#define VOID_COMMAND_SCAN


#include <string>
#include "VoidCommandDisplay.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandScan : public VoidCommand
{
 public:
    VoidCommandScan(VoidServerThread *thread);
    virtual ~VoidCommandScan();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 protected:

    void ScanSector(Sector sector, Sector current_sector);
    VoidCommandScan();

};
    
#endif
