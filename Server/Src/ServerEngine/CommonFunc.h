#ifndef __COMMON_FUNCTION__
#define __COMMON_FUNCTION__

#define GET_BIT(X,Y) (((X) >> (Y-1)) & 1)
#define SET_BIT(X,Y) ((X) |= (1 << (Y-1)))
#define CLR_BIT(X,Y) ((X) &= (~(1<<Y-1)))

/* 通用misc函数 */
namespace CommonFunc
{
INT32           GetProcessorNum();

// 获取进程的cwd
std::string     GetCurrentWorkDir();

// 设置cwd
BOOL            SetCurrentWorkDir(std::string strPath);

// 获取进程可执行文件的目录
std::string     GetCurrentExeDir();

// 创建目录
BOOL            CreateDir(std::string& strDir);

BOOL            GetDirFiles(const char* pszDir, char* pszFileType, std::vector<std::string>& vtFileList, BOOL bRecursion);

BOOL            GetSubDirNames(const char* pszDir, const char* pszBegin, std::vector<std::string>& vtDirList, BOOL bRecursion);

BOOL            IsSameDay(UINT64 uTime);

BOOL            IsSameWeek(UINT64 uTime);

BOOL            IsSameMonth(UINT64 uTime);

INT32           DiffWeeks(UINT64 uTimeSrc, UINT64 uTimeDest);

INT32           DiffDays(UINT64 uTimeSrc, UINT64 uTimeDest);

// UTC 秒
UINT64          GetCurrTime();

// 获取当前的毫秒数
UINT64          GetCurMsTime();

// 当前墙上时间
tm              GetCurrTmTime();

UINT64          GetDayBeginTime(UINT64 uTime = 0); //获取当天起点的秒数

UINT64          GetWeekBeginTime(UINT64 uTime = 0); //获取当周起点的秒数

UINT64          GetMonthBeginTime(UINT64 uTime = 0);//获取当月起点的秒数

UINT64          GetMonthRemainTime(UINT64 uTime = 0);   //获取当月剩余的秒数

time_t          YearTimeToSec(INT32 nYear, INT32 nMonth, INT32 nDay, INT32 nHour, INT32 nMin, INT32 nSec);

std::string     TimeToString(time_t tTime);

time_t          DateStringToTime(std::string strDate);

// 获取自系统启动以来的Tick数
UINT64          GetTickCount();

INT32           GetCurThreadID();

INT32           GetCurProcessID();

// 休眠，毫秒，使用nanosleep实现
VOID            Sleep(INT32 nMilliseconds);

INT32           GetFreePhysMemory();

INT32           GetRandNum(INT32 nType);

INT32           GetLastError();

std::string     GetLastErrorStr(INT32 nError);

// HANDLE       CreateShareMemory(std::string strName, INT32 nSize);
//
// HANDLE       OpenShareMemory(std::string strName);

HANDLE          CreateShareMemory(INT32 nModuleID, INT32 nPage, INT32 nSize);

HANDLE          OpenShareMemory(INT32 nModuleID, INT32 nPage);

CHAR*           GetShareMemory(HANDLE hShm);

BOOL            ReleaseShareMemory(CHAR* pMem);

BOOL            CloseShareMemory(HANDLE hShm);

BOOL            KillProcess(INT32 nPid);

BOOL            IsProcessExist(INT32 nPid);

INT32           GetProcessID(const char* pszProcName);

BOOL            StartProcess(const char* pszProcName, const char* pszCommandLine = NULL, const char*  pszWorkPath = NULL);

// keynote：是否重复启动实例
BOOL            IsAlreadyRun(std::string strSignName);

// 向控制台打印
BOOL            PrintColorText(CHAR* pSzText, INT32 nColor);

BOOL            GetBitValue(UINT64 nValue, INT32 nPos);

BOOL            SetBitValue(UINT64& nValue, INT32 nPos, BOOL bValue);
}


#endif /* __COMMON_FUNCTION__*/
