#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandDeploy.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "VoidServerThread.h"
#include "PlayerHandle.h"
#include <sstream>

VoidCommandDeploy::VoidCommandDeploy(VoidServerThread *thread):VoidCommand(thread),m_sentinel_tools(thread->GetDBConn(), thread->GetLocalSocket())
{
}
VoidCommandDeploy::~VoidCommandDeploy()
{
}

string VoidCommandDeploy::GetCommandString()const
{
    return "Deploy";
}

string VoidCommandDeploy::GetDescription()const
{
    return "deploy sentinels to the sector";
}

string VoidCommandDeploy::GetSyntax()const
{
    return "deploy <amount>";
}

    
bool VoidCommandDeploy::ClaimCommand(const string &command)
{
    if (CompStrings(command,"deploy")) return true;

    return false;
}

bool VoidCommandDeploy::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    if(!arguments.size())
    {
	Send(Color()->get(RED) + "You must specify the amount to deploy. " + endr);
	return false;
    }

    int num = atoi(arguments.c_str());

    if ( num <= 0) 
    {
	Send(Color()->get(RED) + "You must specify a positive amount to deploy. " + endr);
	return false;
    }

    Deploy(num);
    
    return true;
}

void VoidCommandDeploy::Deploy(int num)
{
    std::auto_ptr<ShipHandle> ship( create_handle_to_current_ship( get_thread()->GetPlayer()));

    int sector = ship->GetSector();

    int sector_max = m_sentinel_tools.SentinelCapacity( sector );

    if ( num > sector_max)
    {
	Send(Color()->get(RED) + "This sector only has capacity for " + Color()->get(WHITE) + IntToString(sector_max) 
	     + Color()->get(RED) + " more sentinels." + endr);

	return;
    }

    int ship_sentinels = ship->GetSentinels();

    if ( num > ship_sentinels )
    {
	Send(Color()->get(RED) + "You only have " + Color()->get(WHITE) + IntToString(ship_sentinels) 
	     + Color()->get(RED) + " on board." + endr);

	return;
    }

    m_sentinel_tools.DeploySentinelsAndUnload( num, get_thread()->GetPlayer(), ship.get(), sector );

    Send(Color()->get(WHITE) + IntToString(num) + Color()->get(GREEN) + " sentinels have been deployed to this sector." + endr);

}



