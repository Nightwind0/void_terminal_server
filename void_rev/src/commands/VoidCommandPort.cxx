
#include "VoidCommandPort.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include <string.h>
#include <math.h>

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
    ResourceMaster * RM = ResourceMaster::GetInstance();

    PlayerHandlePtr player = get_player();
    ShipHandlePtr ship = create_handle_to_current_ship(player);

    if(ship->GetIsCloaked())
    {
	Send(Color()->get(RED) + "You cannot port with a cloaked ship." + endr );
	return true;
    }


    int cur_turns = (int)player->GetTurnsLeft();

    if(cur_turns <= 0)
    {
	get_thread()->Send(Color()->get(RED)+ "Sorry, you have no turns left." + endr);
	return true;
    }

    std::string query = "select sname, bspecial, bbuyplasma, bbuymetals, bbuycarbon, kdiscoverer, klastvisitor, extract (year from age(dlastvisit,now())), extract (month from age(dlastvisit,now())), extract (day from age(dlastvisit,now())), extract (hour from age(dlastvisit,now())), extract(minute from age(dlastvisit,now())), floor((round(date_part('epoch',now())) - round(date_part('epoch',dlastvisit))) / 60), fplasmaprice, fmetalsprice, fcarbonprice from outpost where ksector = ";

    query += ship->GetSector().GetAsString();

    query += ";";


    pqxx::result dbresult = get_thread()->DBExec(query);


    int numoutposts = dbresult.size();
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
	  get_thread()->Send(Color()->get(WHITE)+ IntToString(i+1) + ")" + Color()->get(LIGHTPURPLE) + dbresult[i][0].as<std::string>("") + endr);
	}

	std::string selection = get_thread()->ReceiveLine();

	whichoutpost = atoi(selection.c_str()) -1;
    }
    
    std::string outpostname = dbresult[whichoutpost][0].as<std::string>("");


    std::shared_ptr<Text> outpostt = std::make_shared<Text>(OutpostHandle::FieldName(OutpostHandle::NAME), outpostname);
    PrimaryKey key(outpostt);
    
    
    OutpostHandlePtr outpost = std::make_shared<OutpostHandle>(get_thread()->GetDatabaseConn(),key,false);
    

    get_thread()->Send(Color()->get(BLUE, BG_WHITE) + "Docking at " + outpostname + "..." + Color()->blackout() + endr);

    player->Lock();
    player->SetTurnsLeft( cur_turns - 1);
    player->Unlock();

    auto outpostrow = dbresult[whichoutpost];

    const bool buyplasma = outpostrow[2].as<std::string>("f") == "t";
    const bool buymetals = outpostrow[3].as<std::string>("f") == "t";
    const bool buycarbon = outpostrow[4].as<std::string>("f") == "t";

    const double plasma_price_mult = outpostrow[13].as<double>(1.0);
    const double metals_price_mult = outpostrow[14].as<double>(1.0);
    const double carbon_price_mult = outpostrow[15].as<double>(1.0);

    const int base_plasma_price = std::stoi(ResourceMaster::GetInstance()->GetConfig("base_plasma_price"));
    const int base_metals_price = std::stoi(ResourceMaster::GetInstance()->GetConfig("base_metals_price"));
    const int base_carbon_price = std::stoi(ResourceMaster::GetInstance()->GetConfig("base_carbon_price"));

    double plasma_price = plasma_price_mult * double(base_plasma_price);
    double metals_price = metals_price_mult * double(base_metals_price);
    double carbon_price = carbon_price_mult * double(base_carbon_price);


    if(!outpostrow[6].is_null())
    {
    
	std::string visited;
	
	int years = - outpostrow[7].as<int>(0);
	int months = -outpostrow[8].as<int>(0);
	int days = -outpostrow[9].as<int>(0);
	int hours = -outpostrow[10].as<int>(0);
	int minutes = -outpostrow[11].as<int>(0);
	int minutes_delta = outpostrow[12].as<int>(0);
	
	visited  = Color()->get(BROWN) + "Last visited by " + Color()->get(LIGHTCYAN) + outpostrow[6].as<std::string>();
	visited += Color()->get(WHITE) + ' ';
	
	if(years >0) visited += IntToString(years) + " years ";
	else if(months >0) visited += IntToString(months) + " months ";
	else if(days >0) visited += IntToString(days) + " days ";
	else if(hours >0) visited += IntToString(hours) + " hours ";
	else if(minutes >0) visited += IntToString(minutes) + " minutes ";
	else visited += " moments ";
	
	
	visited += Color()->get(BROWN) + "ago." + endr;

	
	// TODO: Adjust for minutes_delta gone by for each of plasma, metals and carbon
	if(buyplasma)
	    plasma_price = OutpostHandle::GetBuyRateAfterTime(minutes_delta, plasma_price);
	else plasma_price = OutpostHandle::GetSellRateAfterTime(minutes_delta,plasma_price);

	if(buymetals)
	    metals_price = OutpostHandle::GetBuyRateAfterTime(minutes_delta, metals_price );
	else metals_price = OutpostHandle::GetSellRateAfterTime(minutes_delta, metals_price );

	if(buycarbon)
	    carbon_price = OutpostHandle::GetBuyRateAfterTime(minutes_delta, carbon_price );
	else carbon_price = OutpostHandle::GetSellRateAfterTime(minutes_delta, carbon_price );

	outpost->Lock();
	outpost->SetPlasmaPriceMultiplier(plasma_price / double(base_plasma_price));
	outpost->SetMetalsPriceMultiplier(metals_price / double(base_metals_price));
	outpost->SetCarbonPriceMultiplier(carbon_price / double(base_carbon_price));
	outpost->Unlock();

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
    outpost->SetLastVisit(Universe::GetToday(*get_thread()->GetDatabaseConn()));
    outpost->Unlock();


    int nplasma_price = round(plasma_price);
    int nmetals_price = round(metals_price);
    int ncarbon_price = round(carbon_price);


    bool deal = false;
    
    int max_holds = ship->GetHolds();
    int ship_plasma = ship->GetPlasma();
    int ship_metals = ship->GetMetals();
    int ship_carbon = ship->GetCarbon();

    int cur_holds = ship_plasma + ship_metals + ship_carbon;
    int empty_holds = max_holds - cur_holds;
    
    int credits = player->GetCredits();

    
    get_thread()->Send(Color()->get(WHITE) + IntToString(credits) + Color()->get(CYAN) + " credits" + endr);

    if(buyplasma)
    {
	if(ship_plasma >0)
	{
	    
	    int sold = BuyGoods(Color()->get(LIGHTPURPLE) + "Plasma", nplasma_price , ship_plasma);

	    empty_holds += sold;

	    if(sold)
	    {
		deal = true;
		ship->Lock();
		ship->SetPlasma(ship_plasma - sold);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() + (sold * nplasma_price ));
		player->Unlock();

		plasma_price = OutpostHandle::GetBuyRateAfterPurchase( sold, plasma_price );
		outpost->Lock();
		outpost->SetPlasmaPriceMultiplier( plasma_price / double(base_plasma_price));
		outpost->Unlock();

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
	    
	    int sold = BuyGoods(Color()->get(BROWN) + "Metals", nmetals_price , ship_metals);


	    empty_holds += sold;
	    if(sold)
	    {
		deal = true;
		ship->Lock();
		ship->SetMetals(ship_metals - sold);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() + (sold * (nmetals_price )));
		player->Unlock();

		metals_price = OutpostHandle::GetBuyRateAfterPurchase( sold, metals_price );
		outpost->Lock();
		outpost->SetMetalsPriceMultiplier( metals_price / double(base_metals_price) );
		outpost->Unlock();

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
	    
	    int sold = BuyGoods(Color()->get(CYAN) + "Carbon", ncarbon_price , ship_carbon);

	    empty_holds += sold;

	    if(sold)
	    {
		deal = true;
		ship->Lock();
		ship->SetCarbon(ship_carbon - sold);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() + (sold * (ncarbon_price )));
		player->Unlock();

		carbon_price = OutpostHandle::GetBuyRateAfterPurchase( sold, carbon_price );
		outpost->Lock();
		outpost->SetCarbonPriceMultiplier( carbon_price / double(base_carbon_price) );
		outpost->Unlock();

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
	    
	    int bought = SellGoods(Color()->get(LIGHTPURPLE) + "Plasma", nplasma_price , empty_holds, player->GetCredits());

	    
	    empty_holds -= bought;

	    if(bought)
	    {
		deal = true;
		ship->Lock();
		ship->SetPlasma(ship_plasma + bought);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() - (bought * (nplasma_price )));
		player->Unlock();


		plasma_price = OutpostHandle::GetSellRateAfterSale ( bought, plasma_price );
		outpost->Lock();
		outpost->SetPlasmaPriceMultiplier( plasma_price / double(base_plasma_price) );
		outpost->Unlock();

		
		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
		
	    }
	}


    }


    if(!buymetals)
    {
	if(empty_holds > 0)
	{
	    deal = true;
	    
	    int bought = SellGoods(Color()->get(BROWN) + "Metals", nmetals_price , empty_holds, player->GetCredits());

	    empty_holds -= bought;
	    if(bought)
	    {
		deal = true;
		ship->Lock();
		ship->SetMetals(ship->GetMetals() + bought);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() - (bought * (nmetals_price )));
		player->Unlock();

		metals_price = OutpostHandle::GetSellRateAfterSale ( bought, metals_price );
		outpost->Lock();
		outpost->SetMetalsPriceMultiplier( metals_price / double(base_metals_price) );
		outpost->Unlock();

		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
		
	    }
	    
		

	}


    }

    if(!buycarbon) // sell carbon
    {
	if(empty_holds > 0)
	{
	    deal = true;

	    int bought = SellGoods(Color()->get(CYAN) + "Carbon", ncarbon_price , empty_holds, player->GetCredits());
	    empty_holds -= bought;
	    
	    if(bought)
	    {
		deal = true;
		ship->Lock();
		ship->SetCarbon(ship->GetCarbon() + bought);
		ship->Unlock();

		player->Lock();
		player->SetCredits(player->GetCredits() - (bought * (ncarbon_price )));
		player->Unlock();

		carbon_price = OutpostHandle::GetSellRateAfterSale ( bought, carbon_price );
		outpost->Lock();
		outpost->SetCarbonPriceMultiplier( carbon_price / double(base_carbon_price) );
		outpost->Unlock();

		get_thread()->Send(Color()->get(LIGHTGREEN) + "Thank you!" + endr);
		
	    }
	    
		

	}


    }





    if(!deal)
    {
	get_thread()->Send(Color()->get(GREEN) + "Looks like you have nothing we want, and we have nothing you want. Come back again!" + endr);
    }
    
        
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

