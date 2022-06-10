﻿#include "stdafx.h"
#include "SpinLock.h"
#include "ServiceBase.h"
#include "NetManager.h"
#include "CommonSocket.h"
#include "DataBuffer.h"
#include "Connection.h"
#include "TimerManager.h"
#include "PacketHeader.h"

#define NEW_CONNECTION 1            // 新的已完成套接字
#define CLOSE_CONNECTION 2          // 关闭已完成套接字

ServiceBase::ServiceBase(void)
{
    m_pPacketDispatcher = NULL;
    m_pRecvDataQueue = new std::deque<NetPacket>();
    m_pDispathQueue = new std::deque<NetPacket>();
}

ServiceBase::~ServiceBase(void)
{
    delete m_pRecvDataQueue;
    delete m_pDispathQueue;

    m_pRecvDataQueue = NULL;
    m_pDispathQueue = NULL;
}

ServiceBase* ServiceBase::GetInstancePtr()
{
    static ServiceBase _ServiceBase;

    return &_ServiceBase;
}


BOOL ServiceBase::OnDataHandle(IDataBuffer* pDataBuffer, INT32 nConnID)
{
    PacketHeader* pHeader = (PacketHeader*)pDataBuffer->GetBuffer();

    m_QueueLock.Lock();
    m_pRecvDataQueue->emplace_back(NetPacket(nConnID, pDataBuffer, pHeader->nMsgID));
    m_QueueLock.Unlock();
    return TRUE;
}

BOOL ServiceBase::StartNetwork(UINT16 nPortNum, INT32 nMaxConn, IPacketDispatcher* pDispather, std::string strListenIp)
{
    ERROR_RETURN_FALSE(pDispather != NULL);
    ERROR_RETURN_FALSE(nPortNum > 0);
    ERROR_RETURN_FALSE(nMaxConn > 0);

    m_pPacketDispatcher = pDispather;

    // keynote 创建连接池，创建线程，epoll，监听套接字
    if (!CNetManager::GetInstancePtr()->Start(nPortNum, nMaxConn, this, strListenIp))
    {
        CLog::GetInstancePtr()->LogError("启动网络层失败!");
        return FALSE;
    }

    m_uLastTick = 0;
    m_nRecvNum = 0;
    m_nFps = 0;
    m_nSendNum = 0;
    m_nLastMsgID = 0;
    return TRUE;
}

BOOL ServiceBase::StopNetwork()
{
    CNetManager::GetInstancePtr()->Stop();

    return TRUE;
}

template<typename T>
BOOL ServiceBase::SendMsgStruct(INT32 nConnID, INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, T& Data)
{
    if (nConnID <= 0)
    {
        return FALSE;
    }

    m_nSendNum++;

    return CNetManager::GetInstancePtr()->SendMessageData(nConnID, nMsgID, u64TargetID, dwUserData, &Data, sizeof(T));
}

BOOL ServiceBase::SendMsgProtoBuf(INT32 nConnID, INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, const google::protobuf::Message& pdata)
{
    if (nConnID <= 0)
    {
        CLog::GetInstancePtr()->LogWarn("SendMsgProtoBuf Error nConnID is Zero MessageID:%d", nMsgID);
        return FALSE;
    }

    char szBuff[102400] = {0};

    ERROR_RETURN_FALSE(pdata.ByteSize() < 102400);

    pdata.SerializePartialToArray(szBuff, pdata.GetCachedSize());
    m_nSendNum++;
    return CNetManager::GetInstancePtr()->SendMessageData(nConnID, nMsgID, u64TargetID, dwUserData, szBuff, pdata.GetCachedSize());
}

BOOL ServiceBase::SendMsgRawData(INT32 nConnID, INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, const char* pdata, UINT32 dwLen)
{
    if (nConnID <= 0)
    {
        return FALSE;
    }

    m_nSendNum++;

    return CNetManager::GetInstancePtr()->SendMessageData(nConnID, nMsgID, u64TargetID, dwUserData, pdata, dwLen);
}

BOOL ServiceBase::SendMsgBuffer(INT32 nConnID, IDataBuffer* pDataBuffer)
{
    if (nConnID == 0)
    {
        return FALSE;
    }

    m_nSendNum++;
    return CNetManager::GetInstancePtr()->SendMessageBuff(nConnID, pDataBuffer);
}

