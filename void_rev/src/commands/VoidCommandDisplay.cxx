#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandDisplay.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "OutpostHandle.h"
#include "OutpostTools.h"
#include <math.h>
#include <sstream>
#include <iomanip>

const std::string kQueryShipsStmt{"displayQueryShips"};
const std::string kQueryShipsCloakedStmt{"displayQueryShipsCloaked"};
const std::string kQueryOutpostsStmt{"displayQueryOutposts"};
const std::string kQueryStardocksStmt{"displayQueryStardocks"};
const std::string kQuerySentinelsStmt{"displayQuerySentinels"};
const std::string kQuerySectorStmt{"displayQuerySector"};
const std::string kQueryPlanetsStmt{"displayQueryPlanets"};

VoidCommandDisplay::VoidCommandDisplay(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandDisplay::~VoidCommandDisplay()
{
}

string VoidCommandDisplay::GetCommandString()const
{
    return "Display";
}

string VoidCommandDisplay::GetDescription()const
{
    return "Show the current sector and it's contents";
}

string VoidCommandDisplay::GetSyntax()const
{
    return "display";
}

    
bool VoidCommandDisplay::ClaimCommand(const string &command)
{
    if (CompStrings(command,"display")) return true;

    return false;
}

bool VoidCommandDisplay::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    int sector=0;
    bool show_cloaked = false;

    if(arguments.size() && bFromPost)
    {
	std::vector<std::string> words = WordsFromString( arguments );
	
	if(words.size())
	{
	    std::string sector_str = words[0];

	    sector = atoi(sector_str.c_str());
	    
	    if( words.size() > 1)
	    {
		show_cloaked = words[1] == "cloaked" ?true:false;
	    }
	    
	}
    }
    else
    {
	ShipHandlePtr ship=  create_handle_to_current_ship(get_thread()->GetPlayer());
	sector = ship->GetSector();
    }
    bool stop = false;
    get_thread()->Send(DisplaySector( sector, show_cloaked, stop ));

    return true;
}
    
std::string VoidCommandDisplay::DisplaySector(Sector sector, bool show_cloaked, bool& should_stop)
{
    std::vector<Sector> sectors = Universe::GetAdjacentSectors(sector);
    std::ostringstream os;

    std::string query = "select sname from territory,sectors where sectors.kterritory = territory.nkey and sectors.nsector = '" + IntToString(sector) + "';";

    pqxx::result dbresult = get_thread()->DBExec(query);

    os << Color()->get(LIGHTGREEN) << endr <<  "Sector " << Color()->get(WHITE) << sector;
    if(dbresult.size() > 0 && !dbresult[0][0].is_null())
    {
      os << Color()->get(GRAY) << ' ' <<  dbresult[0][0].as<std::string>("");
    }

    os << endr;
    size_t stardocks, outposts, planets, ships, sentinels = 0;
    os << DisplayStardockInSector(sector, stardocks);
    os << DisplayOutpostsInSector(sector, outposts);
    os << DisplayPlanetsInSector(sector, planets);
    os << DisplayShipsInSector(sector, show_cloaked, ships);
    os << DisplaySentinelsInSector(sector, sentinels);

    os << Color()->get(GRAY) << "Adjacent Sectors: ";


    for(auto i=sectors.begin();i != sectors.end(); ++i)
    {
      int flags = get_player()->GetSectorFlags(*i);
      FGColor sector_color = WHITE;
      if(!(flags & static_cast<int>(eSectorFlags::VISITED))){
	sector_color = DARKGRAY; // Maybe brown?
      }
      if(flags & static_cast<int>(eSectorFlags::FAVORITE)){
	sector_color = YELLOW;
      }
      if(flags & static_cast<int>(eSectorFlags::AVOID)){
	sector_color = LIGHTRED;
      }
      
	
      os << Color()->get(RED) << '[' << Color()->get(sector_color) << *i << Color()->get(RED) << "] ";
    }

    if(planets > 0 || outposts > 0 || sentinels > 0 || stardocks > 0 || ships > 0) {
      should_stop = true;
    } else {
      should_stop = false;
    }

    return os.str();   

}


