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
private:
    PGconn * m_dbconn;
};

#endif
