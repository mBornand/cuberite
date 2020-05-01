
// Logging free functions defined in Logger.cpp
#pragma once

#include "Logger.h"

// python style format specified logging

template <typename... Args>
extern void FLOG(const char * a_Format, const Args & ... a_ArgList)
{
	cLogger::GetInstance().LogFormat(a_Format, cLogger::llRegular, a_ArgList...);
}

template <typename... Args>
extern void FLOGINFO(const char * a_Format, const Args & ... a_ArgList)
{
	cLogger::GetInstance().LogFormat(a_Format, cLogger::llInfo, a_ArgList...);
}

template <typename... Args>
extern void FLOGWARNING(const char * a_Format, const Args & ... a_ArgList)
{
	cLogger::GetInstance().LogFormat(a_Format, cLogger::llWarning, a_ArgList...);
}

template <typename... Args>
extern void FLOGERROR(const char * a_Format, const Args & ... a_ArgList)
{
	cLogger::GetInstance().LogFormat(a_Format, cLogger::llError, a_ArgList...);
}

// printf style format specified logging (DEPRECATED)

template <typename... Args>
extern void LOG(const char * a_Format, const Args & ... a_ArgList)
{
	cLogger::GetInstance().LogPrintf(a_Format, cLogger::llRegular, a_ArgList...);
}

template <typename... Args>
extern void LOGINFO(const char * a_Format, const Args & ... a_ArgList)
{
	//cLogger::GetInstance().LogPrintf(a_Format, cLogger::llInfo, a_ArgList...);
}

template <typename... Args>
extern void LOGWARNING(const char * a_Format, const Args & ... a_ArgList)
{
	//cLogger::GetInstance().LogPrintf(a_Format, cLogger::llWarning, a_ArgList...);
}

template <typename... Args>
extern void LOGERROR(const char * a_Format, const Args & ... a_ArgList)
{
	cLogger::GetInstance().LogPrintf(a_Format, cLogger::llError, a_ArgList...);
}


// Macro variants

// In debug builds, translate LOGD to LOG, otherwise leave it out altogether:
#ifdef _DEBUG
	#define LOGD LOG
#else
	#define LOGD(...)
#endif  // _DEBUG

#define LOGWARN LOGWARNING

#ifdef _DEBUG
	#define FLOGD FLOG
#else
	#define FLOGD(...)
#endif  // _DEBUG

#define FLOGWARN FLOGWARNING
