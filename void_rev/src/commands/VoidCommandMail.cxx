#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandMail.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "VoidServerThread.h"
#include "PlayerHandle.h"
#include <sstream>

VoidCommandMail::VoidCommandMail(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandMail::~VoidCommandMail()
{
}

string VoidCommandMail::GetCommandString()const
{
    return "Mail";
}

string VoidCommandMail::GetDescription()const
{
    return "Send mail to another player.";
}

string VoidCommandMail::GetSyntax()const
{
    return "mail <player name>";
}

    
bool VoidCommandMail::ClaimCommand(const string &command)
{
    if (CompStrings(command,"mail")) return true;

    return false;
}

bool VoidCommandMail::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    return SendMail(arguments);


}


bool VoidCommandMail::SendMail(const std::string &to)
{


    if(!isValidPlayer(to))
    {
	Send(Color()->get(RED) + to + " is not a valid player." + endr);
	return false;
    }

    string message;

    string line;


    Send(Color()->get(GREEN) + "Enter your message now" + endr + "Enter a blank line to end:" + Color()->get(LIGHTCYAN) +  endr);

    while((line = ReceiveLine()) != "")
    {
	line += endr;
	message += line;
    }
    
    std::ostringstream insert ;
    insert << "insert into mail values(nextval('mail_id_seq'),now(),";
    insert << '\'' << to << "','" << get_thread()->GetPlayer()->GetName().GetAsString() ;
    insert << "',FALSE,'" << PrepareForSQL(message) << "');";

    PGresult *dbresult =get_thread()->DBExec(insert.str());


    if(PQresultStatus(dbresult) != PGRES_COMMAND_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	throw e;
    }

    PQclear(dbresult);
    
    //TODO: Send a system message to player if online, indicating new mail

    Send(Color()->get(GREEN) + endr + "Message sent." + endr);

    

    return true;

}
    


