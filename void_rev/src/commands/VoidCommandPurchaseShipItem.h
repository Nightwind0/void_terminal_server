

#ifndef VOID_COMMAND_PURCHASE_SHIP_ITEM_H
#define VOID_COMMAND_PURCHASE_SHIP_ITEM_H

#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"

using std::string;

class VoidCommandPurchaseShipItem : public VoidCommand
{
 public:
    VoidCommandPurchaseShipItem(VoidServerThread *thread);
    virtual ~VoidCommandPurchaseShipItem();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);
    
 protected:

    virtual std::string GetItemName()const=0;
    virtual int GetMaxOfItem(ShipTypeHandle *shiptype)const=0;
    virtual int GetCurrentOfItem(ShipHandle * shiptype)const =0;
    virtual void SetItemNumber(ShipHandle *ship,int num)=0;
    virtual int GetPriceOfItem()const =0;
    
    void ShowItemStatus();
    bool PurchaseShipItem(const std::string &arguments);

 private:
    VoidCommandPurchaseShipItem();

};
    
#endif
