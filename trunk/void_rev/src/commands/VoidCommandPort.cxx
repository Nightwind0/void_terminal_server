
#include "VoidCommandPort.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"


VoidCommandPort::VoidCommandPort(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandPort::~VoidCommandPort()
{
}

string VoidCommandPort::GetCommandString()const
{
    return "Port";
}

string VoidCommandPort::GetDescription()const
{
    return "Port at a space port to trade goods";
}

string VoidCommandPort::GetSyntax()const
{
    return "port";
}

    
bool VoidCommandPort::ClaimCommand(const string &command)
{
    if (CompStrings(command,"port")) return true;

    return false;
}

bool VoidCommandPort::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    return CommandPort(arguments);
}



int VoidCommandPort::BuyGoods(const std::string &goods, int price_each,  int units_available)
{
    get_thread()->Send(Color()->get(GREEN) + "You have " + Color()->get(YELLOW) +  IntToString(units_available) + " " + goods  + endr);
    get_thread()->Send(Color()->get(GREEN) + "We pay " + Color()->get(WHITE) + IntToString(price_each) + Color()->get(GREEN) + " per unit." + endr);
    get_thread()->Send(Color()->get(GREEN) + "We will pay you " + Color()->get(BLACK,BG_WHITE) +  IntToString(price_each * units_available) + Color()->get(GREEN) +  " for the lot. Y/n");
    
    string answer = get_thread()->ReceiveLine();
    
    if(CompStrings(answer,"yes"))
    {

	return units_available;

    }
    
    return 0;
}

bool VoidCommandPort::CommandPort(const std::string &arguments)
{
    PlayerHandle * player = get_player();
    ShipHandle * ship = create_handle_to_current_ship(player);
    std::string query = "select sname, bspecial, bbuyplasma, bbuymetals, bbuycarbon,fbuyrate,fsellrate, kdiscoverer, klastvisitor, extract (day from age(dlastvisit,now())), extract (hour from age(dlastvisit,now())), extract(minute from age(dlastvisit,now())) from outpost where ksector = ";

    query += ship->GetSector().GetAsString();

    query += ";";

    PGresult *dbresult;

    dbresult = get_thread()->DBExec(query);


    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	throw e;
    }


    int numoutposts = PQntuples(dbresult);
    int whichoutpost;

    if(numoutposts == 0)
    {
	get_thread()->Send(Color()->get(LIGHTRED) + "There are no outposts available for porting in this sector." + endr);
	return true; // command was okay, just not appropriate here
    }

    if(numoutposts == 1)
    {
	whichoutpost = 0;
    }
    else
    {
	get_thread()->Send(Color()->get(GRAY) + "Select an outpost:" + endr);

	for(int i=0;i<numoutposts;i++)
	{
	    get_thread()->Send(Color()->get(WHITE)+ IntToString(i+1) + ")" + Color()->get(LIGHTPURPLE) + PQgetvalue(dbresult,0,i) + endr);
	}

	std::string selection = get_thread()->ReceiveLine();

	whichoutpost = atoi(selection.c_str()) -1;
    }
    
    std::string outpostname = PQgetvalue(dbresult,whichoutpost,0);
    
