
#ifndef VOID_COMMAND_MAIL
#define VOID_COMMAND_MAIL


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandMail : public VoidCommand
{
 public:
    VoidCommandMail(VoidServerThread *thread);
    virtual ~VoidCommandMail();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    bool SendMail(const std::string &to);

    VoidCommandMail();

};
    
#endif
