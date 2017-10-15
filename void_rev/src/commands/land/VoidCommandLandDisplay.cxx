#include "VoidCommandLandDisplay.h"
#include <iomanip>

VoidCommandLandDisplay::VoidCommandLandDisplay(VoidServerThread *thread, VoidCommandLand* pParent):VoidCommand(thread), m_parent(pParent){
}

std::string VoidCommandLandDisplay::GetCommandString()const
{
  return "display";
}

std::string VoidCommandLandDisplay::GetDescription()const
{
  return "view the current stats of the planet";
}

std::string VoidCommandLandDisplay::GetSyntax()const
{
  return "display";
}

bool VoidCommandLandDisplay::ClaimCommand(const string &command)
{
  return CompStrings(command, "display");
}

bool VoidCommandLandDisplay::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
  PlanetHandlePtr planet = m_parent->GetPlanet();
  std::ostringstream os;
  if(planet->GetOwner().IsNull()) {
    os << Color()->get(YELLOW) << "This planet is not claimed." << endr;
  } else {
    os << Color()->get(GREEN) << "This planet is owned by " << Color()->get(LIGHTPURPLE) <<  std::string(planet->GetOwner()) << '.' << endr;
  }
  os << Color()->get(LIGHTCYAN) << (std::string)planet->GetName() << endr;
  os << Color()->get(WHITE, BG_GREEN) << "Population" << " Civ.Level" << " Plasma" << " Metals" << " Carbon" << endr;
  os << Color()->get(WHITE, BG_BLACK);
  os <<  std::setw(10) << std::right << int(planet->GetPopulation())  << std::setw(10) <<std::right << planet->GetCivLevel() << std::setw(7) << std::right << int(planet->GetPlasma()) << std::setw(7) <<std::right << int(planet->GetMetals()) << std::setw(7) << std::right << int(planet->GetCarbon()) << endr; 
  os << Color()->get(WHITE, BG_BLACK) << endr;
  Send(os.str());
  return true;
}
