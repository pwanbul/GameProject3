#ifndef _NET_MANAGER_H_
#define _NET_MANAGER_H_
#include "IBufferHandler.h"
#include "Connection.h"
#include "Platform.h"

/* 网络管理器 */
class CNetManager
{
    CNetManager(void);

    virtual ~CNetManager(void);
public:
    static CNetManager* GetInstancePtr()
    {
        static CNetManager NetManager;

        return &NetManager;
    }
public:
    // 启动网络管理器
    BOOL    Start(UINT16 nPortNum,  INT32 nMaxConn, IDataHandler* pBufferHandler, std::string strIpAddr);

    BOOL    Stop();

    BOOL    SendMessageData(INT32 nConnID,  INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData,  const char* pData, UINT32 dwLen);

    BOOL    SendMessageBuff(INT32 nConnID, IDataBuffer* pBuffer);


    BOOL    InitNetwork();

    BOOL    UninitNetwork();

    // 开启监听
    BOOL    StartNetListen(UINT16 nPortNum, std::string strIpAddr);

    BOOL    StopListen();

    //以下是完成端口部分(windows的概念)
public:
    // 创建epollfd
    BOOL    CreateCompletePort();

    BOOL    DestroyCompletePort();

    // 创建事件线程
    BOOL    CreateEventThread(INT32 nNum);

    BOOL    CloseEventThread();

    BOOL    WorkThread_ProcessEvent(INT32 nParam);

    BOOL    WorkThread_Listen();

    BOOL    EventDelete(CConnection* pConnection);

    BOOL    PostSendOperation(CConnection* pConnection);

    CConnection*    AssociateCompletePort(SOCKET hSocket, BOOL bConnect);

    CConnection*    ConnectTo_Sync(std::string strIpAddr, UINT16 sPort);

    CConnection*    ConnectTo_Async(std::string strIpAddr, UINT16 sPort);

    // epoll_ctl，m_hListenSocket
    BOOL            WaitConnect();
public:
    SOCKET              m_hListenSocket;            // 监听套接字
    NetIoOperatorData   m_IoOverlapAccept;
    SOCKET              m_hCurAcceptSocket;
    HANDLE              m_hCompletePort;            // epollfd
    CHAR                m_AddressBuf[128];
    BOOL                m_bCloseEvent;      // 是否关闭事件处理线程

    IDataHandler*       m_pBufferHandler;
    std::vector<std::thread*> m_vtEventThread;      // 线程队列，指针

};

#endif

