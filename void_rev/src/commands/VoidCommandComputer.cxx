
#include "VoidCommandComputer.h"
#include "VoidCommandComputerShipList.h"
#include "VoidCommandComputerPlayers.h"
#include "VoidCommandComputerCloak.h"
#include "VoidCommandComputerDockList.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"
#include <vector>

VoidCommandComputer::VoidCommandComputer(VoidServerThread *thread):VoidCommandSubCommands(thread)
{
  AddSubCommand(std::make_shared<VoidCommandComputerShipList>(thread));
  AddSubCommand(std::make_shared<VoidCommandComputerPlayers>(thread));
  AddSubCommand(std::make_shared<VoidCommandComputerCloak>(thread));
  AddSubCommand(std::make_shared<VoidCommandComputerDockList>(thread));
}
VoidCommandComputer::~VoidCommandComputer()
{
}

string VoidCommandComputer::GetCommandString()const
{
    return "Computer";
}

string VoidCommandComputer::GetDescription()const
{
    return "access the onboard computer functions";
}

string VoidCommandComputer::GetSyntax()const
{
    return "computer";
}

    
bool VoidCommandComputer::ClaimCommand(const string &command)
{
    if (CompStrings(command,"computer")) return true;

    return false;
}


void VoidCommandComputer::DisplayOutro()const 
{

}

std::string VoidCommandComputer::GetPrompt()const 
{
    return Color()->get(LIGHTCYAN) + "Computer Command: " + Color()->get(WHITE);
}

void VoidCommandComputer::DisplayOptionsFull()
{
    Send(Color()->get(LIGHTCYAN, BG_WHITE) + "    Computer Functions    " + Color()->blackout() + endr);

    DisplayOptions();
    
    Send(Color()->get(WHITE) + "quit: " + Color()->get(BROWN) + "return to main command prompt." + endr);
    Send(Color()->get(WHITE) + "help: " + Color()->get(BROWN) + "view this list." + endr + endr);

}

std::string VoidCommandComputer::GetOptionDescriptionColor()const 
{
    return Color()->get(BROWN);
}

std::string VoidCommandComputer::GetOptionColor()const 
{
    return Color()->get(WHITE);
}



void VoidCommandComputer::DisplayIntro()const
{

    
        
}


