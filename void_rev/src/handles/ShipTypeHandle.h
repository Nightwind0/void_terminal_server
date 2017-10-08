#ifndef VOID_SHIPTYPE_HANDLE_H
#define VOID_SHIPTYPE_HANDLE_H

#include "SerialObject.h"
#include "Resource.h"
#include "PrimaryKey.h"
#include <memory>
#include <string>
#include "void_util.h"

class ShipTypeHandle : public SerialObject
{
 private:
    static const char * FIELD_NAMES[];
    std::string m_manufacturer;

 public:
 ShipTypeHandle(DatabaseConnPtr dbconn, const PrimaryKey &key):SerialObject(dbconn,key,false){}
    ~ShipTypeHandle(){CloseDownObject();}


    ResourceType GetType()const{return ResourceType::SHIP;};

    enum FIELDS{NKEY,MANUFACTURER,NAME,MAXMISSILES,MAXSHIELDS,MAXATTACK,MAXHOLDS,INITHOLDS,MAXSENTINELS,MAXTRACKERS,MAXMINES,MAXPEOPLE,MAXPROBES,TURNSPERSECTOR,HASWARPDRIVE, ISCLOAKABLE,HASANALYZER,SCANDISTANCE,COST,FORECOLOR,BACKCOLOR, TRANSRANGE,FORSALE};

    virtual std::string GetFieldName(int fieldnum)const;
    static std::string FieldName(int fieldnum){ return FIELD_NAMES[fieldnum]; }
    virtual void LoadFromDB();


    Integer GetNkey()const;
    Integer GetManufacturer()const;
    Text    GetName()const;
    Integer GetMaxMissiles()const;
    Integer GetMaxShields()const;
    Integer GetMaxAttack()const;
    Integer GetMaxHolds()const;
    Integer GetInitHolds()const;
    Integer GetMaxSentinels()const;
    Integer GetMaxTrackers()const;
    Integer GetMaxMines()const;
    Integer GetMaxPeople()const;
    Integer GetMaxProbes()const;
    Integer GetTurnsPerSector()const;
    Boolean GetHasWarpDrive()const;
    Boolean GetIsCloakable()const;
    Boolean GetHasAnalyzer()const;
    Integer GetScanDistance()const;
    Integer GetCost()const;
    Integer GetForeColor()const;
    Integer GetBackColor()const;
    Boolean GetForSale()const;
    Integer GetTransRange()const;
    std::string GetManufacturerName();
    std::string GetShipTypeName(std::shared_ptr<ColorType>  color);
    
    virtual std::string DBTable()const { return "ShipType";};
};


using ShipTypeHandlePtr = std::shared_ptr<ShipTypeHandle>;

#endif
