#ifndef VOID_SOCKET_H
#define VOID_SOCKET_H

#include <sys/socket.h>
#include <string>
#include <memory>

class Socket
{
 public:
    Socket(const std::string &address, int socketid);
    Socket(const std::string &address);
    ~Socket();
    
    virtual void Create()=0;
   
    virtual void Bind(int port, const std::string &addr)=0;

    virtual void Close();

    // True means this socket is the result of the select,
    // False means its "other"
    // Timeout of 0 means no timeout
    enum class eSelectResult : unsigned char { THIS_SOCKET, OTHER_SOCKET, TIMEOUT };
    
    eSelectResult Select(const Socket &other, int sec_timeout=0);
    
    std::string GetAddress()const;
 protected:
    void setAddress(const std::string &addr);
    void setSocketid(int socketid);
    int getSocketid();
 private:
    int m_socketid;
    std::string m_address;
};

using SocketPtr = std::shared_ptr<Socket>;


class ConnectedSocket : public Socket
{
 public:
    ConnectedSocket(const std::string &addr):Socket(addr){}
    ConnectedSocket(const std::string &addr, int s):Socket(addr,s){}
    virtual  void Send(const void *msg, int len, int flags)=0;
    virtual int Recv(void *buf, int len, unsigned int flags )=0;
    virtual void Listen(int backlog=5)=0;
    virtual SocketPtr  Accept()=0;
};

class DatagramSocket: public Socket
{
 public:
    DatagramSocket(const std::string &addr):Socket(addr){}
    DatagramSocket(const std::string &addr, int s):Socket(addr,s){}
    virtual void SendTo(const void *msg, int len,std::string to,
			unsigned int flags)=0;

    virtual int RecvFrom(void *buf, int len, unsigned int flags)=0;
};

using DatagramSocketPtr = std::shared_ptr<DatagramSocket>;

class TCPSocket : public ConnectedSocket
{
 public:
    TCPSocket(const std::string &address, int socketid);
    TCPSocket(const std::string &address);

    virtual void Create();
//    virtual void Connect(const std::string &addr, int port);;
    virtual void Bind(int port, const std::string &addr);
    virtual void Listen(int backlog=5);
    virtual SocketPtr  Accept();

    void Send(const void *msg, int len, int flags);
    int Recv(void *buf, int len, unsigned int flags);
};



using TCPSocketPtr = std::shared_ptr<TCPSocket>;

class UNIXDatagramSocket : public DatagramSocket
{
 public:
    UNIXDatagramSocket(const std::string &addr);
    UNIXDatagramSocket(const std::string &addr, int s);
    virtual void Create();
   
    virtual void Bind(int port, const std::string &addr);



    virtual void SendTo(const void *msg, int len,std::string to,
			unsigned int flags);

    virtual int RecvFrom(void *buf, int len, unsigned int flags);


};

using UNIXDatagramSocketPtr = std::shared_ptr<UNIXDatagramSocket>;


#endif
