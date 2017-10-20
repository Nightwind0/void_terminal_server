#include "VoidCommandLandClaim.h"
#include <iomanip>

VoidCommandLandClaim::VoidCommandLandClaim(VoidServerThread *thread, VoidCommandLand* pParent):VoidCommand(thread), m_parent(pParent){
}

std::string VoidCommandLandClaim::GetCommandString()const
{
  return "claim";
}

std::string VoidCommandLandClaim::GetDescription()const
{
  return "take ownership of this planet";
}

std::string VoidCommandLandClaim::GetSyntax()const
{
  return "claim";
}

bool VoidCommandLandClaim::ClaimCommand(const string &command)
{
  return CompStrings(command, "claim");
}

bool VoidCommandLandClaim::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
  PlanetHandlePtr planet = m_parent->GetPlanet();
  std::ostringstream os;
  if(planet->GetOwner().IsNull()) {
    planet->Lock();
    planet->SetOwner(get_thread()->GetPlayer());
    planet->Unlock();
    os << Color()->get(YELLOW) << "This planet is now yours!" << endr;
  } else {
    if(planet->GetOwner() == get_thread()->GetPlayer()->GetName()){
      os << Color()->get(RED)  << "You already own this planet." << endr;
    } else {
      os << Color()->get(LIGHTPURPLE) << "This planet is now longer owned by " << std::string(planet->GetOwner()) << '.' << endr;
      planet->Lock();
      planet->SetOwner(get_thread()->GetPlayer());
      planet->Unlock();
      os << Color()->get(YELLOW) << "This planet is now yours!" << endr;
    }
  }
  Send(os.str());
  return true;
}
