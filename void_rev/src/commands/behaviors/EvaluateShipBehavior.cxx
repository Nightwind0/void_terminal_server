#include "EvaluateShipBehavior.h"
#include <vector>
#include <sstream>
#include <deque>
#include "Universe.h"
#include "void_util.h"
#include <math.h>
#include "ResourceMaster.h"
#include "SocketException.h"
#include <algorithm>
#include "VoidServerThread.h"

EvaluateShipBehavior::EvaluateShipBehavior(VoidServerThread *thread):Behavior(thread)
{
   
}

EvaluateShipBehavior::~EvaluateShipBehavior()
{
}

int EvaluateShipBehavior::EvaluateShip(int shipnum)
{
  ResourceMaster * RM = ResourceMaster::GetInstance();
  std::string sql = "select nmissiles, nshields, nholds - ninitholds, nsentinels, nmines, ncost from ship,shiptype where ship.ktype = shiptype.nkey and ship.nkey = '" + IntToString(shipnum) + "';";

  pqxx::result dbresult = get_behavior_thread()->DBExec(sql);

  if(dbresult.size() != 1) {
    DBException e("Evaluate ship didnt return a row.");
    throw e;
  }


  int cost = dbresult[0][5].as<int>();
  int value = cost /2;

  int missiles = dbresult[0][0].as<int>();
  int shields = dbresult[0][1].as<int>();
  int holds = dbresult[0][2].as<int>();
  int sentinels = dbresult[0][3].as<int>();
  int mines = dbresult[0][4].as<int>();
  
  value += missiles * config_int(RM, "missile_price");
  value += shields * config_int(RM, "shield_price");
  value += holds * config_int(RM, "hold_price");
  value += sentinels * config_int(RM, "sentinel_price");
  value += mines * config_int(RM, "mine_price");

  return value;
}