//    PQclear(dbresult);

    Text outpostt(OutpostHandle::FieldName(OutpostHandle::NAME), outpostname);
    PrimaryKey key(&outpostt);
    
    
    OutpostHandle *outpost = new OutpostHandle(get_thread()->GetDBConn(),key,false);
    
    
    get_thread()->Send(Color()->get(BLUE, BG_WHITE) + "Docking at " + outpostname + "..." + Color()->blackout() + endr);
    
    //  Text lastvisitor = outpost->GetLastVisitor();
    //Timestamp lastvisit = outpost->GetLastVisit();
    
    if(!PQgetisnull(dbresult,0,8))
    {
//	get_thread()->Send(Color()->get(BROWN) + "Last visited by " + Color()->get(LIGHTCYAN) + lastvisitor.GetAsString() + Color()->get(BROWN) + " on " + Color()->get(WHITE) + lastvisit.GetAsString() + endr);
    
	std::string visited;
	
	int days = -atoi(PQgetvalue(dbresult,whichoutpost,9));
	int hours = -atoi(PQgetvalue(dbresult,whichoutpost,10));
	int minutes = -atoi(PQgetvalue(dbresult,whichoutpost,11));
	
	visited  = Color()->get(BROWN) + "Last visited by " + Color()->get(LIGHTCYAN) + PQgetvalue(dbresult,0,8) ;
	visited += Color()->get(WHITE) + ' ';
	
	if(days >0) visited += IntToString(days) + " days ";
	else if(hours >0) visited += IntToString(hours) + " hours ";
	else if(minutes >0) visited += IntToString(minutes) + " minutes ";
	else visited += " moments ";
	
	
	visited += Color()->get(BROWN) + "ago." + endr;
	
	
	Send(visited);
	
    }
    else
    {
	get_thread()->Send(Color()->get(YELLOW) + "You are the first visitor to this port! 50 Points." + endr);
	outpost->Lock();
	outpost->SetDiscoverer(player->GetName());
	outpost->Unlock();
	player->Lock();
	player->SetPoints((int)player->GetPoints() + 50);
	player->Unlock();
    }

    outpost->Lock();
    outpost->SetLastVisitor(player->GetName());
    outpost->SetLastVisit(Universe::GetToday(get_thread()->GetDBConn()));
    outpost->Unlock();



    bool deal = false;
    
    int max_holds = ship->GetHolds();
    int ship_plasma = ship->GetPlasma();
    int ship_metals = ship->GetMetals();
    int ship_carbon = ship->GetCarbon();

    int cur_holds = ship_plasma + ship_metals + ship_carbon;
    int empty_holds = max_holds - cur_holds;
    
    bool buyplasma = ((std::string)PQgetvalue(dbresult,whichoutpost,2) == "t") ;
    bool buymetals = ((std::string)PQgetvalue(dbresult,whichoutpost,3) == "t");
    bool buycarbon = ((std::string)PQgetvalue(dbresult,whichoutpost,4) == "t");

    int plasma_price = 50; // TODO: From settings table
    int carbon_price = 20; // TODO: From settings table
    int metals_price = 30; // TODO: From settings table

    int credits = player->GetCredits();

    double buyrate = atof(PQgetvalue(dbresult,0,5));
    double sellrate = atof(PQgetvalue(dbresult,0,6));

    PQclear(dbresult);

    
    get_thread()->Send(Color()->get(WHITE) + IntToString(credits) + Color()->get(CYAN) + " credits" + endr);

    // Plasma trading

