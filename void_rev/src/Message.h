#ifndef VOID_MESSAGE_H
#define VOID_MESSAGE_H

#include <string>
#include <sys/socket.h>
#include "Socket.h"

class Message
{
 public:

    enum Type{SYSTEM,ADMIN,COMM,TRANSMIT,BROADCAST,LINK, BATTLE};

    Message(Type type, std::string str);
    Message();
    virtual ~Message();


    void SetType(Type type);
    Type GetType()const;


    std::string GetFrom()const;
    void SetFrom(const std::string &str);


    std::string GetString()const;
    std::string SetString(const std::string &str);
    
    // Returns the name of the sender socket
    virtual std::string ReadFromSocket(DatagramSocketPtr socket);
    virtual void WriteToSocket(DatagramSocketPtr socket, std::string to)const;


 private:

    struct msg_hdr
    {
	Type type;
	unsigned int str_len;
	unsigned int from_len;
    };

    Type m_type;
    std::string m_string;
    std::string m_from;

};

using MessagePtr = std::shared_ptr<Message>;


#endif
