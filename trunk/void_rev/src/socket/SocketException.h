#ifndef VOID_SOCKET_EXCEPTION_H
#define VOID_SOCKET_EXCEPTION_H

enum SocketError{CREATEERROR, BINDERROR, LISTENERROR, ACCEPTERROR, CONNREFUSED, NOTCONN, UNKNOWN, PIPE, WOULDBLOCK, SELECTERROR};

class SocketException
{
 public:
    SocketException(SocketError type, int err = 0):m_type(type), m_errno(err){}

    SocketError GetType()const;

    int GetErrno()const{ return m_errno;}

 private:
    SocketError m_type;
    int m_errno;
};



#endif
