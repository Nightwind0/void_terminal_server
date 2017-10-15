#ifndef __VOID_COMMAND_LAND_LOAD_H_
#define __VOID_COMMAND_LAND_LOAD_H_
#include "VoidCommandLandCargoBase.h"

class VoidCommandLandLoad : public VoidCommandLandCargoBase {
 public:
  VoidCommandLandLoad(VoidServerThread *thread, VoidCommandLand* pParent);
  virtual string GetCommandString()const;
  virtual string GetDescription()const;
  virtual string GetSyntax()const;
  
  virtual bool ClaimCommand(const string &command);
  virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
};

#endif
