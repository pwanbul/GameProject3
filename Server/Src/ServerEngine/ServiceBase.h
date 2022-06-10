#ifndef __SERVICE_BASE_H__
#define __SERVICE_BASE_H__

#include "IBufferHandler.h"
#include "Connection.h"
#include "google/protobuf/message.h"
#include "ConfigFile.h"

/* 网络服务 */
class ServiceBase : public IDataHandler//, public CEventFuncManager
{
protected:
    ServiceBase(void);
    virtual ~ServiceBase(void);
public:
    static ServiceBase* GetInstancePtr();

    BOOL            StartNetwork(UINT16 nPortNum, INT32 nMaxConn, IPacketDispatcher* pDispather, std::string strListenIp = "");

    BOOL            StopNetwork();

    BOOL            OnDataHandle(IDataBuffer* pDataBuffer, INT32 nConnID);

    BOOL            OnCloseConnect(INT32 nConnID);

    BOOL            OnNewConnect(INT32 nConnID);

    CConnection*    ConnectTo(std::string strIpAddr, UINT16 sPort);

    BOOL            CloseConnect(INT32 nConnID);

    template<typename T>
    BOOL            SendMsgStruct(INT32 nConnID, INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, T& Data);

    BOOL            SendMsgProtoBuf(INT32 nConnID, INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, const google::protobuf::Message& pdata);

    BOOL            SendMsgRawData(INT32 nConnID, INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, const char* pdata, UINT32 dwLen);

    BOOL            SendMsgBuffer(INT32 nConnID, IDataBuffer* pDataBuffer);

    CConnection*    GetConnectionByID(INT32 nConnID);

    //
    BOOL            Update();

    BOOL            FixFrameNum(INT32 nFrames);

protected:
    IPacketDispatcher*                  m_pPacketDispatcher;        // 处理收到的包的对象

    std::deque<NetPacket>*              m_pRecvDataQueue;           // 收包队列
    std::deque<NetPacket>*              m_pDispathQueue;            // 发包队列
    CSpinLock                           m_QueueLock;                // 自选锁

    //以下用于统计
    UINT64                              m_uLastTick;                // 最近一次的tick数
    INT32                               m_nRecvNum;
    INT32                               m_nSendNum;
    INT32                               m_nFps;
    INT32                               m_nLastMsgID;
};


#endif /*__SERVICE_BASE_H__*/