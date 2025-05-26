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

class Logger
{
public:
	enum LogLevel
	{
		Fatal,
		Error,
		Warning,
		Info,
		Debug,
		Trace
	};

	static b8 Initialize();
	static void Terminate();

	KIWI_API static void Output(LogLevel Level, const char *Message, ...);
};

// NOTE: Variadic macros are compiler specific,
// that's why we check for MSVC
#ifdef KIWI_MSVC

#define LogFatal(Message, ...) Logger::Output(Logger::LogLevel::Fatal, (Message), __VA_ARGS__)
#define LogError(Message, ...) Logger::Output(Logger::LogLevel::Error, (Message), __VA_ARGS__)

#if WARNING_LOG_ENABLED
#define LogWarning(Message, ...) Logger::Output(Logger::LogLevel::Warning, (Message), __VA_ARGS__)
#else
#define LogWarning(Message, ...)
#endif

#if INFO_LOG_ENABLED
#define LogInfo(Message, ...) Logger::Output(Logger::LogLevel::Info, (Message), __VA_ARGS__)
#else
#define LogInfo(Message, ...)
#endif

#if DEBUG_LOG_ENABLED
#define LogDebug(Message, ...) Logger::Output(Logger::LogLevel::Debug, (Message), __VA_ARGS__)
#else
#define LogDebug(Message, ...)
#endif

#if TRACE_LOG_ENABLED
#define LogTrace(Message, ...) Logger::Output(Logger::LogLevel::Trace, (Message), __VA_ARGS__)
#else
#define LogTrace(Message, ...)
#endif

#endif