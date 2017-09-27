#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandReclaim.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "VoidServerThread.h"
#include "PlayerHandle.h"
#include <sstream>

VoidCommandReclaim::VoidCommandReclaim(VoidServerThread *thread):VoidCommand(thread),m_sentinel_tools(thread->GetDBConn(), thread->GetLocalSocket())
{
}
VoidCommandReclaim::~VoidCommandReclaim()
{
}

string VoidCommandReclaim::GetCommandString()const
{
    return "Reclaim";
}

string VoidCommandReclaim::GetDescription()const
{
    return "reclaim your sentinels from the sector";
}

string VoidCommandReclaim::GetSyntax()const
{
    return "reclaim <amount>";
}

    
bool VoidCommandReclaim::ClaimCommand(const string &command)
{
    if (CompStrings(command,"reclaim")) return true;

    return false;
}

bool VoidCommandReclaim::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    if(!arguments.size())
    {
	Send(Color()->get(RED) + "You must specify the amount to reclaim. " + endr);
	return false;
    }

    int num = atoi(arguments.c_str());

    if ( num <= 0) 
    {
	Send(Color()->get(RED) + "You must specify a positive amount to reclaim. " + endr);
	return false;
    }

    Reclaim(num);
    
    return true;
}

void VoidCommandReclaim::Reclaim(int num)
{
    std::unique_ptr<ShipHandle> ship( create_handle_to_current_ship( get_thread()->GetPlayer()));

    ShipTypeHandle shiptype = ship->GetShipTypeHandle();

    int sector = ship->GetSector();

    int sentinel_max = shiptype.GetMaxSentinels();
    int current_sentinels = ship->GetSentinels();

    int sentinel_room = sentinel_max - current_sentinels;

    if ( num > sentinel_room)
    {
	Send(Color()->get(RED) + "This ship only has capacity for " + Color()->get(WHITE) + IntToString(sentinel_room) 
	     + Color()->get(RED) + " more sentinels." + endr);

	return;
    }

    int sector_sentinels = m_sentinel_tools.GetPersonalSentinelCount( get_thread()->GetPlayer()->GetName(), sector );

    if(sector_sentinels == 0)
    {
	Send(Color()->get(RED) + "You have no sentinels in this sector." + endr);
	
	return;
    }
    else if ( num > sector_sentinels )
    {
	Send(Color()->get(RED) + "You only have " + Color()->get(WHITE) + IntToString(sector_sentinels) 
	     + Color()->get(RED) + " in this sector." + endr);

	return;
    }

    m_sentinel_tools.ReclaimAndReloadSentinels( num, get_thread()->GetPlayer(), ship.get(), sector );

    Send(Color()->get(WHITE) + IntToString(num) + Color()->get(GREEN) + " sentinels have been reclaimed to your ship." + endr);

}



