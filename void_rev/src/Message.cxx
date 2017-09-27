#include "Message.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "ResourceMaster.h"
#include "void_util.h"
#include "Socket.h"

Message::Message()
{
}

Message::~Message()
{
}
 
Message::Message(Type type, std::string str):m_type(type),m_string(str)
{
    
}


void Message::SetType(Type type)
{
    m_type = type;
}

Message::Type Message::GetType()const
{
    return m_type;
}


void Message::SetFrom(const std::string &from)
{
    m_from = from;
}

std::string Message::GetFrom()const
{
    return m_from;
}


std::string Message::GetString()const
{
    return m_string;
}
std::string Message::SetString(const std::string &str)
{
    m_string = str;
    return str;
}

std::string Message::ReadFromSocket(DatagramSocketPtr socket)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
   
    char *buffer, *orig_buffer;
    msg_hdr header;
    buffer = new char[1024];
    orig_buffer = buffer;
    bool gotit = false;

    int str_size =0;
    int from_size = 0;

    RM->Log(AUDIT, "- Decoding MSG - ");

    int chunk = socket->RecvFrom(buffer,1024, MSG_NOSIGNAL);

    RM->Log(AUDIT, " - MSG Chunk size = " + IntToString(chunk));

    header = *((msg_hdr*)buffer);

    m_type = header.type;
    str_size = header.str_len;
    from_size = header.from_len;


    RM->Log(AUDIT, " - MSG Indicated String Size = " + IntToString(str_size));

    buffer += sizeof(msg_hdr);

    m_string = buffer;


    RM->Log(AUDIT, " - MSG Actual String Size = " + IntToString(m_string.size()));

    RM->Log(AUDIT, "- MSG Chunk = " + m_string);

    buffer += m_string.size() +1;

    m_from = buffer;

    RM->Log(AUDIT, "- MSG From = " + m_from);

    
    delete[] orig_buffer;
    return "";
}

void Message::WriteToSocket(DatagramSocketPtr socket, std::string to)const
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    
    
    char * buffer;
    char * orig_buffer;
    int str_size = m_string.size();

    RM->Log(AUDIT, "- Sending MSG: " + m_string);
    RM->Log(AUDIT, "- String size: " + m_string.size());

    buffer = new char[m_string.size() + m_from.size() + sizeof(msg_hdr) + 2];

    orig_buffer = buffer;

    msg_hdr header;
    header.type = m_type;
    header.str_len = str_size +1;
    header.from_len = m_from.size() +1;

    *((msg_hdr*)buffer) = header;

    buffer += sizeof(header);

    strcpy(buffer, m_string.c_str());

    buffer += m_string.size() + 1;

    strcpy(buffer,m_from.c_str());


    socket->SendTo((void*)orig_buffer, sizeof(msg_hdr)+ m_string.size() +1 + m_from.size() + 1,to, MSG_NOSIGNAL);

    delete[] orig_buffer;
}
