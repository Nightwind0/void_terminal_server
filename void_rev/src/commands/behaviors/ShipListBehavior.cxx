#include "ShipListBehavior.h"
#include <list>
#include <sstream>

using std::ostringstream;

using std::left;
using std::right;


ShipListBehavior::ShipListBehavior(VoidServerThread *thread):Behavior(thread)
{
}

ShipListBehavior::~ShipListBehavior()
{
}

std::list<int> ShipListBehavior::GetOwnedShips()
{
    PlayerHandlePtr player = get_behavior_thread()->GetPlayer();

    std::string query = "select nkey from ship where kowner = '" + (std::string)player->GetName() + "';";

    pqxx::result dbresult = get_behavior_thread()->DBExec(query);
    int numships = dbresult.size();

    std::list<int> ships;

    for(auto row : dbresult)
    {
      ships.push_back(row[0].as<int>());
    }

    return ships;
}

void ShipListBehavior::ShowShipList(const std::list<int> &ships)
{
 
   
    ostringstream os;
    os << get_behavior_thread()->Color()->get(BROWN,BG_WHITE)
       << "                    Ship List                    " << get_behavior_thread()->Color()->blackout() << endr;
    os << get_behavior_thread()->Color()->get(LIGHTGREEN);
    os.fill(' ');
    os.width(7);
    os << left << "Ship#";
    os.width(25);
    os << left << "Ship";
    os.width(25);
    os << left << "Ship Type";
    os.width(6);
    os << left << "Sec.";
    os.width(5);
    os << right << "Cloak";
    os << endr;

   

    for(std::list<int>::const_iterator iter = ships.begin();
	iter != ships.end();
	iter++)
    {

      std::shared_ptr<Integer> nkey = std::make_shared<Integer>(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(*iter));
	PrimaryKey key(nkey);
	ShipHandle shiph(get_behavior_thread()->GetDatabaseConn(), key);

	ShipTypeHandlePtr shiptype = shiph.GetShipTypeHandle();

	int sector = (int)shiph.GetSector();
	
	char cloaked = shiph.GetIsCloaked()?'T':'F';
	
	FGColor fgcolor = (FGColor)((int)shiptype->GetForeColor());
	BGColor bgcolor = (BGColor)((int)shiptype->GetBackColor());
	

	os << get_behavior_thread()->Color()->get(WHITE);
	os.width(7);
	os << left <<  *iter;
	os << get_behavior_thread()->Color()->get(LIGHTBLUE);
	os.width(25);
	os << left << (std::string)shiph.GetName();
	os << get_behavior_thread()->Color()->get(fgcolor,bgcolor);
	os.width(25 + get_behavior_thread()->Color()->get(fgcolor,bgcolor).size());
	os << left << (std::string)shiptype->GetShipTypeName(get_behavior_thread()->Color());
	os << get_behavior_thread()->Color()->get(WHITE,BG_BLACK);
	os.width(6);
	os << right << sector;
	os.width(5);
	os << right << cloaked;
	os << endr;	
    }
    os << endr;

    get_behavior_thread()->Send(os.str());

}
