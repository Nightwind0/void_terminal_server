#ifndef EVENT_H
#define EVENT_H


#include <string>

using std::string;


class Event
{
 public:

    enum EventType{SHIP_DESTROYED, NEW_PLAYER };

    Event(EventType type);
    virtual ~Event();

    EventType GetType()const;
    
    void SetActor(const std::string &player);
    std::string GetActor()const;

    void SetMessage(const std::string &msg);
    std::string GetMessage()const;

    void SetSubject(const std::string &msg);
    std::string GetSubject()const;

    void SetShipType(int type);
    int GetShipType() const;

    void SetShipName(const std::string &msg);
    std::string GetShipName()const;
    
 private:

   EventType m_type;
   string m_actor;
   string m_msg;
   string m_subject;
   int m_shiptype;
   string m_shipname;
};


#endif
