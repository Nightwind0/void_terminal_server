
#include "VoidCommandDockShipYard.h"
#include "VoidCommandDockShipYardList.h"
#include "VoidCommandDockShipYardBuy.h"
#include "VoidCommandDockShipYardSpec.h"
#include "VoidCommandDockShipYardHolds.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"


VoidCommandDockShipYard::VoidCommandDockShipYard(VoidServerThread *thread):VoidCommandSubCommands(thread)
{
    AddSubCommand(new VoidCommandDockShipYardList(thread));
    AddSubCommand(new VoidCommandDockShipYardBuy(thread));
    AddSubCommand(new VoidCommandDockShipYardSpec(thread));
    AddSubCommand(new VoidCommandDockShipYardHolds(thread));
}
VoidCommandDockShipYard::~VoidCommandDockShipYard()
{
}

string VoidCommandDockShipYard::GetCommandString()const
{
    return "shipyard";
}

string VoidCommandDockShipYard::GetDescription()const
{
    return "Buy and sell spaceships at the shipyard.";
}

string VoidCommandDockShipYard::GetSyntax()const
{
    return "shipyard";
}

    
bool VoidCommandDockShipYard::ClaimCommand(const string &command)
{
    if (CompStrings(command,"shipyard")) return true;

    return false;
}



void VoidCommandDockShipYard::DisplayOutro()const 
{
    Send(endr + Color()->get(GREEN) + "You take a shuttle away from the shipyard." + endr);
}

std::string VoidCommandDockShipYard::GetPrompt()const 
{
    return Color()->get(CYAN) + "Shipyard Command: " + Color()->get(WHITE);
}

void VoidCommandDockShipYard::DisplayOptionsFull()
{
    Send(endr + Color()->get(RED, BG_WHITE) + "    Shipyard Options    " + Color()->blackout() + endr);

    DisplayOptions();
    
    Send(GetOptionColor() + "quit: " + GetOptionDescriptionColor() + "leave the shipyard." + endr);
    Send(GetOptionColor() + "help: " + GetOptionDescriptionColor() + "view this list." + endr + endr);

}

std::string VoidCommandDockShipYard::GetOptionDescriptionColor()const 
{
    return Color()->get(GREEN);
}

std::string VoidCommandDockShipYard::GetOptionColor()const 
{
    return Color()->get(GRAY);
}



void VoidCommandDockShipYard::DisplayIntro()const
{

    PlayerHandle * player = get_player();
    ShipHandle * ship = create_handle_to_current_ship(player);
    
    get_thread()->SendClearScreen();

    Send(Color()->get(BLACK, BG_MAGENTA) + "            Welcome to the Shipyards             " + Color()->blackout() + endr + endr);
    

    get_thread()->SendWordWrapped(Color()->get(GREEN) + 
				  "The shipyard sprawls as far as the eye can see. It is a beautiful display of all shapes, colors, and sizes of wonderful gleaming "
				  "space craft. All of the major brands are represented. USST, the original spacecraft manufacturers from Earth, which grew from "
				  "collaborative efforts of NASA and private businesses. N-Tech, makers of the most sleek and efficient crafts. DPT, a maker of fine "
				  "star ships which are designed for warfare. Even the unremarkable IM&V line of affordable ships for the average consumer are here. "
				  "The only manufacturer absent from this shipyard is the mysterious \"Void\" group, allegedly responsible for creating the star "
				  "ships of legend which are said to wield unfathomable power." + endr, 80);

    
}


