
#ifndef VOID_COMMAND_MOVE
#define VOID_COMMAND_MOVE

#include "VoidCommand.h"
#include "VoidCommandDisplay.h"
#include "VoidServerThread.h"
#include <string>

using std::string;

class VoidCommandMove : public VoidCommandDisplay 
{
 public:
    VoidCommandMove(VoidServerThread *thread);
    virtual ~VoidCommandMove();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 private:
    virtual bool move_player_to_sector(Sector sector) override;

    VoidCommandMove();
    bool CommandMove(const std::string &arguments);

};

#endif
