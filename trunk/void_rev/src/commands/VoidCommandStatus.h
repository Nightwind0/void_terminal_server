
#ifndef VOID_COMMAND_STATUS_H
#define VOID_COMMAND_STATUS_H


#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandStatus : public VoidCommand
{
 public:
    VoidCommandStatus(VoidServerThread *thread);
    virtual ~VoidCommandStatus();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    std::string ToBool(const std::string &value);
    std::string SendValue(const std::string &name, const std::string &value);
    std::string SendRow(const std::string &col1,const  std::string &col2,const std::string &col3);
    void ShowStatus();
    VoidCommandStatus();

};
    
#endif
