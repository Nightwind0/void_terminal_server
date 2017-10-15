#include "VoidCommandLandCargoBase.h"
#include "void_util.h"

VoidCommandLandCargoBase::VoidCommandLandCargoBase(VoidServerThread *thread, VoidCommandLand* pParent):VoidCommand(thread),m_parent(pParent){
}

void VoidCommandLandCargoBase::DisplayCargoTypes()const{
  std::stringstream os;
  os << Color()->get(WHITE, BG_BLUE) << "    Which Resource?    " << endr;
  os << Color()->get(WHITE, BG_BLUE) << "1." << Color()->get(BLUE) << " People" << endr
     << Color()->get(WHITE, BG_BLUE) << "2." << Color()->get(BLUE) << " Plasma" << endr
     << Color()->get(WHITE, BG_BLUE) << "3." << Color()->get(BLUE) << " Metals" << endr
     << Color()->get(WHITE, BG_BLUE) << "4." << Color()->get(BLUE) << " Carbon" << endr;
  Send(os.str());
}

VoidCommandLandCargoBase::CargoType VoidCommandLandCargoBase::CargoTypeFromArgument(const std::string& arg)const{
  if(CompStrings(arg, "invalid")){
    return CargoType::INVALID;
  }else if(CompStrings(arg, "people")){
    return CargoType::PEOPLE;
  } else if(CompStrings(arg, "plasma")) {
    return CargoType::PLASMA;
  } else if(CompStrings(arg, "metals")) {
    return CargoType::METALS;
  } else if(CompStrings(arg, "carbon")) {
    return CargoType::CARBON;
  }

  return CargoType::INVALID;
}

std::string VoidCommandLandCargoBase::TypeToString(CargoType type)const{
  switch(type){
  case CargoType::PEOPLE:
    return "People";
  case CargoType::PLASMA:
    return "Plasma";
  case CargoType::METALS:
    return "Metals";
  case CargoType::CARBON:
    return "Carbon";
  case CargoType::INVALID:
    return "INVALID";
  }
}

size_t VoidCommandLandCargoBase::PlanetAmountOfType(CargoType type)const{
  switch(type){
  case CargoType::PEOPLE:
    return get_planet()->GetPopulation();
  case CargoType::PLASMA:
    return get_planet()->GetPlasma();
  case CargoType::METALS:
    return get_planet()->GetMetals();
  case CargoType::CARBON:
    return get_planet()->GetCarbon();
  default:
    return 0;
  }
}

size_t VoidCommandLandCargoBase::ShipAmountOfType(ShipHandlePtr ship, CargoType type)const{
  switch(type){
  case CargoType::PEOPLE:
    return ship->GetPeople();
  case CargoType::PLASMA:
    return ship->GetPlasma();
  case CargoType::METALS:
    return ship->GetMetals();
  case CargoType::CARBON:
    return ship->GetCarbon();
  default:
    return 0;
  }
}

void VoidCommandLandCargoBase::SetPlanetAmount(CargoType type, size_t amount)const{
  switch(type){
  case CargoType::PEOPLE:
    get_planet()->SetPopulation(amount);
    break;
  case CargoType::PLASMA:
    get_planet()->SetPlasma(amount);
    break;
  case CargoType::METALS:
    get_planet()->SetMetals(amount);
    break;
  case CargoType::CARBON:
    get_planet()->SetCarbon(amount);
    break;
  default:
    break;
  }
}

void VoidCommandLandCargoBase::SetShipAmount(ShipHandlePtr ship, CargoType type, size_t amount)const{
  switch(type){
  case CargoType::PEOPLE:
    ship->SetPeople(amount);
    break;
  case CargoType::PLASMA:
    ship->SetPlasma(amount);
    break;
  case CargoType::METALS:
    ship->SetMetals(amount);
    break;
  case CargoType::CARBON:
    ship->SetCarbon(amount);
    break;
  default:
    break;
  }
}
void VoidCommandLandCargoBase::TransferFromPlanet(CargoType type, unsigned int amount){
  ShipHandlePtr ship = create_handle_to_current_ship(get_player());
  const unsigned int free_holds = ship->GetHoldsFree();
  const unsigned int current_planet_amount = PlanetAmountOfType(type);
  
  if(amount > free_holds){
    Send(Color()->get(RED) + "Looks like your ship doesn't have room for that." + endr);
    return;
  }

  if(amount > current_planet_amount){
    Send(Color()->get(RED) + "The planet does not have this much available." + endr);
    return;
  }
  

  get_planet()->Lock();
  SetPlanetAmount(type, current_planet_amount - amount);
  get_planet()->Unlock();

  ship->Lock();
  unsigned int ship_amount = ShipAmountOfType(ship,type);
  SetShipAmount(ship,type,ship_amount + amount);
  ship->Unlock();

  Send(Color()->get(GREEN) + "Your holds have been loaded." + endr);
}

void VoidCommandLandCargoBase::TransferToPlanet(CargoType type, unsigned int amount){
  ShipHandlePtr ship = create_handle_to_current_ship(get_player());
  const unsigned int current_ship_amount = ShipAmountOfType(ship, type);
  const unsigned int current_planet_amount = PlanetAmountOfType(type);

  if(amount > current_ship_amount){
    Send(Color()->get(RED) + "You don't have that much available on your ship." + endr);
    return;
  }

  ship->Lock();
  SetShipAmount(ship,type, current_ship_amount - amount);
  ship->Unlock();

  get_planet()->Lock();
  SetPlanetAmount(type, current_planet_amount + amount);
  get_planet()->Unlock();

  Send(Color()->get(GREEN) + "We've unloaded that from your ship and onto the planet." + endr);
}

VoidCommandLandCargoBase::CargoType VoidCommandLandCargoBase::ChooseCargoType()const{
  DisplayCargoTypes();
  while(true){
    std::string choice = get_thread()->ReceiveLine();
    int nc = std::stoi(choice);
    if(nc >= 1 && nc <=4){
      return static_cast<VoidCommandLandCargoBase::CargoType>(nc);
    } else {
      std::ostringstream os;
      os << Color()->get(RED, BG_BLACK);
      os << "Invalid choice" << endr;
      Send(os.str());
    }
  }
}
