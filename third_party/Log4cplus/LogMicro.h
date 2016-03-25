#ifndef Log4cplus_h__
#define Log4cplus_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	LogMicro.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-6-11 16:04:42
// ����޸�	:	2014-6-11 16:04:42
// ˵��		:	��־ϵͳ��
//*////////////////////////////////////////////////////////////
#pragma once
#include "log4cplus/loggingmacros.h"
#include "log4cplus/logger.h"

using namespace log4cplus;
using namespace log4cplus::helpers;

#ifndef TRACE
#define TRACE(fmt,...) LOG4CPLUS_TRACE_FMT(log4cplus::Logger::getRoot(),fmt,__VA_ARGS__)
#endif

#ifndef _TRACE
#define _TRACE(fmt,...) LOG4CPLUS_TRACE_FMT(log4cplus::Logger::getRoot(),fmt,__VA_ARGS__)
#endif

#ifndef DEBUG
#define DEBUG(fmt,...) LOG4CPLUS_DEBUG_FMT(log4cplus::Logger::getRoot(), fmt,__VA_ARGS__)  
#endif


#define NOTICE(fmt,...) LOG4CPLUS_INFO_FMT(log4cplus::Logger::getRoot(), fmt,__VA_ARGS__)  
#define WARNING(fmt,...) LOG4CPLUS_WARN_FMT(log4cplus::Logger::getRoot(), fmt,__VA_ARGS__)  
#define FATAL(fmt,...) LOG4CPLUS_ERROR_FMT(log4cplus::Logger::getRoot(), fmt,__VA_ARGS__)
#endif // Log4cplus_h__
