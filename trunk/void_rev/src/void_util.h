
#ifndef VOID_UTIL_H
#define VOID_UTIL_H

#include <string>
#include <cctype>
#include <vector>

const int VOID_VER_MAJOR = 0;

const int VOID_VER_MINOR = 0;
const int VOID_VER_INCR = 1;
const int VOID_VER_INCRMINOR = 1;

enum FGColor{BLACK=0,RED,GREEN,BROWN,BLUE,PURPLE,CYAN,GRAY,DARKGRAY,LIGHTRED,LIGHTGREEN,YELLOW,LIGHTBLUE,LIGHTPURPLE,LIGHTCYAN,WHITE};
enum BGColor{BG_BLACK=0,BG_RED,BG_GREEN,BG_YELLOW,BG_BLUE,BG_MAGENTA,BG_CYAN,BG_WHITE};

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


    virtual std::string blackout()const{ return get(BLACK,BG_BLACK) + "        "; }
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

                                                                                
bool CompStrings(std::string s1, std::string s2);
std::string PrepareForSQL(std::string message);

                                                                               
std::string IntToString(const int &i);
std::string DoubleToString(const double &d);
std::string BooleanToString(const bool &b);

std::vector<std::string> WordsFromString(const std::string &str);

#define LOWERCASE(x) std::transform (x.begin(), x.end(), x.begin(), ToLower())


#endif
