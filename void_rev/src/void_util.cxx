#include "void_util.h"
#include <libpq-fe.h>
#include <string>
#include <sstream>


using namespace std;

string ANSIColor::get(FGColor fg, BGColor bg) const
{
    string ret = "\e[";

    switch(fg)
    {
    case BLACK:
	ret += "0;30";
	break;
    case RED:
	ret += "0;31";
	break;
    case GREEN:
	ret += "0;32";
	break;
    case BROWN:
	ret += "0;33";
	break;
    case BLUE:
	ret += "0;34";
	break;
    case PURPLE:
	ret += "0;35";
	break;
    case CYAN:
	ret += "0;36";
	break;
    case GRAY:
	ret += "0;37";
	break;
    case DARKGRAY:
	ret += "1;30";
	break;
    case LIGHTRED:
	ret += "1;31";
	break;
    case LIGHTGREEN:
	ret += "1;32";
	break;
    case YELLOW:
	ret += "1;33";
	break;
    case LIGHTBLUE:
	ret += "1;34";
	break;
    case LIGHTPURPLE:
	ret += "1;35";
	break;
    case LIGHTCYAN:
	ret += "1;36";
	break;
    case WHITE:
	ret += "1;37";
	break;
   

    }


    switch(bg)
    {
    case BG_BLACK:
	ret += ";40";
	break;
    case BG_RED:
	ret += ";41";
	break;
    case BG_GREEN:
	ret += ";42";
	break;
    case BG_YELLOW:
	ret += ";43";
	break;
    case BG_BLUE:
	ret += ";44";
	break;
    case BG_MAGENTA:
	ret += ";45";
	break;
    case BG_CYAN:
	ret += ";46";
	break;
    case BG_WHITE:
	ret += ";47";
	break;
    }


    ret += "m";

    return ret;

}


string GUIColor::get(FGColor fg, BGColor bg) const
{
    string ret = "<";

    switch(fg)
    {
    case BLACK:
	ret += "BLACK";
	break;
    case RED:
	ret += "RED";
	break;
    case GREEN:
	ret += "GREEN";
	break;
    case BROWN:
	ret += "BROWN";
	break;
    case BLUE:
	ret += "BLUE";
	break;
    case PURPLE:
	ret += "PURPLE";
	break;
    case CYAN:
	ret += "CYAN";
	break;
    case GRAY:
	ret += "GRAY";
	break;
    case DARKGRAY:
	ret += "DARKGRAY";
	break;
    case LIGHTRED:
	ret += "LIGHTRED";
	break;
    case LIGHTGREEN:
	ret += "LIGHTGREEN";
	break;
    case YELLOW:
	ret += "YELLOW";
	break;
    case LIGHTBLUE:
	ret += "LIGHTBLUE";
	break;
    case LIGHTPURPLE:
	ret += "LIGHTPURPLE";
	break;
    case LIGHTCYAN:
	ret += "LIGHTCYAN";
	break;
    case WHITE:
	ret += "WHITE";
	break;
   

    }

    ret += ">";

    return ret;

}




bool CompStrings(std::string s1, std::string s2)
{
   std::string s2b = s2.substr(0,s1.size());
                                                                                
   if(s1 == s2b)
         return true;
   return false;
                                                                                
}


/*
std::string PrepareForSQL(std::string message)
{
    string out = message;
    int begin =0;
    int newbegin=0;
    const char * slash = "\\";
    
    // Find occurances of either ' or " and add a slash before them
    while((begin = out.find_first_of("\'\"",begin)) != std::string::npos)
    {
	out.insert(begin,slash);
	begin+=2;
    }

	  return out;



}*/

std::string PrepareForSQL(std::string message)
{
    std::string str;
    char *buffer = new char[(message.size() * 2)  +  1];


    PQescapeString(buffer,message.c_str(),message.size());

    str = buffer;

    delete[] buffer;
    return str;
}


std::string IntToString(const int &i)
{
  return std::to_string(i);
}

std::string DoubleToString(const double &d)
{
  return std::to_string(d);
}

std::string BooleanToString(const bool &b)
{
    if(b) return "t";
    else return "f";
}


std::vector<std::string> WordsFromString(const std::string &str)
{
    std::vector<std::string> words;

    if(!str.size()) return words; // Don't even process if it's empty

    std::string::size_type start=0;
    std::string::size_type space = str.size();

    do
    {
	space = str.find_first_of(' ',start);

	if(space == std::string::npos)
	    space = str.size();

	words.push_back(str.substr(start,space - start));

	start = space + 1;

    }while(start < str.size());

    return words;

}
ControlException::ControlException()
{
}

ControlException::~ControlException()
{
}









ShipDestroyedException::ShipDestroyedException()
{
}

ShipDestroyedException::~ShipDestroyedException()
{
}

const char * endr =  "\n\r";