//    if(!outpost->Exists())
	

    if(buyplasma)
    {
	if(ship_plasma >0)
	{
	    
	    int sold = BuyGoods(Color()->get(LIGHTPURPLE) + "Plasma", plasma_price * buyrate, ship_plasma);

	    empty_holds += sold;

	    if(sold)
	    {
		deal = true;
		ship->Lock();
		ship->SetPlasma(ship_plasma - sold);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() + (sold * (plasma_price * buyrate)));
		player->Unlock();
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
	    }
	    else
	    {
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thanks anyways." + endr);
	    }

	    
	
	}
    }



    if(buymetals)
    {
	if(ship_metals >0)
	{
	    
	    int sold = BuyGoods(Color()->get(BROWN) + "Metals", metals_price * buyrate, ship_metals);


	    empty_holds += sold;
	    if(sold)
	    {
		deal = true;
		ship->Lock();
		ship->SetMetals(ship_metals - sold);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() + (sold * (metals_price * buyrate)));
		player->Unlock();
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
	    }
	    else
	    {
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thanks anyways." + endr);
	    }

	    
	
	}
    }


    if(buycarbon)
    {
	if(ship_carbon >0)
	{
	    
	    int sold = BuyGoods(Color()->get(CYAN) + "Carbon", carbon_price * buyrate, ship_carbon);

	    empty_holds += sold;

	    if(sold)
	    {
		deal = true;
		ship->Lock();
		ship->SetCarbon(ship_carbon - sold);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() + (sold * (carbon_price * buyrate)));
		player->Unlock();
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
	    }
	    else
	    {
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thanks anyways." + endr);
	    }

	    
	
	}
    }



    if(!buyplasma)
    {
	if(empty_holds > 0)
	{
	    deal = true;
	    int price = (int)((double)plasma_price * sellrate);


	    int bought = SellGoods(Color()->get(LIGHTPURPLE) + "Plasma", plasma_price * sellrate, empty_holds, player->GetCredits());

	    
	    empty_holds -= bought;

	    if(bought)
	    {
		deal = true;
		ship->Lock();
		ship->SetPlasma(ship_plasma + bought);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() - (bought * (plasma_price * sellrate)));
		player->Unlock();
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
		
	    }
	    
		

	}


    }


    if(!buymetals)
    {
	if(empty_holds > 0)
	{
	    deal = true;
	    int price = (int)((double)metals_price * sellrate);


	    int bought = SellGoods(Color()->get(BROWN) + "Metals", metals_price * sellrate, empty_holds, player->GetCredits());

	    empty_holds -= bought;
	    if(bought)
	    {
		deal = true;
		ship->Lock();
		ship->SetMetals(ship->GetMetals() + bought);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() - (bought * (metals_price * sellrate)));
		player->Unlock();
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
		
	    }
	    
		

	}


    }

    if(!buycarbon) // sell carbon
    {
	if(empty_holds > 0)
	{
	    deal = true;
	    int price = (int)((double)carbon_price * sellrate);


	    int bought = SellGoods(Color()->get(CYAN) + "Carbon", carbon_price * sellrate, empty_holds, player->GetCredits());
	    empty_holds -= bought;
	    
	    if(bought)
	    {
		deal = true;
		ship->Lock();
		ship->SetCarbon(ship->GetCarbon() + bought);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() - (bought * (carbon_price * sellrate)));
		player->Unlock();
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
		
	    }
	    
		

	}


    }





    if(!deal)
    {
	get_thread()->Send(Color()->get(GREEN) + "Looks like you have nothing we want, and we have nothing you want. Come back again!" + endr);
    }
    
    

    delete ship;
    delete outpost;
    
    return true;

}

int VoidCommandPort::SellGoods(const std::string &goods, int price_each, int space_available, int credits)
{
    bool done = false;
    bool dealt = false;
    
    int num_units=0;
    
    while(!done && !dealt)
    {

	//Todo, calculate max based on credits if they aren't enough to buy the maximum based on space
	
	get_thread()->Send(Color()->get(BLUE) + "You have " + Color()->get(YELLOW) + IntToString(space_available) +  Color()->get(BLUE) + " available holds." + endr);
	get_thread()->Send(Color()->get(GREEN) + "How many units of " + goods + Color()->get(GREEN)  + " would you like at " + 
			   Color()->get(BLACK,BG_WHITE) + IntToString(price_each) + Color()->get(GREEN) + " each? " +
			   Color()->get(WHITE) + "[" + Color()->get(GREEN) + IntToString(space_available) + Color()->get(WHITE) + "]"
	    );
	
	string units = get_thread()->ReceiveLine();
		
		
	if(units == "" || units.size() == 0)
	    num_units = space_available;
	else num_units = atoi(units.c_str());
	
	if(num_units < 0)
	{
	    Send(Color()->get(LIGHTRED) + "Sorry, we don't buy those. We're too busy selling." + endr);
	    dealt = false;
	    continue;
	}
	
	if(num_units > space_available)
	{
	    get_thread()->Send(Color()->get(LIGHTRED) + "Sorry, you can't hold that many." + endr);
	    dealt= false;
	}
	else if(num_units !=0 )
	{

	    int total = price_each * num_units;
		    
	    get_thread()->Send(Color()->get(GREEN) + "That will be " + Color()->get(BLACK,BG_WHITE) + IntToString(total) + Color()->get(GREEN) + " Y/n");

    
	    string answer = get_thread()->ReceiveLine();

	    LOWERCASE(answer);
		    
	    if(CompStrings(answer,"yes"))
	    {
			
		if(total > credits)
		{
		    get_thread()->Send(Color()->get(LIGHTRED) + "Sorry, you don't have enough credits..." + endr);
		    num_units = 0;
		    dealt = false;
		}
		else
		{
			    
		    dealt = true;
			    
		}
	    }
		
	}
	else dealt = true;
		
		
		
    }
    
    return num_units;

}

