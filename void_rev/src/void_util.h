
#ifndef VOID_UTIL_H
#define VOID_UTIL_H

#include <string>
#include <cctype>
#include <vector>
#include <tuple>
#include <algorithm>
#include <memory>
#include <pqxx/pqxx>


/* Defines and constants
 *
 */
const int VOID_VER_MAJOR = 0;

const int VOID_VER_MINOR = 3;
const int VOID_VER_INCR = 1;
const int VOID_VER_INCRMINOR = 1;

class ResourceMaster;
using Sector = unsigned int;
using DatabaseConnPtr = std::shared_ptr<pqxx::connection_base>;

enum FGColor{BLACK=0,RED,GREEN,BROWN,BLUE,PURPLE,CYAN,GRAY,DARKGRAY,LIGHTRED,LIGHTGREEN,YELLOW,LIGHTBLUE,LIGHTPURPLE,LIGHTCYAN,WHITE};
enum BGColor{BG_BLACK=0,BG_RED,BG_GREEN,BG_YELLOW,BG_BLUE,BG_MAGENTA,BG_CYAN,BG_WHITE};

extern const std::string endr; //=  "\n\r";
enum LOG_SEVERITY {EMERGENCY, ERROR, WARNING, DEBUG, DEBUG2, AUDIT};
enum class eSectorFlags : unsigned int {VISITED = 1, AVOID = (1<<1), FAVORITE = (1<<2)};


/* Config utilities
 *
 */
double config_float(ResourceMaster * rm, const std::string& key);
int config_int(ResourceMaster * rm, const std::string& key);
std::string config_string(ResourceMaster *rm, const std::string& key);

class ColorType
{
 public:
    virtual std::string get(FGColor fg, BGColor bg = BG_BLACK)const=0;
    virtual std::string blackout()const=0;
    virtual std::string clear()const=0;
};

class ANSIColor : public ColorType
{
 public:
    virtual std::string get(FGColor fg, BGColor bg = BG_BLACK)const;
    virtual std::string clear()const
    {
	    return "\e[2J\e[H";
    }    


    virtual std::string blackout()const{ return get(BLACK,BG_BLACK) + "    "; }
 private:
};

class GUIColor : public ColorType
{
 public:
    virtual std::string get(FGColor fg, BGColor bg = BG_BLACK)const;
    virtual std::string clear()const{ return "<CLEAR>";}
    virtual std::string blackout()const{ return "";}


 private:
};



struct ToLower
{
     char operator() (char c) const  { return std::tolower(c); }
};


class DBException //: public std::exception
{
 private: 
    std::string m_message;
 public:
    DBException(std::string message){ m_message = message;}

    std::string GetMessage()const{return m_message;} 
};


class ShutdownException
{
 public:
    ShutdownException(bool normal):m_normal(normal){}
    ~ShutdownException(){}

    bool isNormal()const{ return m_normal;}
 private:
    bool m_normal;
};

class ControlException
{
 public:
    ControlException();
    virtual ~ControlException();
};

class ShipDestroyedException : public ControlException
{
 public:
    ShipDestroyedException();
    virtual ~ShipDestroyedException();
};

class DeathException : public ShipDestroyedException
{
};


using StardockData = std::tuple<Sector,std::string>;

                                                                                
bool CompStrings(const std::string& s1,const std::string& s2);


class ShipHandle;
class ShipTypeHandle;
using ShipHandlePtr = std::shared_ptr<ShipHandle>;
using ShipTypeHandlePtr = std::shared_ptr<ShipTypeHandle>;

std::string IntToString(const int &i);
std::string DoubleToString(const double &d);
std::string BooleanToString(const bool &b);

std::vector<std::string> WordsFromString(const std::string &str);

#define LOWERCASE(x) std::transform (x.begin(), x.end(), x.begin(), ToLower())
#define LOG(level,message) \
  (ResourceMaster::GetInstance()->Log(level,message,__FILE__,__LINE__))

#endif
