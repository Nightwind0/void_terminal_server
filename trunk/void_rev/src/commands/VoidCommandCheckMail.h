
#ifndef VOID_COMMAND_CHECKMAIL
#define VOID_COMMAND_CHECKMAIL


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandCheckMail : public VoidCommand
{
 public:
    VoidCommandCheckMail(VoidServerThread *thread);
    virtual ~VoidCommandCheckMail();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    bool ReadMail();

    VoidCommandCheckMail();

};
    
#endif
