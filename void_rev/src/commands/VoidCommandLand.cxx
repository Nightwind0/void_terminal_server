
#include "VoidCommandLand.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "PlanetClassHandle.h"
#include "ShipHandle.h"
#include "Universe.h"
#include "VoidCommandLandDisplay.h"
#include "VoidCommandLandLoad.h"
#include "VoidCommandLandUnload.h"
#include "VoidCommandLandClaim.h"
#include "ShipTypeHandle.h"
#include <vector>

VoidCommandLand::VoidCommandLand(VoidServerThread *thread):VoidCommandSubCommands(thread)
{
  AddSubCommand(std::make_shared<VoidCommandLandUnload>(VoidCommandLandUnload(thread, this)));
  AddSubCommand(std::make_shared<VoidCommandLandDisplay>(VoidCommandLandDisplay(thread, this)));
  AddSubCommand(std::make_shared<VoidCommandLandLoad>(VoidCommandLandLoad(thread, this)));
  AddSubCommand(std::make_shared<VoidCommandLandClaim>(VoidCommandLandClaim(thread, this)));
}

VoidCommandLand::~VoidCommandLand()
{
}

string VoidCommandLand::GetCommandString()const
{
    return "land";
}

string VoidCommandLand::GetDescription()const
{
  return "land on a planet in this sector";
}

string VoidCommandLand::GetSyntax()const
{
    return "land";
}

    
bool VoidCommandLand::ClaimCommand(const string &command)
{
    if (CompStrings(command,"land")) return true;

    return false;
}


bool VoidCommandLand::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
  PlayerHandlePtr player = get_player();
  ShipHandlePtr ship = create_handle_to_current_ship(player);
  

  if(ship->GetIsCloaked()) {
    Send(Color()->get(RED) + "You cannot land while your ship is cloaked." + endr );
    return true;
  }

  Sector sector = ship->GetSector();

  std::string query = "select sname, fsize, kclass from planet where ksector = " + IntToString(sector) + ";";
  pqxx::result dbresult = get_thread()->DBExec(query);

  if (dbresult.size() == 0) {
    Send(Color()->get(RED) + "There are no planets in this sector." + endr);
    return true;
  } else if(dbresult.size() == 1) {
    PrimaryKey key(std::make_shared<Text>(PlanetHandle::FieldName(PlanetHandle::NAME), dbresult[0][0].as<std::string>()), std::make_shared<Integer>(PlanetHandle::FieldName(PlanetHandle::SECTOR), sector));
    PlanetHandlePtr planet = std::make_shared<PlanetHandle>(get_thread()->GetDatabaseConn(), key);
    LandOn(planet, sector);
  } else {
    std::ostringstream os;
    for(size_t i = 0; i < dbresult.size(); i++){
      os << Color()->get(BLACK, BG_WHITE) << i << '.';
      os << Color()->get(LIGHTBLUE, BG_BLACK) << dbresult[i][0].as<std::string>("Brigadoon")
	 << Color()->get(PURPLE) << ' ' << dbresult[i][2].as<std::string>("Unknown") << endr;
      // Send(Color()->get(WHITE, BG_GRAY), 
    }
    os << Color()->get(GREEN) << "Which planet? ";
    Send(os.str());
    int selection = -1;
    while(true){
      std::string selection_raw = get_thread()->ReceiveLine();
      selection = std::stoi(selection_raw);
      if(selection < 0 || (size_t)selection >= dbresult.size()){
	Send(Color()->get(RED));
	Send("Please choose one of the options shown.");
      } else {
	break;
      }
    }
    PrimaryKey key(std::make_shared<Text>(PlanetHandle::FieldName(PlanetHandle::NAME), dbresult[0][0].as<std::string>()), std::make_shared<Integer>(PlanetHandle::FieldName(PlanetHandle::SECTOR), sector));
    PlanetHandlePtr planet = std::make_shared<PlanetHandle>(get_thread()->GetDatabaseConn(), key);    
    LandOn(planet, sector);
  }
  
  return true;
}

void VoidCommandLand::LandOn(PlanetHandlePtr planet, Sector sector) {
  m_planet = planet;
  PlanetClassHandlePtr classhandle = planet->GetPlanetClassHandle();
  std::ostringstream os();

  DisplayIntro();

  int cur_turns = get_player()->GetTurnsLeft();
  get_player()->Lock();
  get_player()->SetTurnsLeft(cur_turns - 1);
  get_player()->Unlock();

  if(planet->GetLastVisitor().IsNull()) {
    	get_thread()->Send(Color()->get(YELLOW) + "You are the first visitor to this planet! 100 Points." + endr);
	get_player()->Lock();
	get_player()->SetPoints((int)get_player()->GetPoints() + 100);
	get_player()->Unlock();
  }

  planet->Lock();
  planet->SetLastVisitor(get_player());
  planet->SetLastVisitDate(Universe::GetToday(*get_thread()->GetDatabaseConn()));
  planet->Unlock();


  MainLoop();

  DisplayOutro();
}

void VoidCommandLand::DisplayOutro()const 
{
  std::ostringstream os;
  os << endr << endr << Color()->get(GREEN) << "You take off back into the sky. " << endr;
  Send(os.str());
}

std::string VoidCommandLand::GetPrompt()const 
{
    return Color()->get(PURPLE) + "Planet Command: " + Color()->get(WHITE);
}

void VoidCommandLand::DisplayOptionsFull()
{
    Send(Color()->get(BLUE, BG_WHITE) + "    Planet Functions    " + Color()->blackout() + endr);

    DisplayOptions();
    
    Send(Color()->get(WHITE) + "quit: " + Color()->get(PURPLE) + "return to main command prompt." + endr);
    Send(Color()->get(WHITE) + "help: " + Color()->get(PURPLE) + "view this list." + endr + endr);

}

std::string VoidCommandLand::GetOptionDescriptionColor()const 
{
    return Color()->get(GRAY);
}

std::string VoidCommandLand::GetOptionColor()const 
{
    return Color()->get(WHITE);
}



void VoidCommandLand::DisplayIntro()const
{
  std::ostringstream os;
  os << endr << endr << Color()->get(GREEN) << "Your ship lands safely on the planets surface. " << endr;
  if(!m_planet->GetOwner().IsNull()){
    os << Color()->get(LIGHTPURPLE) << "This planet is owned by " << std::string(m_planet->GetOwner());
    if (!m_planet->GetAlliance().IsNull()){
      os << '{' << std::string(m_planet->GetAlliance()) << '}';
    }
    os << endr;
  }
  Send(os.str());
}


