﻿#ifndef _TIMER_MANAGER__
#define _TIMER_MANAGER__
#include "CommonFunc.h"
#include "Platform.h"

class CTimerSlotBase
{
public:
    virtual ~CTimerSlotBase() {}
    virtual BOOL operator()(INT32 pData)
    {
        return TRUE;
    }
    virtual VOID* GetThisAddr()
    {
        return 0;
    }
};

template<typename T>
class CTimerSlot : public CTimerSlotBase
{
    typedef BOOL (T::*FuncType)(INT32);
public:
    CTimerSlot(BOOL (T::*FuncType)(INT32), T* pObj)
        : m_pFuncPtr(FuncType), m_pThis(pObj)
    {

    }
    virtual ~CTimerSlot() {}

    virtual BOOL operator() (INT32 pData)
    {
        if (m_pThis != NULL && m_pFuncPtr != NULL)
        {
            (m_pThis->*m_pFuncPtr)(pData);
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual VOID* GetThisAddr()
    {
        return reinterpret_cast<VOID*>(m_pThis);
    }

private:
    FuncType m_pFuncPtr;
    T*      m_pThis;
};

/* 定时器对象 */
struct TimeEvent
{
public:
    TimeEvent()
    {
        m_uFireTime     = 0;
        m_nSec          = 0;
        m_nData         = 0;
        m_pNext         = NULL;
        m_pPrev         = NULL;
        m_nRepeateTimes = 0x0FFFFFFF;           // 重复执行的次数
        m_pTimerFuncSlot = NULL;
    }

    ~TimeEvent()
    {
        Reset();
    }

    void Reset()
    {
        m_uFireTime     = 0;
        m_nSec          = 0;
        m_nData         = 0;
        m_pNext          = NULL;
        m_pPrev          = NULL;
        m_nRepeateTimes = 0x0FFFFFFF;
        if(m_pTimerFuncSlot != NULL)
        {
            delete m_pTimerFuncSlot;
            m_pTimerFuncSlot = NULL;
        }
    }

    UINT64 m_uFireTime;  // 触发时间戳
    INT32  m_nSec;      // 时长
    INT32  m_nData;     //
    TimeEvent* m_pPrev; //前一节点
    TimeEvent* m_pNext; //后一节点
    INT32  m_nType;   //事件类型,1 绝对时间定时器,2 相对时间定时器
    INT32  m_nRepeateTimes;
    CTimerSlotBase* m_pTimerFuncSlot;
};

/* 定时器管理 */
class EngineClass TimerManager
{
    TimerManager();
    ~TimerManager();

public:
    static TimerManager* GetInstancePtr();

public:
    // 绝对时间定时器
    template<typename T>
    BOOL AddFixTimer(INT32 nSec, INT32 nData, BOOL(T::*FuncPtr)(INT32), T* pObj)
    {
        TimeEvent* pNewEvent = NULL;
        if (m_pFreeHead == NULL)
        {   // 空闲链表为空，则创建新的新的定时器对象
            pNewEvent = new TimeEvent;
        }
        else
        {   // 如果不为空，则从空闲链表中取出定时器对象
            pNewEvent = m_pFreeHead;
            m_pFreeHead = m_pFreeHead->m_pNext;
            if (m_pFreeHead != NULL)
            {
                m_pFreeHead->m_pPrev = NULL;
            }
        }

        pNewEvent->m_pNext = NULL;
        pNewEvent->m_pPrev = NULL;

        pNewEvent->m_nData = nData;
        pNewEvent->m_uFireTime = CommonFunc::GetDayBeginTime() + nSec;

        pNewEvent->m_nSec = nSec;
        pNewEvent->m_nType = 1;
        pNewEvent->m_pTimerFuncSlot = new CTimerSlot<T>(FuncPtr, pObj);     // callback

        if (m_pUsedHead == NULL)
        {   // 如果触发链表为空，则直接加入
            m_pUsedHead = pNewEvent;
        }
        else
        {   // 否则，头插入触发链表
            pNewEvent->m_pNext = m_pUsedHead;
            m_pUsedHead->m_pPrev = pNewEvent;
            m_pUsedHead = pNewEvent;
            m_pUsedHead->m_pPrev = NULL;
        }

        return TRUE;
    }

    // 相对时间定时器
    template<typename T>
    BOOL AddDiffTimer(INT32 nSec, INT32 nData, BOOL(T::*FuncPtr)(INT32), T* pObj)
    {
        TimeEvent* pNewEvent = NULL;
        if (m_pFreeHead == NULL)
        {
            pNewEvent = new TimeEvent;
        }
        else
        {
            pNewEvent = m_pFreeHead;
            m_pFreeHead = m_pFreeHead->m_pNext;
            m_pFreeHead->m_pPrev = NULL;
        }

        pNewEvent->m_pNext = NULL;
        pNewEvent->m_pPrev = NULL;

        pNewEvent->m_nData = nData;

        pNewEvent->m_uFireTime = CommonFunc::GetCurrTime() + nSec;
        pNewEvent->m_nSec = nSec;
        pNewEvent->m_nType = 2;

        pNewEvent->m_pTimerFuncSlot = new CTimerSlot<T>(FuncPtr, pObj);
        if (m_pUsedHead == NULL)
        {
            m_pUsedHead = pNewEvent;
        }
        else
        {
            pNewEvent->m_pNext = m_pUsedHead;
            m_pUsedHead->m_pPrev = pNewEvent;
            m_pUsedHead = pNewEvent;
            m_pUsedHead->m_pPrev = NULL;
        }

        return TRUE;
    }


    BOOL DelTimer( INT32 nSec, INT32 nData);

    VOID UpdateTimer();

    BOOL OnTimerEvent( TimeEvent* pEvent );

    BOOL InitTimer();

    BOOL Clear();

    TimeEvent* m_pUsedHead;         // 触发链表，保存需要触发的定时器对象

    TimeEvent* m_pFreeHead;         // 空闲链表，保存空闲的定时器对象

    UINT64     m_uCurTime;

    UINT64     m_uInitTime;  // 定时器开始工作时间(不对开始工作时间之前的定时器发生作用)
public:
};



#endif /* _TIMER_MANAGER__ */
