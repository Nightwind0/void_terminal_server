#include "VoidCommandComputerCloak.h"
#include "ShipListBehavior.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <sstream>

using std::ostringstream;
using std::left;
using std::right;

VoidCommandComputerCloak::VoidCommandComputerCloak(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandComputerCloak::~VoidCommandComputerCloak()
{
}

string VoidCommandComputerCloak::GetCommandString()const
{
    return "cloak";
}

string VoidCommandComputerCloak::GetDescription()const
{
    return "cloak your ship so that it cannot be seen, or uncloak (cloak off)";
}

string VoidCommandComputerCloak::GetSyntax()const
{
    return "cloak [off]";
}

    
bool VoidCommandComputerCloak::ClaimCommand(const string &command)
{
    if (CompStrings(command,"cloak")) return true;

    return false;
}

bool VoidCommandComputerCloak::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandComputerCloak(arguments);
}




// Return false for command failure (bad arguments..)
bool VoidCommandComputerCloak::CommandComputerCloak(const std::string &arguments)
{

    bool to_cloak = true;

    ShipHandlePtr ship = create_handle_to_current_ship(get_player());

    ShipTypeHandlePtr shiptype = ship->GetShipTypeHandle();
    PlayerHandlePtr player = get_thread()->GetPlayer();

    if(!shiptype->GetIsCloakable())
    {
	Send(Color()->get(RED) + "This ship is not capable of cloaking." + endr);
	return true;
    }

    if(arguments == "off")
    {
	to_cloak = false;
    }
    
    ship->Lock();
    ship->SetIsCloaked ( to_cloak );
    ship->Unlock();

    if( to_cloak )
    {
	Send(Color()->get(GREEN) + "Your ship has been cloaked. Though your ship is now invisible, some scanners may " + endr
	+"be able to detect your presence. While cloaked, your ship may not attack, " + endr 
	 +  "move, or be towed, nor may you port at outposts, or dock at stardocks." + endr);
    }
    else
    {
	Send(Color()->get(GREEN) + "Your ship is now visible." + endr);
    }


    return true;
}
