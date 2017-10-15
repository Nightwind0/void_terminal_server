#ifndef VOID_PLANET_HANDLE_H
#define VOID_PLANET_HANDLE_H


#include <string>
#include "Resource.h"
#include "PrimaryKey.h"
#include "SerialObject.h"
#include "PlayerHandle.h"
#include "PlanetClassHandle.h"

class PlanetHandle;
using PlanetHandlePtr = std::shared_ptr<PlanetHandle>;

class PlanetHandle : public SerialObject
{
 public:
    PlanetHandle(DatabaseConnPtr dbconn, const PrimaryKey &key, bool isnew = false);
    virtual ~PlanetHandle(){CloseDownObject();}
	

    ResourceType GetType() const {return ResourceType::PLANET;}

    enum FIELDS { SECTOR, NAME, SIZE, PLANETCLASS, POPULATION, CIVLEVEL, CIVSTARTDATE, STARTCONSTRUCTIONDATE, FLAGS, OWNER, ALLIANCE, LASTVISITOR, CREATIONDATE, CARBON, METALS, PLASMA, CLASS, LASTVISITDATE };
    // Polymorphic vesion
    virtual std::string GetFieldName(int fieldnum)const;
    // Class version
    static std::string FieldName(int fieldnum);
    Integer      GetSector()const;
    Text         GetName()const;
    Float        GetSize()const;
    Float        GetPopulation()const;
    Integer      GetCivLevel()const;
    Timestamp    GetCivStartDate()const;
    Timestamp    GetStartConstructionDate()const;
    Integer      GetFlags()const;
    Text         GetOwner()const;
    Text         GetAlliance()const;
    Text         GetLastVisitor()const;
    Timestamp    GetCreationDate()const;
    Timestamp    GetLastVisitDate()const;
    Float        GetCarbon()const;
    Float        GetMetals()const;
    Float        GetPlasma()const;
    Text         GetClass()const;
    
    void         SetSector(Sector s);
    void         SetPopulation(int pop);
    void         SetCivLevel(int level);
    void         SetCivStartDateNow();
    void         SetOwner(PlayerHandlePtr owner);
    void         SetAlliance(const std::string& alliance);
    void         SetLastVisitor(PlayerHandlePtr visitor);
    void         SetLastVisitDate(const std::string& ts);
    void         SetCarbon(double carbon);
    void         SetMetals(double metals);
    void         SetPlasma(double plasma);
    
    virtual void LoadFromDB();

    PlanetClassHandlePtr  GetPlanetClassHandle() const;

  
    virtual std::string DBTable() const { return "Planet";}
};

using PlanetHandlePtr = std::shared_ptr<PlanetHandle>;


#endif
