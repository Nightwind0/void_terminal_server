
#ifndef VOID_COMMAND_TRANSMIT
#define VOID_COMMAND_TRANSMIT


#include <string>
#include <list>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandTransmit : public VoidCommand
{
 public:
    VoidCommandTransmit(VoidServerThread *thread);
    virtual ~VoidCommandTransmit();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    std::list<std::string> get_players_in_sector(int);
    bool Transmit(std::string msg);

    VoidCommandTransmit();

};
    
#endif
