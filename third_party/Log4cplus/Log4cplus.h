#ifndef __Log4cplus_H__
#define __Log4cplus_H__
#pragma once

#include <atlstr.h>
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	Log4cplus.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2013-7-28 0:02:28
// ����޸�	:	2013-7-28 0:02:28
// ˵��			:	Log4cplus��Դ��־��Wrap

ע�⣺
1. Debug��������Ϣͬʱ�����Win32���Կ���̨������DbgView�鿴
2. Release����Ҫ��FileAppender

Instance ����	Ĭ��Ϊroot���޷��޸�ɾ��
ָ����Ϣ�ķ��֧࣬�ֵ�ָ�ʽ�����νṹ
	UI
	UI.MainWnd
	SpeedDetection
	SpeedDetection.UserAuth
SetInstanceFilter()�ӿ�ͨ��Instance��ָ�ʽ�ַ���ƥ������������
����α���뽫ֻ���UI.MainWnd�����ӷ����SpeedDetection��������Ϣ
SetInstanceFilter( UI.MainWnd , SpeedDetection)


˵����
LogLevel		int 	��־��¼�ȼ�����������ڵȼ�����Ϣ
��־ϵͳԤ����ȼ���
ALL_LOG_LEVEL				0
TRACE_LOG_LEVEL			0
DEBUG_LOG_LEVEL			10000
INFO_LOG_LEVEL				20000
WARN_LOG_LEVEL			30000
ERROR_LOG_LEVEL			40000
FATAL_LOG_LEVEL			50000
OFF_LOG_LEVEL				60000

Ϊ�˱�֤OFF_LOG_LEVEL��������Ч���Զ�����־�ȼ���Ҫ�������ֵ

PatternLayout
��һ���дʷ��������ܵ�ģʽ������,����������ʽ���ԡ�%����Ϊ��ͷ������Ԥ�����ʶ��������������ĸ�ʽ��Ϣ��

��1��"%%"��ת��Ϊ% ��
��2��"%c"�����logger���ƣ���test.subtest ��Ҳ���Կ���logger���Ƶ���ʾ��Σ�����"%c{1}"ʱ���"test"���������ֱ�ʾ��Ρ�
��3��"%D"����ʾ����ʱ�䣬���磺"2004-10-16 18:55:45"��%d��ʾ��׼ʱ�䡣
��4��"%F"�������ǰ��¼�����ڵ��ļ����ƣ�����"main.cpp"
��5��"%L"�������ǰ��¼�����ڵ��ļ��кţ�����"51"
��6��"%l"�������ǰ��¼�����ڵ��ļ����ƺ��кţ�����"main.cpp:51"
��7��"%m"�����ԭʼ��Ϣ��
��8��"%n"�����з���
��9��"%p"�����LogLevel������"DEBUG"
��10��"%t"�������¼�����ڵ��߳�ID������ "1075298944"
��11��"%x"��Ƕ�����������NDC (nested diagnostic context) ������Ӷ�ջ�е�����������Ϣ��NDC�����öԲ�ͬԴ��log��Ϣ��ͬʱ�أ���������������֡�
��12����ʽ���룬����"%-10m"ʱ��ʾ����룬�����10����Ȼ�����Ŀ����ַ�Ҳ������ͬ�ķ�ʽ��ʹ�ã�����"%-12d"��"%-5p"�ȵȡ�

ʱ�䶨�壺
			����ͨ��%d{...}�������ϸ����ʾ��ʽ������%d{%H:%M:%s}��ʾҪ��ʾСʱ:���ӣ��롣�������п���ʾ��Ԥ�����ʶ�����£�
			%a -- ��ʾ��ݼ���Ӣ����д��ʽ������"Fri"
			%A -- ��ʾ��ݼ�������"Friday"
			%b -- ��ʾ���·ݣ�Ӣ����д��ʽ������"Oct"
			%B -- ��ʾ���·ݣ�"October"
			%c -- ��׼�����ڣ�ʱ���ʽ���� "Sat Oct 16 18:56:19 2004"
			%d -- ��ʾ����������µļ���(1-31)"16"
			%H -- ��ʾ��ǰʱ���Ǽ�ʱ(0-23)���� "18"
			%I -- ��ʾ��ǰʱ���Ǽ�ʱ(1-12)���� "6"
			%j -- ��ʾ��������һ��(1-366)���� "290"
			%m -- ��ʾ��������һ��(1-12)���� "10"
			%M -- ��ʾ��ǰʱ������һ����(0-59)���� "59"
			%p -- ��ʾ���������绹�����磬 AM or PM
			%q -- ��ʾ��ǰʱ���к��벿��(0-999)���� "237"
			%Q -- ��ʾ��ǰʱ���д�С���ĺ��벿��(0-999.999)���� "430.732"
			%S -- ��ʾ��ǰʱ�̵Ķ�����(0-59)���� "32"
			%U -- ��ʾ�����ǽ���ĵڼ�����ݣ�������Ϊ��һ�쿪ʼ����(0-53)���� "41"
			%w -- ��ʾ��ݼ���(0-6, �����Ϊ0)���� "6"
			%W -- ��ʾ�����ǽ���ĵڼ�����ݣ�����һΪ��һ�쿪ʼ����(0-53)���� "41"
			%x -- ��׼�����ڸ�ʽ���� "10/16/04"
			%X -- ��׼��ʱ���ʽ���� "19:02:34"
			%y -- ��λ�������(0-99)���� "04"
			%Y -- ��λ������ݣ��� "2004"
			%Z -- ʱ���������� "GMT"

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
		void SetInternalDebugging(bool bInternalDebugging=false);	// �Ƿ�����Debug�ȼ���Ϣ
		void SetQuietMode(bool bQuietMode=false);	// �Ƿ������������

		//////////////////////////////////////////////////////////////////////////
		// ����������־ϵͳ֮ǰ������Ч
		void SetPatternString(LPCTSTR lpszPattern);				// ��־��Ϣ��ʽ
		void SetDebugPatternString(LPCTSTR lpszPattern);	// TRACE��Ϣ��ʽ

		LogLevel GetLogLevel();
		CString& GetPatternString();
		CString& GetDebugPatternString();

		/// ������־ϵͳ
		void InitLogger();

		// ���û���Instance�Ĺ���
		// ÿ����������˵������Ϲ�������
		void SetInstanceFilter(VString& filterList);

		void ClearInstanceFilter();		// �����InstanceFilter�б�,����Ч
		void AddInstanceFilter(LPCTSTR lpszNewFilter);		// �����һ�����˵��б�����Ч
		void UpdateInstanceFilter();	// ˢ�¹��ˣ�������Ч

		void Clear();	// �ֶ��ر��������

		//////////////////////////////////////////////////////////////////////////
		// ָ������ȼ�
		void Log(LogLevel level, LPCTSTR pstrFormat, ...);

		//////////////////////////////////////////////////////////////////////////
		// ָ������ȼ�&����
		void LogBy(LogLevel level,LPCTSTR lpInstance,LPCTSTR pstrFormat, ...);

		//////////////////////////////////////////////////////////////////////////
		// �����Ĭ��
		void Debug(LPCTSTR pstrFormat, ... );
		void Info(LPCTSTR pstrFormat, ... );
		void Warn(LPCTSTR pstrFormat, ... );											
		void Error(LPCTSTR pstrFormat, ... );
		void Fatal(LPCTSTR pstrFormat, ... );
		void Post(LPCTSTR pstrFormat, ... );
		//////////////////////////////////////////////////////////////////////////
		// ָ�� ����
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
