#include "DBInterface/CppMysql.h"
#ifndef _LOG_MSG_HANDLER_H_
#define _LOG_MSG_HANDLER_H_

/* 日志处理类 */
class CLogMsgHandler
{
public:
	CLogMsgHandler();

	~CLogMsgHandler();

	BOOL		Init(INT32 nReserved);

	BOOL		Uninit();

	BOOL		OnUpdate(UINT64 uTick);

    // 处理分派的数据
	BOOL		DispatchPacket(NetPacket* pNetPacket);

public:
	//*********************消息处理定义开始******************************
	BOOL		OnMsgLogDataNtf(NetPacket* pNetPacket);
	//*********************消息处理定义结束******************************

	CppMySQL3DB     m_DBConnection;     // 数据库连接

	UINT32			m_nWriteCount;      // 写入次数

	UINT64          m_nLastWriteTime;   // 最后写入时间

};

#endif //_STAT_MSG_HANDLER_H_
