
#include "VoidCommandDock.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"
#include "VoidCommandDockShipYard.h"
#include "VoidCommandDockArmory.h"
#include <vector>

VoidCommandDock::VoidCommandDock(VoidServerThread *thread):VoidCommandSubCommands(thread)
{
  AddSubCommand(std::make_shared<VoidCommandDockShipYard>(VoidCommandDockShipYard(thread)));
  AddSubCommand(std::make_shared<VoidCommandDockArmory>(VoidCommandDockArmory(thread)));
}
VoidCommandDock::~VoidCommandDock()
{
}

string VoidCommandDock::GetCommandString()const
{
    return "Dock";
}

string VoidCommandDock::GetDescription()const
{
    return "Dock at a star dock.";
}

string VoidCommandDock::GetSyntax()const
{
    return "dock [opt. star dock command]";
}

    
bool VoidCommandDock::ClaimCommand(const string &command)
{
    if (CompStrings(command,"dock")) return true;

    return false;
}

bool VoidCommandDock::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    std::vector<std::string> argument_vector = WordsFromString(arguments);
    PlayerHandlePtr player = get_player();
    ShipHandlePtr ship = create_handle_to_current_ship(player);


    if(ship->GetIsCloaked())
    {
	Send(Color()->get(RED) + "You cannot dock while your ship is cloaked." + endr );
	return true;
    }

    int sector = ship->GetSector();

    std::string query = "select bstardock from sectors where nsector = '" + IntToString(sector) + "';";

    pqxx::result dbresult = get_thread()->DBExec(query);
    Boolean stardock("bstardock",dbresult[0][0].as<std::string>("f"), dbresult[0][0].is_null());


    if(!(bool)stardock)
    {
	Send(Color()->get(RED) + "There is no star dock in this sector." + endr);
	return true;
    }
    
    DisplayIntro();
    DisplayOptionsFull();
    
    if(argument_vector.size()) DoCommand(argument_vector[0], arguments.substr(argument_vector[0].size() +1), bFromPost);

    MainLoop();

    DisplayOutro();
    
    return true;
}



void VoidCommandDock::DisplayOutro()const 
{
    Send(endr + Color()->get(GREEN) + "The shuttle takes you back to your ship and you take off." + endr);
}

std::string VoidCommandDock::GetPrompt()const 
{
    return Color()->get(PURPLE) + "Stardock Command: " + Color()->get(WHITE);
}

void VoidCommandDock::DisplayOptionsFull()
{
    Send(Color()->get(BLACK, BG_WHITE) + "    Stardock Options    " + Color()->blackout() + endr);

    DisplayOptions();
    
    Send(Color()->get(WHITE) + "quit: " + Color()->get(GRAY) + "leave stardock." + endr);
    Send(Color()->get(WHITE) + "help: " + Color()->get(GRAY) + "view this list." + endr + endr);

}

std::string VoidCommandDock::GetOptionDescriptionColor()const 
{
    return Color()->get(GRAY);
}

std::string VoidCommandDock::GetOptionColor()const 
{
    return Color()->get(WHITE);
}



void VoidCommandDock::DisplayIntro()const
{

    PlayerHandlePtr player = get_player();
    ShipHandlePtr ship = create_handle_to_current_ship(player);

    int sector = ship->GetSector();

    std::string query = "select sstardockname from sectors where nsector = '" + IntToString(sector) + "';";


    pqxx::result dbresult = get_thread()->DBExec(query);

    Text stardock("sstardockname",dbresult[0][0].as<std::string>(), dbresult[0][0].is_null());


    get_thread()->SendClearScreen();

    Send(Color()->get(WHITE, BG_BLUE) + "            Welcome to " + (std::string)stardock + "            " + Color()->blackout() + endr + endr);

    get_thread()->SendWordWrapped(Color()->get(GREEN) +
		     "The busy space port accomodates travelers, dealers, and shoppers from across the galaxy. All around you, people and creatures "
		     "are boarding and disembarking. Large space ships fill docking stations for miles in all directions. Small shuttles carry visitors "
		     "to the various points of interest, from shopping centers and shipyards, to bustling businesses and seedy waste areas filled with "
		     "pirates and black market traders." + endr + endr, 80);





    //                ................................................................................

    
}


