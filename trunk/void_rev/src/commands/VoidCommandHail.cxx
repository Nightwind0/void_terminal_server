#include <string>
#include "VoidCommand.h"
#include "void_util.h"
#include "VoidServerThread.h"
#include "Message.h"
#include "ResourceMaster.h"
#include "VoidCommandHail.h"

using std::string;


VoidCommandHail::VoidCommandHail(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandHail::~VoidCommandHail()
{
}

string VoidCommandHail::GetCommandString()const
{
    return "Hail";
}
string VoidCommandHail::GetDescription()const
{
    return "Send a message to another online player.";
}
string VoidCommandHail::GetSyntax()const
{
    return "#/hail <player name> <message>";
}
    
bool VoidCommandHail::ClaimCommand(const string &command)
{
    if(CompStrings(command,"hail") || command == "#")
	return true;
    else return false;
}

bool VoidCommandHail::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{
    std::string::size_type space = arguments.find(" ");
    std::string toplayer;
    std::string message;

    if(space == std::string::npos)
	toplayer = arguments;
    else toplayer = arguments.substr(0,space);
    
    if(space != std::string::npos)
	message = arguments.substr(space +1, arguments.size() - space -1 );
    else
	message = "";

    if(toplayer == "")
    {
	get_thread()->Send(Color()->get(RED) + "You must specify a receipient." + endr);
	return false;
	/* Toggle code
	if(get_thread()->CheckComm(VoidServerThread::HAIL))
	{
	    get_thread()->Send(Color()->get(GREEN) + "Hail channel has been turned off." + endr);
	    m_bchannelon = false;
	}
	else
	{
	    
	}
	*/
    }
    else
    {
	if(!Hail(toplayer,message))
	{
	    get_thread()->Send(Color()->get(RED) + "Your message did not go through." + endr);
	}
	else
	{
	    Send(Color()->get(GREEN) + "Sent." + endr);
	}
    }

    return true;
}
    

bool VoidCommandHail::Hail(std::string player, std::string msg)
{
    Message message;
    std::string fromname = (std::string)get_thread()->GetPlayer()->GetName();

    message.SetType(Message::COMM);
    message.SetFrom(fromname);
    message.SetString(msg + endr);

    return ResourceMaster::GetInstance()->SendMessage(get_thread()->GetLocalSocket(),player, &message);
}

 
