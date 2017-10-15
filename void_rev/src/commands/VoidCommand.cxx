#include "VoidCommand.h"
#include <vector>
#include <algorithm>
#include <string>
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "VoidServerThread.h"
#include "void_util.h"
#include "ShipTypeHandle.h"
#include <memory>
#include "ResourceMaster.h"

using std::string;

const char *  VoidCommand::endr = "\n\r";


std::shared_ptr<ColorType>  VoidCommand::Color()const
{
  return get_thread()->Color();
}

VoidCommand::VoidCommand(VoidServerThread *pthread):m_pthread(pthread)
{
}

VoidCommand::~VoidCommand()
{
}


bool VoidCommand::isValidPlayer(const std::string& playername)const
{
  // TODO: This needs to be either a prepared statement or execParams to make safe...
  std::string query = "select count(0) from player where sname = '" + playername + "';";

  pqxx::result dbresult = get_thread()->DBExec(query);

  if(dbresult.size() != 1)
    {
      return false;
    }

  if(dbresult[0][0].as<int>() != 1)
    {
      return false;
    }


  return true;
}

void VoidCommand::Send(const std::string &str) const
{
  get_thread()->Send(str);
}

std::string VoidCommand::ReceiveLine() const
{
  return get_thread()->ReceiveLine();
}

void VoidCommand::log(LOG_SEVERITY severity, const std::string& message) const
{
  ResourceMaster::GetInstance()->Log(severity,message);
}

PlayerHandlePtr VoidCommand::get_player() const
{
  return get_thread()->GetPlayer();
}


bool VoidCommand::move_player_to_sector(Sector sector)
{

  ShipHandlePtr pship = create_handle_to_current_ship(get_player());
  PlayerHandlePtr player = get_thread()->GetPlayer();

    

  // TODO: Broadcast this data to players in the sector ("Player blah blah just left the sector to sector blah")

  // TODO: Mines, trackers, sentinels, enemy ships attacking.
  if(pship == NULL)
    {
      // Problem. no ship. 
    }
  else
    {

      int cur_turns = (int)player->GetTurnsLeft();
      ShipTypeHandlePtr shiptype= pship->GetShipTypeHandle();
      int tps = (int)shiptype->GetTurnsPerSector();

      player->AddSectorFlag(eSectorFlags::VISITED, sector);

      Integer tow = pship->GetTow();

      if(!tow.IsNull()) {
	std::shared_ptr<Integer> towship = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY),tow.GetAsString());
	PrimaryKey key(towship);

	ShipHandle towshiphandle(get_thread()->GetDatabaseConn(),key);
	ShipTypeHandlePtr towshiptype = towshiphandle.GetShipTypeHandle();

	tps += (int)towshiptype->GetTurnsPerSector();

	if(tps <= cur_turns)
	  {

	    towshiphandle.Lock();
	    towshiphandle.SetSector(sector);
	    towshiphandle.Unlock();
	    Send(Color()->get(LIGHTBLUE) + towshiphandle.GetName().GetAsString() + Color()->get(GREEN) + " enters the sector in tow." + endr);
	
	  }
	
      }

      if(tps <= cur_turns)
	{

	  player->Lock();
	  player->SetTurnsLeft( cur_turns - tps);
	  player->Unlock();
	  pship->Lock();
	  pship->SetSector(sector);
	  pship->Unlock();

	    
	    
	    
	  get_thread()->PostCommand("display", "");
	}
      else
	{
	  Send(Color()->get(LIGHTRED) + "You don't have enough turns." + endr);
	  return false;
	}
	
      //TODO: Determine if there are interesting things here and provide option to stop
    }

  return true;
}



PlayerHandlePtr VoidCommand::create_handle_to_player_in_ship(int ship) const
{
  std::string query = "SELECT sname from Player where kcurrentship = " + IntToString(ship) + ";";

  pqxx::result dbresult = get_thread()->DBExec(query);

  if(dbresult.size() > 1)
    {
      DBException e("Multiple players in same ship!!");
      throw e;
    }

  if(dbresult.size() < 1)
    {
      return NULL;
    }

  std::string name = dbresult[0][0].as<std::string>();

  std::shared_ptr<Text> playername = std::make_shared<Text>(PlayerHandle::FieldName(PlayerHandle::NAME), name);
  PrimaryKey key(playername);

  PlayerHandlePtr player = std::make_shared<PlayerHandle>(get_thread()->GetDatabaseConn(),key, false);

  return player;

}

VoidServerThread * VoidCommand::get_thread()const
{
  return m_pthread;
}

ShipHandlePtr VoidCommand::create_handle_to_current_ship(PlayerHandlePtr player) const
{
  std::shared_ptr<Integer> shipnum = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY), player->GetCurrentShip().GetAsString());
  PrimaryKey key(shipnum);

  ShipHandlePtr ship = std::make_shared<ShipHandle>(get_thread()->GetDatabaseConn(), key, false);

  return ship;
}
std::string VoidCommand::get_config(const std::string &key) const
{
  ResourceMaster * RM = ResourceMaster::GetInstance();

  return RM->GetConfig(key);
}


