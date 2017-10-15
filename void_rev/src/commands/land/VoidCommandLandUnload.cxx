#include "VoidCommandLandUnload.h"

VoidCommandLandUnload::VoidCommandLandUnload(VoidServerThread *thread, VoidCommandLand* pParent):VoidCommandLandCargoBase(thread,pParent){
}

std::string VoidCommandLandUnload::GetCommandString()const{
  return "unload";
}

std::string VoidCommandLandUnload::GetDescription()const{
  return "unload colonists or resources onto the planet";
}

std::string VoidCommandLandUnload::GetSyntax()const{
  return "unload <people, plasma, metals, carbon>";
}

bool VoidCommandLandUnload::ClaimCommand(const string &command){
  return CompStrings(command, "unload");
}

bool VoidCommandLandUnload::HandleCommand(const string &command, const string &arguments, bool bFromPost){
  if(!get_planet()->GetOwner().IsNull()){
    if(get_planet()->GetOwner() != get_player()->GetName()){
      Send(Color()->get(RED) + "You do not own this planet!" + endr);
      return true;
    }
  }
  ShipHandlePtr ship = create_handle_to_current_ship(get_player());
  VoidCommandLandCargoBase::CargoType type = CargoTypeFromArgument(arguments);
  if(type == CargoType::INVALID){
    type = ChooseCargoType();
  }

  size_t current_amount = ShipAmountOfType(ship, type);

  Send(TypeToString(type) + endr) ;

  while(true) {
    Send(Color()->get(GREEN) + "You have "  + Color()->get(WHITE) + std::to_string(current_amount) + Color()->get(GREEN) + " available on your ship." + endr);
    Send(Color()->get(YELLOW) + "How many would you like to unload? " + Color()->get(WHITE) + '[' + Color()->get(GREEN) + std::to_string(current_amount) + Color()->get(WHITE) + ']');
    std::string amount_s = get_thread()->ReceiveLine();
    int amount = 0;
    if(amount_s == ""){
      amount = current_amount;
    } else {
      amount = std::stoi(amount_s);
    }

    if(amount == 0){
      return true;
    }

    if(amount > 0 && amount <= current_amount){
      TransferToPlanet(type, amount);
      return true;
    }
  }
  
  

  return true;
}