std::string VoidCommandDisplay::DisplayStardockInSector(Sector sector, size_t& count)
{
    std::ostringstream os;
    get_thread()->EnsurePreparedStatement(kQueryStardocksStmt, "select bstardock,sstardockname from sectors where nsector = $1;");
    pqxx::read_transaction work{*get_thread()->GetDatabaseConn()};
    pqxx::result dbresult = work.prepared(kQueryStardocksStmt)((int)sector).exec();
    work.commit();
    Boolean has_stardock("bstardock", dbresult[0][0].as<std::string>("f"), dbresult[0][0].is_null());
    count = 0;
    if((bool)has_stardock)
    {
      count = 1;
      Text stardock("sstardockname",dbresult[0][1].as<std::string>("f"), dbresult[0][1].is_null());
      os << Color()->get(GREEN) << "Stardock: " << Color()->get(WHITE, BG_BLUE) << (std::string)stardock << endr;
    }

    return os.str();
}
 


std::string VoidCommandDisplay::DisplayShipsInSector(Sector sector, bool show_cloaked, size_t& count)
{
    std::ostringstream os;
    pqxx::read_transaction work{*get_thread()->GetDatabaseConn()};
    pqxx::result dbresult ;
    if(show_cloaked) {
       get_thread()->EnsurePreparedStatement(kQueryShipsCloakedStmt, "select s.nkey,s.sname, tm.sname, t.sname, s.nmissiles, t.nforecolor, t.nbackcolor, s.bcloaked, s.kowner, p.kmob, s.kalliance, s.nshields, t.nmaxshields from ship s,shiptype t, player p, shipmanufacturer tm where s.ksector = $1 and s.ktype = t.nkey and tm.nkey = t.kmanufacturer and (p.sname = s.kowner) order by s.nkey;");
       dbresult = work.prepared(kQueryShipsCloakedStmt)((int)sector).exec();
    }else{
      get_thread()->EnsurePreparedStatement(kQueryShipsStmt, "select s.nkey,s.sname, tm.sname, t.sname, s.nmissiles, t.nforecolor, t.nbackcolor, s.bcloaked, s.kowner, p.kmob, s.kalliance, s.nshields, t.nmaxshields from ship s,shiptype t, player p, shipmanufacturer tm where s.ksector = $1 and s.ktype = t.nkey and tm.nkey = t.kmanufacturer and (s.bcloaked = false or s.bcloaked is null) and (p.sname = s.kowner) order by s.nkey;");
      dbresult = work.prepared(kQueryShipsStmt)((int)sector).exec();
    }
    work.commit();

    size_t numships = dbresult.size();
    count = numships;
    
    if(numships)
    {
   
	os << Color()->get(GREEN) << "Ships:" << endr;
    }

    for(auto row : dbresult)
    {
	
      int ship = row[0].as<int>();

      int shields = row[11].as<int>(0);
      int maxshields = row[12].as<int>(1);
      double percent =(double) shields / (double)maxshields;
 
      int pc = truncf(percent * 10) * 10;

	

	os << '\t' << Color()->get(GREEN) << '[' << Color()->get(WHITE) << ship << Color()->get(GREEN) << ']';
	os << ' ' << Color()->get(LIGHTBLUE) << row[1].as<std::string>("") << ' ';
	os << Color()->get((FGColor)row[5].as<int>(0),(BGColor)row[6].as<int>(0)) << row[2].as<std::string>() <<' ' <<  row[3].as<std::string>() << Color()->get(GREEN);
	os << " w/" << Color()->get(WHITE) << row[4].as<int>(0) << Color()->get(GREEN) << " missiles ";
	os << Color()->get(LIGHTPURPLE) << '(' << Color()->get(GRAY) << pc << '%' << Color()->get(LIGHTPURPLE) << ')' << Color()->get(GREEN) + " shields " <<  endr;

	PlayerHandlePtr player = create_handle_to_player_in_ship(ship);
// Need to check for null here, there may not be a player in the ship.
	

	if(player)
	{
	    os << "\t\t" << Color()->get(LIGHTGREEN) << "piloted by " ;
	
	}
	else
	{
	    os << "\t\t" << Color()->get(GREEN) << "owned by " ;
	}
/* 
	    if((bool)player->GetIsMob())
	    {
		os << Color()->get(LIGHTPURPLE);
	    }
	    else 
	    {
		os << Color()->get(LIGHTCYAN);
	    }
	    
	    os << (std::string)player->GetName();
	    
	    Text alliance = player->GetAlliance();
	    
	    if(!alliance.IsNull())
	    {
		os << Color()->get(GRAY) << " {" << Color()->get(RED) <<  alliance.GetAsString() << Color()->get(GRAY) << '}';
	    }
	    
	    os << endr;
*/


	if(!row[9].is_null())
	{
	    os << Color()->get(LIGHTCYAN);
	}
	else
	{
	    // Owned by a mob
	    os << Color()->get(LIGHTPURPLE);
	}
	
	os << row[8].as<std::string>("");
	
	if(!row[10].is_null())
	{
	  os << Color()->get(GRAY) << " {" << Color()->get(RED) <<  row[10].as<std::string>() << Color()->get(GRAY) << '}';
	}
	os << endr;
	
	    
    }

       
    os << endr;

    return os.str();

}

