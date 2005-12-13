#ifndef VOID_TOOLSET_H
#define VOID_TOOLSET_H


#include <string>
#include "void_util.h"
#include "Event.h"
#include "libpq-fe.h"

class ToolSet
{
public:
    ToolSet(PGconn * dbconn);
    virtual ~ToolSet();

    void LogEvent(const Event &event );
protected:
    PGresult * DBExec(const std::string &stmt);
    PGconn * GetDBConn() const { return m_dbconn; }

    double g_random(int number, float mean_per, float deviation) const;
    double g_rand() const;

private:
    PGconn * m_dbconn;
};

#endif
