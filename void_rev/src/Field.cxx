#include "Field.h"
#include <string>
#include <sstream>
#include <libpq-fe.h>

using namespace std;


/*
void Timestamp::SetFromString(const std::string &str)
{
    m_year = atoi(str.substr(0,4).c_str());
    m_month = atoi(str.substr(5,2).c_str());
    m_day = atoi(str.substr(8,2).c_str());
    m_hour = atoi(str.substr(11,2).c_str());
    m_minute = atoi(str.substr(14,2).c_str());
    m_second = atoi(str.substr(17,2).c_str());



}
*/


int Timestamp::GetMonth() const
{

    return atoi(m_data.substr(5,2).c_str());
    
}
int Timestamp::GetDay() const
{
    return atoi(m_data.substr(8,2).c_str());
}

int Timestamp::GetYear() const
{
    return atoi(m_data.substr(0,4).c_str());
}

int Timestamp::GetHour() const
{
    return atoi(m_data.substr(11,2).c_str());
}
int Timestamp::GetMinute() const
{
    return atoi(m_data.substr(14,2).c_str());
}
int Timestamp::GetSecond() const
{
    return atoi(m_data.substr(17,2).c_str());
}


