#include "VoidCommandComputerShipList.h"
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

VoidCommandComputerShipList::VoidCommandComputerShipList(VoidServerThread *thread):VoidCommand(thread),ShipListBehavior(thread)
{
}
VoidCommandComputerShipList::~VoidCommandComputerShipList()
{
}

string VoidCommandComputerShipList::GetCommandString()const
{
    return "ShipList";
}

string VoidCommandComputerShipList::GetDescription()const
{
    return "view a list of your personally owned ships";
}

string VoidCommandComputerShipList::GetSyntax()const
{
    return "shiplist";
}

    
bool VoidCommandComputerShipList::ClaimCommand(const string &command)
{
    if (CompStrings(command,"shiplist")) return true;

    return false;
}

bool VoidCommandComputerShipList::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandComputerShipList(arguments);
}




// Return false for command failure (bad arguments..)
bool VoidCommandComputerShipList::CommandComputerShipList(const std::string &arguments)
{
    ShowShipList(GetOwnedShips());
    return true;
}
