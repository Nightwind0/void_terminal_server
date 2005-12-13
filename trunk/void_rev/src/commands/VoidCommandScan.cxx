#include "VoidCommandDisplay.h"
#include "void_util.h"
#include "VoidCommandScan.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "OutpostHandle.h"
#include <math.h>
#include <memory>

VoidCommandScan::VoidCommandScan(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandScan::~VoidCommandScan()
{
}

string VoidCommandScan::GetCommandString()const
{
    return "Scan";
}

string VoidCommandScan::GetDescription()const
{
    return "Display a remote sector using ship scanners";
}

string VoidCommandScan::GetSyntax()const
{
    return "scan <sector>";
}

    
bool VoidCommandScan::ClaimCommand(const string &command)
{
    if (CompStrings(command,"scan")) return true;

    return false;
}

bool VoidCommandScan::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    int sector=0;
    std::auto_ptr<ShipHandle> ship (create_handle_to_current_ship(get_thread()->GetPlayer()));
    int current_sector = ship->GetSector();
   
    if(arguments.size())
    {
	sector = atoi(arguments.c_str());
    }
    else
    {

	return false;
    }

    ScanSector(sector, current_sector);

    return true;
}
    
void VoidCommandScan::ScanSector(int sector, int current_sector)
{

    std::auto_ptr<ShipHandle> ship (create_handle_to_current_ship(get_thread()->GetPlayer()));

    ShipTypeHandle shiptype = ship->GetShipTypeHandle();
    int scandistance = shiptype.GetScanDistance();

    if(scandistance == 0)
    {
	Send(Color()->get(RED) + "This model of spacecraft does not include scanners." + endr);
	return;
    }

    PlayerHandle * pPlayer = get_thread()->GetPlayer();
    int turns = pPlayer->GetTurnsLeft();

    if(sector == current_sector)
    {
	// Deep scan
	// Deduct 1 turn.

	if ( turns < 1 )
	{
	    Send(Color()->get(RED) + "You do not have enough turns to scan.");
	    return;
	}
	pPlayer->Lock();
	pPlayer->SetTurnsLeft ( turns - 1  );
	pPlayer->Unlock();

	get_thread()->PostCommand("display", IntToString(sector) + " cloaked" );

    }
    else
    {
	// Distance scan
	std::list<int> noavoids;
	int distance = Universe::GetFlightPath(noavoids, current_sector,sector).size();

	if(distance > scandistance)
	{
	    Send(Color()->get(RED) + "That sector is too far away for your scanners." + endr);
	    return;
	}

	// Deduct turns equal to distance

	if ( turns < distance )
	{
	    Send(Color()->get(RED) + "You do not have enough turns to scan that far. It costs one turn per sector hop." + endr);
	    return;
	}

	pPlayer->Lock();
	pPlayer->SetTurnsLeft ( turns - distance  );
	pPlayer->Unlock();

	get_thread()->PostCommand ("display", IntToString(sector) );
	
    }
}

