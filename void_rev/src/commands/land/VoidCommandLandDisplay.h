#ifndef __VOID_COMMAND_LAND_DISPLAY_H_
#define __VOID_COMMAND_LAND_DISPLAY_H_

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include "VoidCommandLand.h"

class VoidCommandLandDisplay : public VoidCommand
{
 public:
  VoidCommandLandDisplay(VoidServerThread *thread, VoidCommandLand* pParent);
  virtual string GetCommandString()const;
  virtual string GetDescription()const;
  virtual string GetSyntax()const;
  
  virtual bool ClaimCommand(const string &command);
  virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
 private:
  VoidCommandLand* m_parent;
};

#endif
