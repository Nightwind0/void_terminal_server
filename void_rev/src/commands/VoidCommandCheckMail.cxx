#include "VoidCommand.h"
#include "void_util.h"
#include "VoidCommandCheckMail.h"
#include "Universe.h"
#include "ResourceMaster.h"
#include "VoidServerThread.h"
#include "PlayerHandle.h"
#include <sstream>

VoidCommandCheckMail::VoidCommandCheckMail(VoidServerThread *thread):VoidCommand(thread)
{
}
VoidCommandCheckMail::~VoidCommandCheckMail()
{
}

string VoidCommandCheckMail::GetCommandString()const
{
    return "CheckMail";
}

string VoidCommandCheckMail::GetDescription()const
{
    return "Read new mail.";
}

string VoidCommandCheckMail::GetSyntax()const
{
    return "checkmail";
}

    
bool VoidCommandCheckMail::ClaimCommand(const string &command)
{
    if (CompStrings(command,"checkmail")) return true;

    return false;
}

bool VoidCommandCheckMail::HandleCommand(const string &command, const string &arguments, bool bFromPost)
{

    return ReadMail();


}


bool VoidCommandCheckMail::ReadMail()
{
    std::string sql = "select extract(day from dstamp), extract (month from dstamp),"
	"extract (year from dstamp), extract (hour from dstamp), extract (minute from dstamp),"
	"kfromplayer, bfromsystem, smessage, nmailid from mail where ktoplayer = '" + get_thread()->GetPlayer()->GetName().GetAsString() + "'; ";


    PGresult *dbresult = get_thread()->DBExec(sql);
    
    if(PQresultStatus(dbresult) != PGRES_TUPLES_OK)
    {

	DBException e(PQresultErrorMessage(dbresult));
	PQclear(dbresult);
	throw e;
    }

    int num_mail = PQntuples(dbresult);

    if(num_mail < 1)
    {
	PQclear(dbresult);
	return true;
    }

    std::ostringstream mail;

    for(int i=0;i<num_mail;i++)
    {
	mail << Color()->get(BLACK,BG_WHITE) << "Message From " << Color()->get(RED,BG_WHITE) << PQgetvalue(dbresult,i,5);
	mail << Color()->get(BLACK,BG_WHITE) << " On " << Color()->get(RED,BG_WHITE) 
	     << PQgetvalue(dbresult,i,1) << '/' << PQgetvalue(dbresult,i,0) << '/' << PQgetvalue(dbresult,i,2) 
	     << ' ' << PQgetvalue(dbresult,i,3) << ':' << PQgetvalue(dbresult,i,4) << Color()->blackout() << endr;

	if((std::string)PQgetvalue(dbresult,i,6) != "t")
	    mail << Color()->get(LIGHTCYAN,BG_BLACK);
	else mail << Color()->get(LIGHTGREEN, BG_BLACK);
	
	Send(mail.str());

	mail.str("");

	get_thread()->SendWordWrapped((std::string)PQgetvalue(dbresult,i,7)+ endr,80);

	std::string markread = (std::string)"delete from mail where nmailid ='" + PQgetvalue(dbresult,i,8) + "';";
	get_thread()->DBExec(markread); // TODO Check for error
	
	Send(Color()->get(GREEN) + "Read Next Mail? (Y/n):");
	std::string next = ReceiveLine();

	LOWERCASE(next);
	
	if(!CompStrings(next,"yes"))
	{
	    break;
	}
	
    }

    PQclear(dbresult);

    return true;
}
    


