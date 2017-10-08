#include "VoidCommandComputerDockList.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <sstream>

using std::ostringstream;
using std::left;
using std::right;

VoidCommandComputerDockList::VoidCommandComputerDockList(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandComputerDockList::~VoidCommandComputerDockList()
{
}

string VoidCommandComputerDockList::GetCommandString()const
{
    return "docks";
}

string VoidCommandComputerDockList::GetDescription()const
{
    return "view a list of docks in the universe";
}

string VoidCommandComputerDockList::GetSyntax()const
{
    return "docks";
}

    
bool VoidCommandComputerDockList::ClaimCommand(const string &command)
{
    if (CompStrings(command,"docks")) return true;

    return false;
}

bool VoidCommandComputerDockList::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
  std::ostringstream os;
  std::vector<StardockData> stardocks = Universe::GetStardockData(*get_thread()->GetDatabaseConn());
  for(const auto& it : stardocks) {
    Sector sector;
    std::string name;
    std::tie(sector,name) = it;
    os << Color()->get(LIGHTGREEN);
    os << left;
    os.width(7);
    os << sector;
    os << Color()->get(LIGHTBLUE);
    os.width(32);
    os << right << name;
    os << endr;
  }
  get_thread()->Send(os.str());
  return true;
}
