
#include "VoidCommandDockArmory.h"
#include "VoidCommandDockArmoryShields.h"
#include "VoidCommandDockArmoryMissiles.h"
#include "VoidCommandDockArmorySentinels.h"

#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"


VoidCommandDockArmory::VoidCommandDockArmory(VoidServerThread *thread):VoidCommandSubCommands(thread)
{
    AddSubCommand(new VoidCommandDockArmoryMissiles(thread));
    AddSubCommand(new VoidCommandDockArmoryShields(thread));
    AddSubCommand(new VoidCommandDockArmorySentinels(thread));
    // AddSubCommand(new VoidCommandDockArmoryBuy(thread));
    // AddSubCommand(new VoidCommandDockArmorySpec(thread));
}
VoidCommandDockArmory::~VoidCommandDockArmory()
{
}

string VoidCommandDockArmory::GetCommandString()const
{
    return "armory";
}

string VoidCommandDockArmory::GetDescription()const
{
    return "Buy missiles, mines, sentinels and shields.";
}

string VoidCommandDockArmory::GetSyntax()const
{
    return "armory";
}

    
bool VoidCommandDockArmory::ClaimCommand(const string &command)
{
    if (CompStrings(command,"armory")) return true;

    return false;
}



void VoidCommandDockArmory::DisplayOutro()const 
{
    Send(endr + Color()->get(GREEN) + "A shuttle takes you away." + endr);
}

std::string VoidCommandDockArmory::GetPrompt()const 
{
    return Color()->get(YELLOW) + "Armory Command: " + Color()->get(WHITE);
}

void VoidCommandDockArmory::DisplayOptionsFull()
{
    Send(endr + Color()->get(WHITE, BG_RED) + "    Armory Options    " + Color()->blackout() + endr);

    DisplayOptions();
    
    Send(GetOptionColor() + "quit: " + GetOptionDescriptionColor() + "leave the armory." + endr);
    Send(GetOptionColor() + "help: " + GetOptionDescriptionColor() + "view this list." + endr + endr);

}

std::string VoidCommandDockArmory::GetOptionDescriptionColor()const 
{
    return Color()->get(GREEN);
}

std::string VoidCommandDockArmory::GetOptionColor()const 
{
    return Color()->get(GRAY);
}



void VoidCommandDockArmory::DisplayIntro()const
{

    PlayerHandle * player = get_player();
    ShipHandle * ship = create_handle_to_current_ship(player);
    
    get_thread()->SendClearScreen();

    Send(Color()->get(BLACK, BG_RED) + "            Welcome to the Armory             " + Color()->blackout() + endr + endr);
    

    get_thread()->SendWordWrapped(Color()->get(GREEN) + 
				  "The armory is a junction where several different arms retailers have set up shop. Here they will meet all your warfare needs. "
				  "They can equip your ship with missiles and shields, or load sentinels on board so that you can desposit them "
				  "and defend your turf. If you need mines, they can provide those as well. Trackers, which are small devices "
				  "that float in space and cling to the hulls of passing ships, will transmit back to you, giving you the current "
				  "location of the ship in question. You can even get your ship packed with explosives, to insure the demise of "
				  "anyone who should attack it." + endr,80);


    
}


