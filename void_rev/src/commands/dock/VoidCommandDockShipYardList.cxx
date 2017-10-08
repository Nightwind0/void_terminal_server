#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockShipYardList.h"
#include <sstream>

using std::ostringstream;

using std::string;

using std::right;
using std::left;


VoidCommandDockShipYardList::VoidCommandDockShipYardList(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandDockShipYardList::~VoidCommandDockShipYardList()
{
}

string VoidCommandDockShipYardList::GetCommandString()const
{
    return "List";
}
string VoidCommandDockShipYardList::GetDescription()const
{
    return "show available ships and their numbers";
}
string VoidCommandDockShipYardList::GetSyntax()const
{
    return "list";
}
    
bool VoidCommandDockShipYardList::ClaimCommand(const string &command)
{
    if(CompStrings(command,"list"))
	return true;
    else return false;
}

bool VoidCommandDockShipYardList::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    DockShipYardList();
    return true;
}
    

void VoidCommandDockShipYardList::DockShipYardList()
{
    get_thread()->SendClearScreen();
    
    std::string query = "select shiptype.nkey,shiptype.sname, shipmanufacturer.sname, nforecolor, nbackcolor, ncost from shiptype,shipmanufacturer where bforsale = TRUE and shiptype.kmanufacturer = shipmanufacturer.nkey order by shiptype.nkey;";

    pqxx::result dbresult = get_thread()->DBExec(query);


    ostringstream os;

    int numships = dbresult.size();


    Send(Color()->get(RED, BG_WHITE) + "        Ship List        " + Color()->blackout() + endr);


    for(auto row : dbresult)
    {

      std::string nkey = row[0].as<std::string>();
      std::string name = row[1].as<std::string>();
      std::string manufacturer = row[2].as<std::string>();
      std::string cost = row[5].as<std::string>();

      FGColor fgcolor = (FGColor)row[3].as<int>();
      BGColor bgcolor = (BGColor)row[4].as<int>();

      os << Color()->get(LIGHTCYAN) ;
      os.width(2);
      os << left << nkey ;
      os << Color()->get(GRAY) << ") ";
      os << Color()->get(fgcolor,bgcolor);
      os.width(7);
      os << left << manufacturer << ' ';
      os.width(15);
      os << left << name << ' ';
      os << Color()->get(WHITE);
      os.width(10);
      os << right << cost;
      os << endr;
      
      
      Send(os.str());
      os.str("");
    }

    
}

 
