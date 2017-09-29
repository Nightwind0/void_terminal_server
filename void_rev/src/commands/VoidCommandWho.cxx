#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandWho.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "VoidServerThread.h"
#include "PlayerHandle.h"
#include <sstream>

VoidCommandWho::VoidCommandWho(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandWho::~VoidCommandWho()
{
}

string VoidCommandWho::GetCommandString()const
{
    return "Who";
}

string VoidCommandWho::GetDescription()const
{
    return "List currently logged in players.";
}

string VoidCommandWho::GetSyntax()const
{
    return "who";
}

    
bool VoidCommandWho::ClaimCommand(const string &command)
{
    if (CompStrings(command,"who")) return true;

    return false;
}

bool VoidCommandWho::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    ListPlayers();

    return true;
}

void VoidCommandWho::ListPlayers()
{
    ResourceMaster *RM = ResourceMaster::GetInstance();

    get_thread()->Send(Color()->get(BLACK,BG_CYAN) + "    Who Is Playing    " + Color()->blackout() + endr);

    for(std::vector<VoidServerThread*>::iterator i = RM->GetServerThreadsBegin();
	i != RM->GetServerThreadsEnd();
	i++)
    {
	PlayerHandlePtr player = (*i)->GetPlayer();
	
	if(player)
	{
	    get_thread()->Send(Color()->get(GREEN) + (std::string)player->GetName() + Color()->blackout() +  endr);
	}
    }
}
    


