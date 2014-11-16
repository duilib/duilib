#ifndef __Log4cplus_H__
#define __Log4cplus_H__
#pragma once

#include <atlstr.h>
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	Log4cplus.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2013-7-28 0:02:28
// 最后修改	:	2013-7-28 0:02:28
// 说明			:	Log4cplus开源日志类Wrap

注意：
1. Debug版所有信息同时输出到Win32调试控制台，可用DbgView查看
2. Release版需要打开FileAppender

Instance 分类	默认为root，无法修改删除
指定消息的分类，支持点分格式的树形结构
	UI
	UI.MainWnd
	SpeedDetection
	SpeedDetection.UserAuth
SetInstanceFilter()接口通过Instance点分格式字符串匹配控制输出内容
如下伪代码将只输出UI.MainWnd及其子分类和SpeedDetection的所有信息
SetInstanceFilter( UI.MainWnd , SpeedDetection)


说明：
LogLevel		int 	日志记录等级，仅输出高于等级的信息
日志系统预定义等级：
ALL_LOG_LEVEL				0
TRACE_LOG_LEVEL			0
DEBUG_LOG_LEVEL			10000
INFO_LOG_LEVEL				20000
WARN_LOG_LEVEL			30000
ERROR_LOG_LEVEL			40000
FATAL_LOG_LEVEL			50000
OFF_LOG_LEVEL				60000

为了保证OFF_LOG_LEVEL能正常生效，自定义日志等级不要高于这个值

PatternLayout
是一种有词法分析功能的模式布局器,类似正则表达式。以“%”作为开头的特殊预定义标识符，将产生特殊的格式信息。

（1）"%%"，转义为% 。
（2）"%c"，输出logger名称，如test.subtest 。也可以控制logger名称的显示层次，比如"%c{1}"时输出"test"，其中数字表示层次。
（3）"%D"，显示本地时间，比如："2004-10-16 18:55:45"，%d显示标准时间。
（4）"%F"，输出当前记录器所在的文件名称，比如"main.cpp"
（5）"%L"，输出当前记录器所在的文件行号，比如"51"
（6）"%l"，输出当前记录器所在的文件名称和行号，比如"main.cpp:51"
（7）"%m"，输出原始信息。
（8）"%n"，换行符。
（9）"%p"，输出LogLevel，比如"DEBUG"
（10）"%t"，输出记录器所在的线程ID，比如 "1075298944"
（11）"%x"，嵌套诊断上下文NDC (nested diagnostic context) 输出，从堆栈中弹出上下文信息，NDC可以用对不同源的log信息（同时地）交叉输出进行区分。
（12）格式对齐，比如"%-10m"时表示左对齐，宽度是10，当然其它的控制字符也可以相同的方式来使用，比如"%-12d"，"%-5p"等等。

时间定义：
			可以通过%d{...}定义更详细的显示格式，比如%d{%H:%M:%s}表示要显示小时:分钟：秒。大括号中可显示的预定义标识符如下：
			%a -- 表示礼拜几，英文缩写形式，比如"Fri"
			%A -- 表示礼拜几，比如"Friday"
			%b -- 表示几月份，英文缩写形式，比如"Oct"
			%B -- 表示几月份，"October"
			%c -- 标准的日期＋时间格式，如 "Sat Oct 16 18:56:19 2004"
			%d -- 表示今天是这个月的几号(1-31)"16"
			%H -- 表示当前时刻是几时(0-23)，如 "18"
			%I -- 表示当前时刻是几时(1-12)，如 "6"
			%j -- 表示今天是哪一天(1-366)，如 "290"
			%m -- 表示本月是哪一月(1-12)，如 "10"
			%M -- 表示当前时刻是哪一分钟(0-59)，如 "59"
			%p -- 表示现在是上午还是下午， AM or PM
			%q -- 表示当前时刻中毫秒部分(0-999)，如 "237"
			%Q -- 表示当前时刻中带小数的毫秒部分(0-999.999)，如 "430.732"
			%S -- 表示当前时刻的多少秒(0-59)，如 "32"
			%U -- 表示本周是今年的第几个礼拜，以周日为第一天开始计算(0-53)，如 "41"
			%w -- 表示礼拜几，(0-6, 礼拜天为0)，如 "6"
			%W -- 表示本周是今年的第几个礼拜，以周一为第一天开始计算(0-53)，如 "41"
			%x -- 标准的日期格式，如 "10/16/04"
			%X -- 标准的时间格式，如 "19:02:34"
			%y -- 两位数的年份(0-99)，如 "04"
			%Y -- 四位数的年份，如 "2004"
			%Z -- 时区名，比如 "GMT"

