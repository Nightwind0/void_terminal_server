#include "VoidCommandLandLoad.h"

VoidCommandLandLoad::VoidCommandLandLoad(VoidServerThread *thread, VoidCommandLand* pParent):VoidCommandLandCargoBase(thread,pParent){
}

std::string VoidCommandLandLoad::GetCommandString()const{
  return "load";
}

std::string VoidCommandLandLoad::GetDescription()const{
  return "load colonists or resources from the planet onto your ship";
}

std::string VoidCommandLandLoad::GetSyntax()const{
  return "load <people, plasma, metals, carbon>";
}

bool VoidCommandLandLoad::ClaimCommand(const string &command){
  return CompStrings(command, "load");
}

bool VoidCommandLandLoad::HandleCommand(const string &command, const string &arguments, bool bFromPost){
  ShipHandlePtr ship = create_handle_to_current_ship(get_player());
  VoidCommandLandCargoBase::CargoType type = CargoTypeFromArgument(arguments);
  if(type == CargoType::INVALID){
    type = ChooseCargoType();
  }

  Send(Color()->get(LIGHTGREEN) + TypeToString(type) + ':' + endr);

  size_t space  = ship->GetHoldsFree();
  size_t available = PlanetAmountOfType(type);

  size_t minimum = std::min(space,available);

  while(true) {
    Send(Color()->get(GREEN) + "The planet has " + Color()->get(WHITE) + std::to_string(available) + Color()->get(GREEN) + " available." + endr);
    Send(Color()->get(GREEN) + "You have "  + Color()->get(WHITE) + std::to_string(space) + Color()->get(GREEN) + " free hold space on your ship." + endr);
    Send(Color()->get(YELLOW) + "How many would you like to load? " + Color()->get(WHITE) + '[' + Color()->get(GREEN) + std::to_string(minimum) + Color()->get(WHITE) + ']');
    std::string amount_s = get_thread()->ReceiveLine();
    int amount = 0;
    if(amount_s == ""){
      amount = minimum;
    } else {
      amount = std::stoi(amount_s);
    }

    if(amount == 0){
      return true;
    }
    
    if(amount > 0 && amount <= minimum){
      TransferFromPlanet(type, amount);
      return true;
    }
  }
  
  

  return true;
}