std::string VoidCommandDisplay::DisplayOutpostsInSector(Sector sector, size_t& count)
{
  pqxx::read_transaction work{*get_thread()->GetDatabaseConn()};
  get_thread()->EnsurePreparedStatement(kQueryOutpostsStmt, "select sname, bbuyplasma, bbuymetals, bbuycarbon, fplasmaprice, fmetalsprice,fcarbonprice, floor((round(date_part('epoch',now())) - round(date_part('epoch',dlastvisit))) / 60) from Outpost where ksector = $1;");

  pqxx::result dbresult = work.prepared(kQueryOutpostsStmt)((int)sector).exec();
  std::ostringstream os;
  work.commit();

  size_t numoutposts = dbresult.size();
  count = numoutposts;

  if(numoutposts == 0 ) return "";

    os << Color()->get(GREEN) << "Outposts:" << endr;

    for(auto row : dbresult)
    {
      os << '\t' << Color()->get(LIGHTPURPLE) << row[0].as<std::string>("Unknown");
      os << Color()->get(WHITE) << " (";

      int minutes = row[7].as<int>(0);

      Boolean buyplasma("bbuyplasma", row[1].as<std::string>("f"), row[1].is_null());
      Boolean buymetals("bbuymetals", row[2].as<std::string>("f"), row[2].is_null());
      Boolean buycarbon("bbuycarbon", row[3].as<std::string>("f"), row[3].is_null());

      const double plasmapricemult = row[4].as<double>(1.0);
      const double metalspricemult = row[5].as<double>(1.0);
      const double carbonpricemult = row[6].as<double>(1.0);

      const int base_plasma_price = std::stoi(ResourceMaster::GetInstance()->GetConfig("base_plasma_price"));
      const int base_metals_price = std::stoi(ResourceMaster::GetInstance()->GetConfig("base_metals_price"));
      const int base_carbon_price = std::stoi(ResourceMaster::GetInstance()->GetConfig("base_carbon_price"));
      const double plasmaprice = double(base_plasma_price) * plasmapricemult;
      const double metalsprice = double(base_metals_price) * metalspricemult;
      const double carbonprice = double(base_carbon_price) * carbonpricemult;
      

      if(buyplasma.GetValue()) {
	  os << Color()->get(LIGHTBLUE) << 'B' ;
	  os << (int) round( OutpostTools::GetBuyRateAfterTime(minutes,plasmaprice));
	  os << ',';
      } else {
	os << Color()->get(LIGHTCYAN) << 'S' ;
	os << (int) round(OutpostTools::GetSellRateAfterTime(minutes,plasmaprice));
	os << ',';
      }
      
      
      if(buymetals.GetValue()) {
	os << Color()->get(LIGHTBLUE) << 'B';
	os << (int) round(OutpostTools::GetBuyRateAfterTime(minutes,metalsprice));
	os << ',';
      } else {
	os << Color()->get(LIGHTCYAN) << 'S' ;
	os << (int) round(OutpostTools::GetSellRateAfterTime(minutes,metalsprice));
	os << ',';
      }


      if(buycarbon.GetValue()) {
	os << Color()->get(LIGHTBLUE) << 'B';
	os << (int) round(OutpostTools::GetBuyRateAfterTime(minutes,carbonprice));
      } else {
	os << Color()->get(LIGHTCYAN) << 'S';
	os << (int) round(OutpostTools::GetSellRateAfterTime(minutes,carbonprice));
      }
      
      os << Color()->get(WHITE) << ')' << endr;
      
    }

    return os.str();
}

