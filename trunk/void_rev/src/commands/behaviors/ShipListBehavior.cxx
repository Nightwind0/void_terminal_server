#include "ShipListBehavior.h"
#include <list>
#include <sstream>

using std::ostringstream;

using std::left;
using std::right;

const char * endr = "\n\r";


ShipListBehavior::ShipListBehavior(VoidServerThread *thread):m_thread(thread)
{
}

ShipListBehavior::~ShipListBehavior()
{
}

std::list<int> ShipListBehavior::GetOwnedShips()
{
    PlayerHandle * player = m_thread->GetPlayer();

    std::string query = "select nkey from ship where kowner = '" + (std::string)player->GetName() + "';";

    PGresult *dbresult = m_thread->DBExec(query);

    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {
	
	DBException e(std::string("Get Owned Ships: ") +  PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	throw e;
    }

    int numships = PQntuples(dbresult);

    std::list<int> ships;

    for(int i=0;i< numships; i++)
    {
	ships.push_back( atoi(PQgetvalue(dbresult,i,0)));
    }

    PQclear(dbresult);

    return ships;
}

void ShipListBehavior::ShowShipList(const std::list<int> &ships)
{
 
   
    ostringstream os;
    os << m_thread->Color()->get(BROWN,BG_WHITE) << "                    Ship List                    " << m_thread->Color()->blackout() << endr;
    os << m_thread->Color()->get(LIGHTGREEN);
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

	Integer nkey(ShipHandle::FieldName(ShipHandle::NKEY), IntToString(*iter));
	PrimaryKey key(&nkey);
	ShipHandle shiph(m_thread->GetDBConn(), key);

	ShipTypeHandle shiptype = shiph.GetShipTypeHandle();

	int sector = (int)shiph.GetSector();
	
	char cloaked = shiph.GetIsCloaked()?'T':'F';
	
	FGColor fgcolor = (FGColor)((int)shiptype.GetForeColor());
	BGColor bgcolor = (BGColor)((int)shiptype.GetBackColor());
	

	os << m_thread->Color()->get(WHITE);
	os.width(7);
	os << left <<  *iter;
	os << m_thread->Color()->get(LIGHTBLUE);
	os.width(25);
	os << left << (std::string)shiph.GetName();
	os << m_thread->Color()->get(fgcolor,bgcolor);
	os.width(25 + m_thread->Color()->get(fgcolor,bgcolor).size());
	os << left << (std::string)shiptype.GetShipTypeName(m_thread->Color());
	os << m_thread->Color()->get(WHITE,BG_BLACK);
	os.width(6);
	os << right << sector;
	os.width(5);
	os << right << cloaked;
	os << endr;	
    }
    os << endr;

    m_thread->Send(os.str());

}
