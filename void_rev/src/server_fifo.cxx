#include "VoidServerThread.h"
#include <iostream>
#include <string>
#include <fstream>
#include "void_util.h"
#include "ResourceMaster.h"

const int VOID_PORT = 5005;


using namespace std;

int main()
{
    VoidServerThread *void_thread = new VoidServerThread();
    //  TCPServerThread *tcp_thread = new TCPServerThread();
    TCPServer *server;

    bool done = false;
    

    server = new TCPServer(VOID_PORT,5,10,10,1,void_thread,false); // ipV6 support? Switch that false to a true and blammo.

    cout << "Void Revolution Server: Version " << VOID_VER_MAJOR << '.' << VOID_VER_MINOR << '.' << VOID_VER_INCR << endl;


    ifstream in;




    in.open("/tmp/void-server",ifstream::in);
    
    if(in.bad()) exit(1);




    while(!done && in.good())
    {


	string command; 


#ifdef INTERACTIVE_MODE

	cout << endl << "Server Command: ";
	cin >> command;
#else

	in >> command >> ws;
#endif


	if(command == "shutdown")
	{
	    cout << "Immediate Shutdown! Dropping connections..." << endl;
	    done = true;

	    delete server;

	}

	if(command == "list")
	{
	    // TODO: List connections
	    for(vector<TCPSocket*>::iterator i = ResourceMaster::GetInstance()->GetSocketsBegin();
		i != ResourceMaster::GetInstance()->GetSocketsEnd();
		i++)
	    {
		TCPSocket *s = *i;
		cout << s->RemoteIPAddress().GetAddressString() << endl;
	    }
	}

	if(command == "quit")
	{
	    //TODO: Send quit message to all connections
	    done = true;
	    delete server;
	}
	if(command == "help" || command == "?")
	{
	    cout << "Commands: shutdown, list, quit, help" << endl;
	}

	


    }

    delete void_thread;

    in.close();


}