std::string VoidCommandDisplay::DisplayPlanetsInSector(Sector sector, size_t& count)
{
  pqxx::read_transaction work{*get_thread()->GetDatabaseConn()};
  get_thread()->EnsurePreparedStatement(kQueryPlanetsStmt, "select sname, fsize, kclass, kowner, kalliance, dlastvisit from planet where ksector = $1;");

  pqxx::result dbresult = work.prepared(kQueryPlanetsStmt)((int)sector).exec();
  std::ostringstream os;
  work.commit();

  size_t numplanets = dbresult.size();
  count = numplanets;   
  
  if(numplanets == 0 ) return "";

  os << Color()->get(GREEN) << "Planets:" << endr;

  for(auto row : dbresult) {
    os << '\t' << Color()->get(LIGHTBLUE) << row[0].as<std::string>("Brigadoon");
    os << Color()->get(WHITE) << " (" << Color()->get(GRAY) << std::setprecision(3) << row[1].as<double>(1.0) << 'x' << Color()->get(WHITE) << ") " << Color()->get(PURPLE) << row[2].as<std::string>() <<  " class " << endr;
    if(!row[3].is_null()){
      os << "\t\t" << Color()->get(GREEN) << "owned by "
	 << Color()->get(LIGHTCYAN) << row[3].as<std::string>();
      
      if(row[4].is_null()){
	os << endr;
      }	else {
	os << Color()->get(WHITE) << " {" << Color()->get(RED) << row[4].as<std::string>() << 
	  Color()->get(WHITE) << "} " << endr;
      }
    }
    
  }

  return os.str();
}

std::string VoidCommandDisplay::DisplaySentinelsInSector(Sector sector, size_t& total_count)
{
  pqxx::read_transaction work{*get_thread()->GetDatabaseConn()};
  get_thread()->EnsurePreparedStatement(kQuerySentinelsStmt, "select ncount, kplayer, player.kalliance from sentinels, player where sentinels.kplayer = player.sname and sentinels.ksector = $1;");

  pqxx::result dbresult = work.prepared(kQuerySentinelsStmt)((int)sector).exec();
  work.commit();

    int groups = dbresult.size();
    total_count = 0;

    if(groups == 0) return "";

    std::ostringstream os;

    os << Color()->get(GREEN) << "Sentinels:" << endr;

    for(auto row : dbresult)
    {
      size_t count  = row[0].as<int>(0);
      total_count += count;
      std::string player = row[1].as<std::string>("");
      
      os << '\t' << Color()->get(WHITE) << count << Color()->get(BROWN) << " sentinel(s) owned by " <<
	Color()->get(LIGHTCYAN) << player;
      
      if(row[2].is_null()){
	os << endr;
      }	else {
	os << Color()->get(WHITE) << " {" << Color()->get(RED) << row[2].as<std::string>() << 
	  Color()->get(WHITE) << "} " << endr;
      }

	     
    }

    return os.str();

}
