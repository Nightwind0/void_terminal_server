
#ifndef VOID_COMMAND_DEPLOY
#define VOID_COMMAND_DEPLOY


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "SentinelTools.h"

using std::string;

class VoidCommandDeploy : public VoidCommand
{
 public:
    VoidCommandDeploy(VoidServerThread *thread);
    virtual ~VoidCommandDeploy();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    SentinelTools m_sentinel_tools;

    void Deploy(int num);

    VoidCommandDeploy();

};
    
#endif
