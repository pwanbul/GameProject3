#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "LogMsgHandler.h"

/* 日志服务器 */
class CGameService : public IPacketDispatcher
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

    // 初始化
	BOOL		Init();

	BOOL		Uninit();

    // 进入死循环中运行
	BOOL		Run();

    // 创建新的已完成套接字
	BOOL		OnNewConnect(INT32 nConnID);

	BOOL		OnCloseConnect(INT32 nConnID);

	BOOL		OnSecondTimer();

    // 分派数据包
	BOOL		DispatchPacket( NetPacket* pNetPacket);

public:
    // 日志处理类，日志服务器的功能由此承担
	CLogMsgHandler		m_LogMsgHandler;

public:
	//*********************消息处理定义开始******************************
	//*********************消息处理定义结束******************************
};

#endif
