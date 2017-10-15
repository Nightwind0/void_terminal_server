#ifndef VOID_SHIP_HANDLE_H
#define VOID_SHIP_HANDLE_H

#include "SerialObject.h"
#include "Resource.h"
#include "PrimaryKey.h"
#include <string>




class ShipHandle : public SerialObject
{
 public:
    ShipHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew = false);
    virtual ~ShipHandle(){CloseDownObject();}
	

    ResourceType GetType()const{return ResourceType::SHIP;};

    enum FIELDS{NKEY,NAME,TYPE,ISALLIANCEOWNED,OWNER,ALLIANCE,ISPERSONAL,SENTINELS,MISSILES,MINES,TRACKERS,SHIELDS,PLASMA,METALS,CARBON,PEOPLE,DEBRIS,EXPLOSIVES,PROBES,HOLDS, ISCLOAKED,SECTOR,TOWSHIP};
    static std::string FieldName(int fieldnum);
    virtual std::string GetFieldName(int fieldnum)const;
    virtual void LoadFromDB();

    ShipTypeHandlePtr  GetShipTypeHandle()const;

    unsigned int GetHoldsUsed()const;
    unsigned int GetHoldsFree()const;

    Integer GetNkey()const;
    Text GetName()const;
    Integer GetTypeKey()const;
    Boolean GetIsAllianceOwned()const;
    Text GetOwner()const;
    Text GetAlliance()const;
    Boolean GetIsPersonal()const;
    Integer GetSentinels()const;
    Integer GetMissiles()const;
    Integer GetMines()const;
    Integer GetTrackers()const;
    Integer GetShields()const;
    Integer GetPlasma()const;
    Integer GetMetals()const;
    Integer GetCarbon()const;
    Integer GetPeople()const;
    Integer GetDebris()const;
    Integer GetExplosives()const;
    Integer GetProbes()const;
    Integer GetHolds()const;
    Boolean GetIsCloaked()const;
    Integer GetSector()const;
    Integer GetTow()const;
   
    
    void SetNkey(const int &nkey);
    void SetName(const std::string &name);
    void SetTypeKey(const int &type);
    void SetIsAllianceOwned(const bool &b);
    void SetOwner(const std::string &name);
    void SetAlliance(const std::string &alliance);
    void SetIsPersonal(const bool &b);
    void SetSentinels(const int &sentinels);
    void SetMissiles(const int &missiles);
    void SetMines(const int &mines);
    void SetTrackers(const int &trackers);
    void SetShields(const int &shields);
    void SetPlasma(const int &plasma);
    void SetMetals(const int &metal);
    void SetCarbon(const int &carbon);
    void SetPeople(const int &people);
    void SetDebris(const int &debris);
    void SetExplosives(const int &explosives);
    void SetProbes(const int &probes);
    void SetHolds(const int &holds);
    void SetIsCloaked(const bool &b);
    void SetSector(const int &sector);
    void SetTow(const int &ship);

    void BreakTow(); // set tow null

  
    virtual std::string DBTable()const { return "Ship";};

    static ShipHandlePtr HandleFromNkey( DatabaseConnPtr dbconn, const int ship);

};


using ShipHandlePtr = std::shared_ptr<ShipHandle>;


#endif
