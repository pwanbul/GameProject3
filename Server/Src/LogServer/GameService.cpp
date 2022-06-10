#include "stdafx.h"
#include "GameService.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"

CGameService::CGameService(void)
{

}

CGameService::~CGameService(void)
{

}

CGameService* CGameService::GetInstancePtr()
{
    static CGameService _GameService;

    return &_GameService;
}

BOOL CGameService::Init()
{
    // 1. 将cwd修改成进程可执行文件所在目录
    CommonFunc::SetCurrentWorkDir("");

    // 2. 初始化运行日志
    if(!CLog::GetInstancePtr()->Start("LogServer", "log"))
    {
        return FALSE;
    }
    CLog::GetInstancePtr()->LogInfo("---------LogServer启动--------");
    // 3. 读取配置文件
    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        CLog::GetInstancePtr()->LogError("配制文件加载失败!");
        return FALSE;
    }

    // keynote 4. 是否重复启动
    if (CommonFunc::IsAlreadyRun("LogServer" + CConfigFile::GetInstancePtr()->GetStringValue("areaid")))
    {
        CLog::GetInstancePtr()->LogError("LogServer己经在运行!");
        return FALSE;
    }

    // 5.1 设置日志级别
    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("log_log_level"));

    // 5.2 读取服务器监听的端口号
    UINT16 nPort = CConfigFile::GetInstancePtr()->GetRealNetPort("log_svr_port");
    if (nPort <= 0)
    {
        CLog::GetInstancePtr()->LogError("配制文件log_svr_port配制错误!");
        return FALSE;
    }

    // 5.3. 最大连接数
    INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("log_svr_max_con");

    // keynote 6. 启动网络
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this, "127.0.0.1"))
    {
        CLog::GetInstancePtr()->LogError("启动服务失败!");
        return FALSE;
    }

    // keynote 7. 初始化日志消息处理类
    ERROR_RETURN_FALSE(m_LogMsgHandler.Init(0));

    CLog::GetInstancePtr()->LogHiInfo("---------LogServer动成功!--------");

    return TRUE;
}


BOOL CGameService::OnNewConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

BOOL CGameService::OnCloseConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL CGameService::OnSecondTimer()
{

    return TRUE;
}

BOOL CGameService::DispatchPacket(NetPacket* pNetPacket)
{
    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (m_LogMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CGameService::Uninit()
{
    CLog::GetInstancePtr()->LogError("==========LogServer开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    google::protobuf::ShutdownProtobufLibrary();

    CLog::GetInstancePtr()->LogError("==========LogServer关闭完成=======================");

    return TRUE;
}

BOOL CGameService::Run()
{
    // 进入死循环
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        m_LogMsgHandler.OnUpdate(CommonFunc::GetTickCount());

        ServiceBase::GetInstancePtr()->FixFrameNum(30);
    }

    return TRUE;
}

