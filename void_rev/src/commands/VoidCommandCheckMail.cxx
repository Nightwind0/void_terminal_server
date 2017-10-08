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


    pqxx::result dbresult = get_thread()->DBExec(sql);

    int num_mail = dbresult.size();

    if(num_mail < 1) {
	return true;
    }

    std::ostringstream mail;
    int i = 0;
    for(auto row : dbresult) {
      mail << Color()->get(BLACK,BG_WHITE) << "Message From " << Color()->get(RED,BG_WHITE) << row[5].as<std::string>();
      mail << Color()->get(BLACK,BG_WHITE) << " On " << Color()->get(RED,BG_WHITE) 
	   << row[1].as<std::string>() << '/' << row[0].as<std::string>() << '/' << row[2].as<std::string>()
	   << ' ' << row[3].as<std::string>() << ':' << row[4].as<std::string>() << Color()->blackout() << endr;

      if(row[6].as<std::string>() != "t")
	mail << Color()->get(LIGHTCYAN,BG_BLACK);
      else mail << Color()->get(LIGHTGREEN, BG_BLACK);
	
      Send(mail.str());

      mail.str("");

      get_thread()->SendWordWrapped(row[7].as<std::string>() + endr,80);

      std::string markread = (std::string)"delete from mail where nmailid =" + row[8].as<std::string>() + ";";
      get_thread()->DBExec(markread); // TODO Check for error
	
      if(++i !=  num_mail) {
	  Send(Color()->get(GREEN) + "Read Next Mail? (Y/n):");
	  std::string next = ReceiveLine();
	  
	  LOWERCASE(next);
	  
	  if(!CompStrings(next,"yes"))
	    {
	      break;
	    }
	}
      
    }
    

    return true;
}
    


