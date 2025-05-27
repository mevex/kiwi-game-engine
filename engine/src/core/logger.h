#pragma once

#include "defines.h"

// NOTE(valentino): We always want to output fatals and errors
#define WARNING_LOG_ENABLED TRUE
#define INFO_LOG_ENABLED TRUE
#define DEBUG_LOG_ENABLED TRUE
#define TRACE_LOG_ENABLED TRUE

#ifndef KIWI_SLOW
#define DEBUG_LOG_ENABLED FALSE
#define TRACE_LOG_ENABLED FALSE
#endif
enum LogLevel
{
	LogLevel_Fatal,
	LogLevel_Error,
	LogLevel_Warning,
	LogLevel_Info,
	LogLevel_Debug,
	LogLevel_Trace
};

// NOTE: this is a singleton
class Logger
{
public:
	static b8 Initialize();
	static void Terminate();

	KIWI_API static void Output(LogLevel Level, const char *Message, ...);
};

// NOTE: Variadic macros are compiler specific,
// that's why we check for MSVC
#ifdef KIWI_MSVC

#define LogFatal(Message, ...) Logger::Output(LogLevel_Fatal, (Message), __VA_ARGS__)
#define LogError(Message, ...) Logger::Output(LogLevel_Error, (Message), __VA_ARGS__)

#if WARNING_LOG_ENABLED
#define LogWarning(Message, ...) Logger::Output(LogLevel_Warning, (Message), __VA_ARGS__)
#else
#define LogWarning(Message, ...)
#endif

#if INFO_LOG_ENABLED
#define LogInfo(Message, ...) Logger::Output(LogLevel_Info, (Message), __VA_ARGS__)
#else
#define LogInfo(Message, ...)
#endif

#if DEBUG_LOG_ENABLED
#define LogDebug(Message, ...) Logger::Output(LogLevel_Debug, (Message), __VA_ARGS__)
#else
#define LogDebug(Message, ...)
#endif

#if TRACE_LOG_ENABLED
#define LogTrace(Message, ...) Logger::Output(LogLevel_Trace, (Message), __VA_ARGS__)
#else
#define LogTrace(Message, ...)
#endif

#endif