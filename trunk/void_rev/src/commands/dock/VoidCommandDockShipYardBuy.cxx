#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "ShipTypeHandle.h"
#include "VoidCommandDockShipYardBuy.h"

using std::string;


VoidCommandDockShipYardBuy::VoidCommandDockShipYardBuy(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandDockShipYardBuy::~VoidCommandDockShipYardBuy()
{
}

string VoidCommandDockShipYardBuy::GetCommandString()const
{
    return "Buy";
}
string VoidCommandDockShipYardBuy::GetDescription()const
{
    return "purchase a new star ship";
}
string VoidCommandDockShipYardBuy::GetSyntax()const
{
    return "buy <ship number>";
}
    
bool VoidCommandDockShipYardBuy::ClaimCommand(const string &command)
{
    if(CompStrings(command,"buy"))
	return true;
    else return false;
}

bool VoidCommandDockShipYardBuy::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    return DockShipYardBuy(arguments);

}
    

bool VoidCommandDockShipYardBuy::DockShipYardBuy(const string &arguments)
{

    PlayerHandle * player = get_thread()->GetPlayer();
    int shiptype = atoi(arguments.c_str());
    
    std::string query = "select shiptype.nkey,shiptype.sname, shipmanufacturer.sname, nforecolor, nbackcolor, ncost from shiptype,shipmanufacturer where bforsale = TRUE and shiptype.kmanufacturer = shipmanufacturer.nkey and shiptype.nkey = '" + IntToString(shiptype) + "';";

    PGresult *dbresult= get_thread()->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e("Buy ship error: " + std::string(PQresultErrorMessage(dbresult)));
	PQclear(dbresult);
	throw e;
    }


    if(PQntuples(dbresult) != 1 || (shiptype == 0 && arguments != "0"))
    {
	Send(Color()->get(RED) + "Sorry, that is not a vaild ship number." + endr);
	return false;
    }
    
    
    
    std::string nkey = PQgetvalue(dbresult,0,0);
    std::string name = PQgetvalue(dbresult,0,1);
    std::string manufacturer = PQgetvalue(dbresult,0,2);
    
    FGColor fgcolor = (FGColor)atoi(PQgetvalue(dbresult,0,3));
    BGColor bgcolor = (BGColor)atoi(PQgetvalue(dbresult,0,4));
    
    int cost = atoi(PQgetvalue(dbresult,0,5));

    PQclear(dbresult);

    Send(Color()->get(GREEN) + "The " + Color()->get(fgcolor,bgcolor) + manufacturer + ' ' + name + Color()->get(GREEN) +
	 " costs " + Color()->get(WHITE) + IntToString(cost) + Color()->get(GREEN) + " credits. ");
    Send("Do you still want to buy it? (Y/n) : ");

    std::string response = ReceiveLine();

    LOWERCASE(response);

    if(CompStrings(response,"yes"))
    {
	ShipHandle * oldship = create_handle_to_current_ship(get_thread()->GetPlayer());

	Integer shiptypei(ShipTypeHandle::FieldName(ShipTypeHandle::NKEY), oldship->GetTypeKey().GetAsString());
	PrimaryKey key(&shiptypei);
	ShipTypeHandle shiptypeh(get_thread()->GetDBConn(), key);

	int refund =((int)shiptypeh.GetCost() / 2);

	Send(Color()->get(GREEN));
	get_thread()->SendWordWrapped(std::string("Can we take that old ") + shiptypeh.GetShipTypeName(Color()) + Color()->get(GREEN) +
				      " off of your hands for " + Color()->get(WHITE) + IntToString(refund) +
				      Color()->get(GREEN) + " credits ? (y/N) ",80);

	std::string sellold = ReceiveLine();
	bool sellit = false;

	LOWERCASE(sellold);

	if(CompStrings(sellold,"no"))
	{
	    sellit = false;
	    refund = 0;
	}
	else if(CompStrings(sellold,"yes"))
	{
	    sellit = true;
	}

	int credits = (int)player->GetCredits();


	if ((credits + refund) < cost)
	{
	    std::string sorry = Color()->get(RED) + "Sorry, ";
	    if(sellit)
		sorry += "even with the trade-in, ";
	    sorry += "you can't afford this ship.";

	    sorry += endr;

	    Send(sorry);

	    return true;
	}
	


	ShipHandle * newship = get_thread()->CreateNewShip(shiptype);

       
	newship->Lock();
	newship->SetSector(oldship->GetSector());
	newship->Unlock();

	if(sellit)
	{
	    player->Lock();
	    player->SetCredits( player->GetCredits() + refund - cost); 
	    player->SetCurrentShip( newship->GetNkey());
	    player->Unlock();
	    
	    std::string deletestmt = "delete from ship where nkey = '" + oldship->GetNkey().GetAsString() + "';";
	    
	    PGresult *delresult = get_thread()->DBExec(deletestmt);


	    if(PQresultStatus(delresult) != PGRES_COMMAND_OK)
	    {
		
		DBException e("Delete ship error: " + std::string(PQresultErrorMessage(delresult)));
		PQclear(delresult);
		throw e;
	    }

	    PQclear(delresult);
    
    
	}
	else // Keeping old ship
	{
	    player->Lock();
	    player->SetCredits( player->GetCredits() - cost);
	    player->Unlock();
	    Send(Color()->get(GREEN));
	    get_thread()->SendWordWrapped(std::string("Would you like to use this new ship immediately? (Y/n) : "),80);
	    
	    std::string ride_home = ReceiveLine();
	    
	    LOWERCASE(ride_home);
	    
	    if(CompStrings(ride_home,"yes"))
	    {
		player->Lock();
		player->SetCurrentShip(newship->GetNkey());
		player->Unlock();
		Send(Color()->get(GREEN) + "Okay, you are all set." + endr +  " Your shuttle will take you to this ship when you leave star dock." + endr);
		
	    }
	
	}
	
	delete oldship;

	Send(Color()->get(GREEN) + "Thank you very much, please visit again!" + endr);

	return true;
    }
    else
    {
	Send(Color()->get(GREEN) + "No? Okay then. Good day." + endr);
	return true;
    }
    
    

    
}

 