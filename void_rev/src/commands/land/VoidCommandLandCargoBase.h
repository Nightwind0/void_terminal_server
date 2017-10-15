#ifndef __VOID_COMMAND_CARGO_BASE_H_
#define __VOID_COMMAND_CARGO_BASE_H_

#include "VoidCommand.h"
#include "VoidServerThread.h"
#include "VoidCommandLand.h"

class VoidCommandLandCargoBase : public VoidCommand
{
 public:
  VoidCommandLandCargoBase(VoidServerThread *thread, VoidCommandLand* pParent);
  enum class CargoType: unsigned char{INVALID, PEOPLE, PLASMA, METALS, CARBON};
 private:
  VoidCommandLand* m_parent;
 protected:
  PlanetHandlePtr get_planet() const { return m_parent->GetPlanet(); }
  std::string TypeToString(CargoType type)const;
  void DisplayCargoTypes()const;
  size_t ShipAmountOfType(ShipHandlePtr ship, CargoType type)const;
  size_t PlanetAmountOfType(CargoType type)const;
  void SetPlanetAmount(CargoType type, size_t amount)const;
  void SetShipAmount(ShipHandlePtr ship, CargoType type, size_t amount)const;
  CargoType ChooseCargoType()const;
  CargoType CargoTypeFromArgument(const std::string& arg)const;
  void TransferFromPlanet(CargoType type, unsigned int amount);
  void TransferToPlanet(CargoType type, unsigned int amount);
  VoidCommandLand* get_parent()const{ return m_parent; }
  
};

#endif
