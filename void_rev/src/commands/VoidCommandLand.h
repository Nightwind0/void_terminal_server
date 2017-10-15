
#ifndef VOID_COMMAND_LAND_H
#define VOID_COMMAND_LAND_H

#include "PlanetClassHandle.h"
#include "PlanetHandle.h"
#include "VoidCommandSubCommands.h"
#include "VoidServerThread.h"
#include <string>
#include <list>

using std::string;
class Planet;

class VoidCommandLand : public VoidCommandSubCommands
{
 public:
    VoidCommandLand(VoidServerThread *thread);
    virtual ~VoidCommandLand();

    virtual string GetCommandString()const;
    virtual string GetDescription()const;
    virtual string GetSyntax()const;
    
    virtual bool ClaimCommand(const string &command);
    virtual bool HandleCommand(const string &command, const string &arguments, bool bFromPost);

    PlanetHandlePtr GetPlanet() const { return m_planet; }
 private:
    VoidCommandLand();

    void LandOn(PlanetHandlePtr planet, Sector sector);

    void DisplayIntro()const;
    void DisplayOutro()const;
    std::string GetPrompt()const;
    void DisplayOptionsFull();
    std::string GetOptionDescriptionColor()const;
    std::string GetOptionColor()const;

    PlanetHandlePtr m_planet;
};

#endif
