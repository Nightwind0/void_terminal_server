#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <string>
#include "Socket.h"

using namespace std;
    
int main(int argc, char *argv[])
{
    int s, t, len;
    struct sockaddr_un remote;
    char str[65536];

    UNIXDatagramSocket *unixsocket = new UNIXDatagramSocket("/tmp/void-control");


    unixsocket->Create();

    

    if(argc == 2)
    {
	string command = argv[1];
	 unixsocket->SendTo(command.c_str(),command.size(), "/tmp/void-socket",0);
	

	 unixsocket->Close();

	 
	 delete unixsocket;

	return 0;
    }
    

    bool done = false;

    while(!done)
    {
	string command;
	cout << "Enter Command (\\q to quit):" ;
	cin >> command;

	if(command == "\\q")
	{
	    done = true;
	}
	else
	{

	    unixsocket->SendTo(command.c_str(), command.size(), "/tmp/void-socket",0);
	}

    }
    
    
    unixsocket->Close();

    delete unixsocket;
    
    return 0;
}
