
#ifndef VOID_COMMAND_PORT_H
#define VOID_COMMAND_PORT_H

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include <string>

using std::string;

class VoidCommandPort : public VoidCommand
{
 public:
    VoidCommandPort(VoidServerThread *thread);
    virtual ~VoidCommandPort();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:


    VoidCommandPort();
    bool CommandPort(const std::string &arguments);

    int SellGoods(const std::string &, int, int,int);
    int BuyGoods(const std::string &, int, int);


};

#endif
