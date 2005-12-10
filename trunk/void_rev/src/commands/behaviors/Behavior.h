#ifndef VOID_BEHAVIOR_H
#define VOID_BEHAVIOR_H

class VoidServerThread;

class Behavior
{
public:
    Behavior(VoidServerThread *pThread){ m_pThread = pThread; }
    virtual ~Behavior(){}

protected:
    VoidServerThread * get_behavior_thread() const { return m_pThread; }
private:
    VoidServerThread * m_pThread;
};

#endif
