
#include <string>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>


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


    if(selectval == -1)
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

    if(s == -1)
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

    my_addr.sin_addr.s_addr = INADDR_ANY;
 
    my_addr.sin_family = AF_INET;     
    my_addr.sin_port = htons(port);    
//    my_addr.sin_addr.s_addr = inet_addr(localIP);
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct
    

    int res = bind(getSocketid(), (struct sockaddr *)&my_addr, sizeof(struct sockaddr));

    if(res == -1)
    {
	throw SocketException(BINDERROR, res);
    }
}

void TCPSocket::Listen(int backlog)
{

    int res = listen(getSocketid(), backlog);

    if(res == -1)
    {
	throw SocketException(LISTENERROR);
    }
}

Socket * TCPSocket::Accept()
{
    socklen_t sin_size = sizeof(struct sockaddr_in);
    sockaddr_in their_addr;
    int new_fd = accept(getSocketid(), (struct sockaddr *)&their_addr, &sin_size);

    if(new_fd == -1)
    {
	throw SocketError(ACCEPTERROR);
    }

    std::string addy  = inet_ntoa(their_addr.sin_addr);

    TCPSocket * nSocket = new TCPSocket(addy, new_fd);

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
	    throw SocketError(WOULDBLOCK);
	    break;
	case EPIPE:
	    throw SocketError(PIPE);
	    break;
	default:
	    throw SocketError(UNKNOWN);
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
	    throw SocketError(CONNREFUSED);
	    break;
	case ENOTCONN:
	    throw SocketError(NOTCONN);
	    break;
	default:
	    throw SocketError(UNKNOWN);
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
	    throw SocketError(WOULDBLOCK);
	    break;
	case EPIPE:
	    throw SocketError(PIPE);
	    break;
	default:
	    throw SocketError(UNKNOWN);
	}
    }

}


int UNIXDatagramSocket::RecvFrom(void *buf, int len, unsigned int flags=MSG_NOSIGNAL)
{
 
    sockaddr_un sockadd;
    socklen_t sockadd_size;
    std::string from;

    int chunk = recvfrom(getSocketid(),buf,len,flags, (sockaddr*)&sockadd,&sockadd_size);

    from = sockadd.sun_path;

    if(chunk == -1)
    {
	switch(errno)
	{
	case ECONNREFUSED:
	    throw SocketError(CONNREFUSED);
	    break;
	case ENOTCONN:
	    throw SocketError(NOTCONN);
	    break;
	default:
	    throw SocketError(UNKNOWN);
	}
    }

    return chunk;

}







