
#ifndef VOID_COMMAND_TOW
#define VOID_COMMAND_TOW

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>

using std::string;

class VoidCommandTow : public VoidCommand
{
 public:
    VoidCommandTow(VoidServerThread *thread);
    virtual ~VoidCommandTow();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:

    std::list<int> GetValidShipList(int cur_sector, int cur_ship);

    VoidCommandTow();
    bool CommandTow(const std::string &arguments);

};

#endif
