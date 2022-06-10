#ifndef _WATCHER_CLIENT_H_
#define _WATCHER_CLIENT_H_

/* 观察者客户端 */
class CWatcherClient
{
private:
    CWatcherClient(void);
    virtual ~CWatcherClient(void);

public:
    static CWatcherClient* GetInstancePtr();

    BOOL        OnNewConnect(INT32 nConnID);

    BOOL        OnCloseConnect(INT32 nConnID);

    BOOL        OnSecondTimer();

    BOOL        DispatchPacket( NetPacket* pNetPacket);

    BOOL        IsRun();

    BOOL        RegExitSignal();

    BOOL        StopServer();

    BOOL                    m_bRun;         // 默认True
public:
    //*********************消息处理定义开始******************************
    //*********************消息处理定义结束******************************
};

#endif //_WATCHER_CLIENT_H_
