
#ifndef VOID_COMMAND_RECLAIM
#define VOID_COMMAND_RECLAIM


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "SentinelTools.h"

using std::string;

class VoidCommandReclaim : public VoidCommand
{
 public:
    VoidCommandReclaim(VoidServerThread *thread);
    virtual ~VoidCommandReclaim();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    
    SentinelTools m_sentinel_tools;

    void Reclaim(int num);

    VoidCommandReclaim();

};
    
#endif
