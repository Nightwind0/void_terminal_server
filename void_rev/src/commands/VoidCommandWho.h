
#ifndef VOID_COMMAND_WHO
#define VOID_COMMAND_WHO


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandWho : public VoidCommand
{
 public:
    VoidCommandWho(VoidServerThread *thread);
    virtual ~VoidCommandWho();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    void ListPlayers();

    VoidCommandWho();

};
    
#endif
