#ifndef VOID_TOOLSET_H
#define VOID_TOOLSET_H


#include <string>
#include "void_util.h"
#include "Event.h"
#include <pqxx/pqxx>

class ToolSet
{
public:
    ToolSet(DatabaseConnPtr dbconn);
    virtual ~ToolSet();

    void LogEvent(const Event &event );
protected:
    pqxx::result DBExec(const std::string &stmt);
    DatabaseConnPtr GetDBConn() const { return m_dbconn; }

    double g_random(int number, float mean_per, float deviation) const;
    double g_rand() const;

private:
    DatabaseConnPtr m_dbconn;
};

#endif
