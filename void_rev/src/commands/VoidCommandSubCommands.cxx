
#include "VoidCommandSubCommands.h"
#include "void_util.h"
#include "OutpostHandle.h"
#include "PlayerHandle.h"
#include "ShipHandle.h"
#include "Universe.h"
#include <list>
#include <vector>

VoidCommandSubCommands::VoidCommandSubCommands(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandSubCommands::~VoidCommandSubCommands()
{
    for(std::list<VoidCommand*>::iterator iter = m_commandlist.begin();
	iter != m_commandlist.end();
	iter++)
    {
	delete *iter;
    }
}



bool VoidCommandSubCommands::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    std::vector<std::string> argument_vector = WordsFromString(arguments);

    DisplayIntro();
    DisplayOptionsFull();
    
    if(argument_vector.size()) DoCommand(argument_vector[0], arguments.substr(argument_vector[0].size()+1), bFromPost);

    MainLoop();

    DisplayOutro();
    
    return true;
}

void VoidCommandSubCommands::AddSubCommand(VoidCommand* command)
{
    m_commandlist.push_back(command);
}


bool VoidCommandSubCommands::DoCommand(const string &command, const string &arguments, bool bFromPost)
{
    for(std::list<VoidCommand*>::iterator iter = m_commandlist.begin();
	iter != m_commandlist.end();
	iter++)
    {
	if((*iter)->ClaimCommand(command))
	{
	    (*iter)->HandleCommand(command, arguments, bFromPost);
	    return true;
	}
    }

    return false;
}


void VoidCommandSubCommands::DisplayOptions()
{

    for(std::list<VoidCommand *>::iterator iter = m_commandlist.begin();
	iter != m_commandlist.end();
	iter++)
    {
	Send(GetOptionColor() + (*iter)->GetSyntax() + ": " + GetOptionDescriptionColor() +  (*iter)->GetDescription() + endr);
    }
}


void VoidCommandSubCommands::MainLoop()
{

    bool done = false;

    while(!done)
    {

	Send(GetPrompt());

	std::string line = ReceiveLine();
	std::string command, arguments;

	std::string::size_type  space;
	
	bool claimed = false;

	space = line.find(" ");
	
	if(space == std::string::npos)
	    command = line;
	else command = line.substr(0,space);
	
	if(space != std::string::npos)
	    arguments = line.substr(space +1, line.size() - space -1 );
	else
	    arguments = "";
	
	
	std::transform (command.begin(), command.end(), command.begin(), ToLower());


	if(command != "")
	    claimed = DoCommand(command, arguments, false);
	
	
	if(!claimed)
	{
	    if(command == "?" || command == "help")
	    {
		DisplayOptionsFull();
	    }
	    
	    if(command == "bye"  || command == "quit" || command == "exit")
	    {
		done = true;
	    
	    }
	}
	
    }



}