//*////////////////////////////////////////////////////////////
#include <log4cplus/config.hxx>
#include "log4cplus/clogger.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/appender.h"
#include "log4cplus/loggingmacros.h"

#include <vector>
typedef std::vector<CString> VString;

using namespace log4cplus;
using namespace log4cplus::helpers;

	class LOG4CPLUS_EXPORT CLogger
	{
	public:
		CLogger();
		~CLogger();

		void SetLogPath(LPCTSTR lpszLogPath);
		void SetLogLevel(LogLevel ll);
		void SetInternalDebugging(bool bInternalDebugging=false);	// 是否屏蔽Debug等级信息
		void SetQuietMode(bool bQuietMode=false);	// 是否屏蔽所有输出

		//////////////////////////////////////////////////////////////////////////
		// 仅在启动日志系统之前设置生效
		void SetPatternString(LPCTSTR lpszPattern);				// 日志信息格式
		void SetDebugPatternString(LPCTSTR lpszPattern);	// TRACE信息格式

		LogLevel GetLogLevel();
		CString& GetPatternString();
		CString& GetDebugPatternString();

		/// 启动日志系统
		void InitLogger();

		// 设置基于Instance的过滤
		// 每次设置请过滤掉不符合规则的输出
		void SetInstanceFilter(VString& filterList);

		void ClearInstanceFilter();		// 仅清空InstanceFilter列表,不生效
		void AddInstanceFilter(LPCTSTR lpszNewFilter);		// 仅添加一个过滤到列表，不生效
		void UpdateInstanceFilter();	// 刷新过滤，立即生效

		void Clear();	// 手动关闭所有输出

		//////////////////////////////////////////////////////////////////////////
		// 指定输出等级
		void Log(LogLevel level, LPCTSTR pstrFormat, ...);

		//////////////////////////////////////////////////////////////////////////
		// 指定输出等级&分类
		void LogBy(LogLevel level,LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);

		//////////////////////////////////////////////////////////////////////////
		// 输出到默认
		void Debug(LPCTSTR pstrFormat, ... );
		void Info(LPCTSTR pstrFormat, ... );
		void Warn(LPCTSTR pstrFormat, ... );											
		void Error(LPCTSTR pstrFormat, ... );
		void Fatal(LPCTSTR pstrFormat, ... );
		void Post(LPCTSTR pstrFormat, ... );
		//////////////////////////////////////////////////////////////////////////
		// 指定 分类
		void DebugBy(LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);
		void InfoBy(LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);
		void WarnBy(LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);
		void ErrorBy(LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);
		void FatalBy(LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);

		CString FormatString(LPCTSTR pstrFormat, ... );
	private:
		void CleanThread(void);
		SharedAppenderPtr m_SQLiteAppender;

		SharedAppenderPtr GetAppender();
		SharedAppenderPtr GetDebugAppender();
		LogLevel m_DefaultLevel;
		VString m_InstanceFilterList;
		CString m_strLogPath;
		CString m_strPattern;
		CString m_strDebugPattern;
		DWORD m_dwMainThreadID;
	};

#endif // __Log4cplus_H__