CConnection* ServiceBase::ConnectTo( std::string strIpAddr, UINT16 sPort )
{
    ERROR_RETURN_NULL(!strIpAddr.empty() && sPort > 0);

    return CNetManager::GetInstancePtr()->ConnectTo_Async(strIpAddr, sPort);
}

BOOL ServiceBase::CloseConnect(INT32 nConnID)
{
    CConnection* pConnection = GetConnectionByID(nConnID);

    ERROR_RETURN_FALSE(pConnection != NULL);

    pConnection->Close();

    return TRUE;
}

BOOL ServiceBase::OnCloseConnect(INT32 nConnID)
{
    ERROR_RETURN_FALSE(nConnID != 0);
    m_QueueLock.Lock();
    m_pRecvDataQueue->emplace_back(NetPacket(nConnID, NULL, CLOSE_CONNECTION));
    m_QueueLock.Unlock();
    return TRUE;
}

BOOL ServiceBase::OnNewConnect(INT32 nConnID)
{
    ERROR_RETURN_FALSE(nConnID != 0);
    m_QueueLock.Lock();
    m_pRecvDataQueue->emplace_back(NetPacket(nConnID, NULL, NEW_CONNECTION));
    m_QueueLock.Unlock();
    return TRUE;
}


CConnection* ServiceBase::GetConnectionByID( INT32 nConnID )
{
    return CConnectionMgr::GetInstancePtr()->GetConnectionByID(nConnID);
}

BOOL ServiceBase::Update()
{
    if (m_uLastTick == 0)
    {
        // 初始化tick数
        m_uLastTick = CommonFunc::GetTickCount();
    }

    m_QueueLock.Lock();     // 加自选锁
    // 交换容器中数据，实际值交换了指针
    std::swap(m_pRecvDataQueue, m_pDispathQueue);
    m_QueueLock.Unlock();   // 解除自选锁

    if (m_pDispathQueue->size() > 0)            // 分配队列不为空
    {
        for (std::deque<NetPacket>::iterator itor = m_pDispathQueue->begin(); itor != m_pDispathQueue->end(); ++itor)
        {
            NetPacket& item = *itor;
            if (item.m_nMsgID == NEW_CONNECTION)
            {
                // 什么事情也没做？？？
                m_pPacketDispatcher->OnNewConnect(item.m_nConnID);
            }
            else if (item.m_nMsgID == CLOSE_CONNECTION)
            {
                // 什么事情也没做？？？
                m_pPacketDispatcher->OnCloseConnect(item.m_nConnID);
                //发送通知
                CConnectionMgr::GetInstancePtr()->DeleteConnection(item.m_nConnID);
            }
            else
            {
                m_nLastMsgID = item.m_nMsgID;
                m_pPacketDispatcher->DispatchPacket(&item);

                item.m_pDataBuffer->Release();

                m_nRecvNum += 1;
            }
        }

        m_pDispathQueue->clear();           // 清空
    }

    m_nFps += 1;

    // 将统计量写入运行日志
    if((CommonFunc::GetTickCount() - m_uLastTick) > 1000)
    {
        m_pPacketDispatcher->OnSecondTimer();

        CLog::GetInstancePtr()->SetTitle("[AreaID:%d]-[FPS:%d]-[RecvPack:%d]--[SendPack:%d]", CConfigFile::GetInstancePtr()->GetIntValue("areaid"), m_nFps, m_nRecvNum, m_nSendNum);
        m_nFps = 0;
        m_nRecvNum = 0;
        m_nSendNum = 0;
        m_uLastTick = CommonFunc::GetTickCount();
    }

    // 处理定时器任务
    TimerManager::GetInstancePtr()->UpdateTimer();

    return TRUE;
}

BOOL ServiceBase::FixFrameNum(INT32 nFrames)
{
    if (nFrames < 1)
    {
        nFrames = 1;
    }

    static UINT64 uNextTick = CommonFunc::GetTickCount();
    UINT64 uCurTick = CommonFunc::GetTickCount();

    if (uNextTick > uCurTick)
    {
        CommonFunc::Sleep(uNextTick - uCurTick);
    }

    uNextTick = uNextTick + 1000 / nFrames;

    return TRUE;
}
