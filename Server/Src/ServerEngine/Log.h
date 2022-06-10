#ifndef _GLOBAL_LOG_H_
#define _GLOBAL_LOG_H_

// 日志级别
enum LogLevel
{
    Log_HiInfo,
    Log_Error,
    Log_Warn,
    Log_Info,
    Log_None
};

/* 运行时日志 */
class CLog
{
private:
    CLog(void);
    ~CLog(void);

public:
    static CLog* GetInstancePtr();

    // 创建日志文件
    BOOL Start(std::string strPrefix, std::string strLogDir = "log");

    // 关闭日志文件
    BOOL Close();

    // 四种级别的日志
    void LogHiInfo(char* lpszFormat, ...);

    void LogWarn(char* lpszFormat, ...);

    void LogError(char* lpszFormat, ...);

    void LogInfo(char* lpszFormat, ...);

    // 修改日志级别
    void SetLogLevel(int Level);

    void SetTitle(char* lpszFormat, ...);

    // 检查是否需要截断日志文件
    void CheckAndCreate();

protected:
    std::mutex          m_WriteMutex;       // 写互斥锁

    INT32               m_LogCount;         // 统计量

    FILE*               m_pLogFile;         // 日志文件

    INT32               m_LogLevel;         // 日志级别

    std::string         m_strPrefix;        // 日志文件名，默认后缀".log"
    std::string         m_strLogDir;        // 日志目录，默认log
};

// 断言
#define LOG_FUNCTION_LINE  CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);

#define ERROR_RETURN_TRUE(P) \
    if((P) == FALSE)\
    {\
        CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return TRUE;    \
    }


#define ERROR_RETURN_FALSE(P) \
    if((P) == FALSE)\
    {\
        CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return FALSE;   \
    }

#define ERROR_RETURN_NULL(P) \
    if((P) == FALSE)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return NULL;    \
}

#define ERROR_RETURN_NONE(P) \
    if((P) == FALSE)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return ;    \
}

#define ERROR_RETURN_VALUE(P, V) \
    if((P) == FALSE)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return V;   \
}

#define ERROR_CONTINUE_EX(P) \
    if((P) == FALSE)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#define ERROR_TO_CONTINUE(P) \
    if((P) == FALSE)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#define PARSE_FROM_PACKET(TYPE, VALUE)   TYPE VALUE; \
ERROR_RETURN_TRUE(VALUE.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth()));

#endif