
#include <string>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include "Socket.h"
#include "SocketException.h"

SocketError SocketException::GetType()const
{
    return m_type;
}

int Socket::getSocketid()
{
    return m_socketid;
}

void Socket::setSocketid(int id)
{
    m_socketid = id;
}


Socket::Socket(const std::string &address):m_address(address)
{
}

Socket::Socket(const std::string &address, int socketid):m_address(address),m_socketid(socketid)
{
}

Socket::~Socket()
{
}

void Socket::Close()
{
    unsigned int opt = 1;
    setsockopt(m_socketid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	

    shutdown(m_socketid, 2);
    close(m_socketid);
}
   
    
std::string Socket::GetAddress()const
{
    return m_address;
}

void Socket::setAddress(const std::string &addr)
{
    m_address = addr;
}


bool Socket::Select(const Socket &other)
{

    fd_set readfs;
    
    FD_ZERO(&readfs);
    FD_SET(m_socketid, &readfs);
    FD_SET(other.m_socketid,&readfs);
    
    int selectval = select(std::max(other.m_socketid,m_socketid)+1,&readfs,NULL,NULL,NULL);


    if(selectval < 0)
    {
	throw SocketException(SELECTERROR);
    }

    if(FD_ISSET(m_socketid,&readfs)) return true;
    else return false;
    

}


TCPSocket::TCPSocket(const std::string &addr):ConnectedSocket(addr)
{
}

TCPSocket::TCPSocket(const std::string &addr, int s):ConnectedSocket(addr,s)
{
}

void TCPSocket::Create()
{
    int s = socket(AF_INET,SOCK_STREAM,0);

    unsigned int opt = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	throw SocketException(CREATEERROR);

    if(s < 0)
    {
	throw SocketException(CREATEERROR);
    }

    setSocketid( s );

}


void TCPSocket::Bind(int port, const std::string &addr)
{
    //  hostent *localHost = gethostbyname("localhost");

//    if(localHost == NULL) throw SocketException(BINDERROR);

//    char *localIP = inet_ntoa (*(struct in_addr *)*localHost->h_addr_list);
    

    sockaddr_in my_addr;

    memset(&my_addr,0,sizeof(my_addr));
    my_addr.sin_addr.s_addr = INADDR_ANY;
 
    my_addr.sin_family = AF_INET;     
    my_addr.sin_port = htons(port);    
//    my_addr.sin_addr.s_addr = inet_addr(localIP);
    //memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct
    

    int res = bind(getSocketid(), (struct sockaddr *)&my_addr, sizeof(struct sockaddr));

    if(res < 0)
    {
	throw SocketException(BINDERROR, errno);
    }
}

void TCPSocket::Listen(int backlog)
{

    int res = listen(getSocketid(), backlog);

    if(res < 0)
    {
	throw SocketException(LISTENERROR);
    }
}

SocketPtr  TCPSocket::Accept()
{
    socklen_t sin_size = sizeof(struct sockaddr_in);
    sockaddr_in their_addr;
    memset(&their_addr,0,sizeof(their_addr));
    int new_fd = accept(getSocketid(), (struct sockaddr *)&their_addr, &sin_size);

    if(new_fd == -1)
    {
	throw SocketException(ACCEPTERROR);
    }

    std::string addy  = inet_ntoa(their_addr.sin_addr);

    TCPSocketPtr nSocket = std::make_shared<TCPSocket>(addy, new_fd);

    return nSocket;

}

void TCPSocket::Send(const void *msg, int len, int flags=MSG_NOSIGNAL)
{
    int res = send(getSocketid(),msg,len,flags);

    if (res == -1)
    {
	switch(errno)
	{

	case EWOULDBLOCK:
	    throw SocketException(WOULDBLOCK);
	    break;
	case EPIPE:
	    throw SocketException(PIPE);
	    break;
	default:
	    throw SocketException(UNKNOWN);
	}
    }
   
}




int TCPSocket::Recv(void *buf, int len, unsigned int flags =MSG_NOSIGNAL)
{
    int res = recv(getSocketid(), buf, len, flags);

    if(res == -1)
    {
	switch(errno)
	{
	case ECONNREFUSED:
	    throw SocketException(CONNREFUSED);
	    break;
	case ENOTCONN:
	    throw SocketException(NOTCONN);
	    break;
	default:
	    throw SocketException(UNKNOWN);
	}
    }

    return res;
}



UNIXDatagramSocket::UNIXDatagramSocket(const std::string &addr):DatagramSocket(addr)
{
}

UNIXDatagramSocket::UNIXDatagramSocket(const std::string &addr, int s):DatagramSocket(addr,s)
{
}

void UNIXDatagramSocket::Create()
{
    int s = socket(AF_UNIX,SOCK_DGRAM,0);

    if(s == -1)
    {
	throw SocketException(CREATEERROR);
    }

    setSocketid( s );

}


void UNIXDatagramSocket::Bind(int port, const std::string &address)
{

    sockaddr_un addr;
    strcpy(addr.sun_path, address.c_str()); 
    addr.sun_family = AF_UNIX; 
    int res = bind(getSocketid(), (struct sockaddr *) &addr, strlen(addr.sun_path) + 
	 sizeof (addr.sun_family));
    

    if(res == -1)
    {
	throw SocketException(BINDERROR);
    }
}




void UNIXDatagramSocket::SendTo(const void *msg, int len,std::string to,
		    unsigned int flags=MSG_NOSIGNAL)
{
    
    sockaddr_un sockadd;
    sockadd.sun_family = AF_UNIX;
    strcpy(sockadd.sun_path,to.c_str());

    int res = sendto(getSocketid(),msg,len,flags, (sockaddr*)&sockadd, sizeof(sockadd.sun_family) + strlen(sockadd.sun_path));

    if (res == -1)
    {
	switch(errno)
	{

	case EWOULDBLOCK:
	    throw SocketException(WOULDBLOCK);
	    break;
	case EPIPE:
	    throw SocketException(PIPE);
	    break;
	default:
	    throw SocketException(UNKNOWN);
	}
    }

}


int UNIXDatagramSocket::RecvFrom(void *buf, int len, unsigned int flags=MSG_NOSIGNAL)
{
 
    sockaddr_un sockadd;
    socklen_t sockadd_size = sizeof(sockadd);
    std::string from;

    int chunk = recvfrom(getSocketid(),buf,len,flags, (sockaddr*)&sockadd,&sockadd_size);

    from = sockadd.sun_path;

    if(chunk == -1)
    {
	switch(errno)
	{
	case ECONNREFUSED:
	    throw SocketException(CONNREFUSED);
	    break;
	case ENOTCONN:
	    throw SocketException(NOTCONN);
	    break;
	default:
	    std::cerr << "Exception errno: " << errno << ":" << strerror(errno) << std::endl;
	    throw SocketException(UNKNOWN);
	}
    }

    return chunk;

}







