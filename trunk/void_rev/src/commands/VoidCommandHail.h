
#ifndef VOID_COMMAND_HAIL
#define VOID_COMMAND_HAIL


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandHail : public VoidCommand
{
 public:
    VoidCommandHail(VoidServerThread *thread);
    virtual ~VoidCommandHail();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    bool Hail(std::string player, std::string msg);

    VoidCommandHail();

};
    
#endif
