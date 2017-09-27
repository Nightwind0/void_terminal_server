#include "Event.h"
#include <string>

using std::string;


Event::Event(EventType type): m_type(type)
{
    m_shiptype = 0;
    m_shipname = "";
}

Event::~Event()
{
}

Event::EventType Event::GetType()const
{
    return m_type;
}
    
void Event::SetActor(const std::string &player)
{
    m_actor = player;
}
std::string Event::GetActor()const
{
    return m_actor;
}

void Event::SetMessage(const std::string &msg)
{
    m_msg = msg;
}

std::string Event::GetMessage()const
{
    return m_msg;
}

void Event::SetSubject(const std::string &msg)
{
    m_subject = msg;
}

std::string Event::GetSubject()const
{
    return m_subject;
}

void Event::SetShipType(unsigned char type)
{
    m_shiptype = type;
}
int Event::GetShipType() const
{
    return m_shiptype;
}

void Event::SetShipName(const std::string &msg)
{
    m_shipname = msg;
}
std::string Event::GetShipName()const
{
    return m_shipname;
}
