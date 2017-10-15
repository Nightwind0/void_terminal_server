#ifndef VOID_PLANETCLASS_HANDLE_H
#define VOID_PLANETCLASS_HANDLE_H

#include "SerialObject.h"
#include "Resource.h"
#include "PrimaryKey.h"
#include <memory>
#include <string>
#include "void_util.h"

class PlanetClassHandle : public SerialObject
{

 public:
    PlanetClassHandle(DatabaseConnPtr dbconn, const PrimaryKey &key);
    ~PlanetClassHandle(){CloseDownObject();}


    virtual ResourceType GetType()const{return ResourceType::PLANETCLASS;};

    enum FIELDS {NAME, PLASMAMULTIPLIER, METALSMULTIPLIER, CARBONMULTIPLIER, MEANSIZE, VARIANCE, HABITABLE, MAXCIVLEVEL, DAILYGROWTH };
    
    static  std::string FieldName(int fieldnum);
    virtual std::string GetFieldName(int fieldnum)const;
    virtual void LoadFromDB();

    Text    GetName()const;
    Float   GetPlasmaMultiplier()const;
    Float   GetMetalsMultiplier()const;
    Float   GetCarbonMultiplier()const;
    Boolean GetHabitable()const;
    Integer GetMaxCivLevel()const;
    Float   GetDailyGrowth()const;
    virtual std::string DBTable()const { return "PlanetClass";};
};


using PlanetClassHandlePtr = std::shared_ptr<PlanetClassHandle>;

#endif
