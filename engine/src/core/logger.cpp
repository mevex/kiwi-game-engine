#include "logger.h"

// TODO: Temporary! Remove once we have implemented those ourselves
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 Logger::InitializeLogging()
{
	// TODO: implement file logging
	// TODO: implement platform specific initialization
	return true;
}

void Logger::TerminateLogging()
{
	// TODO: terminate logging/write queue to disk
}

#define LOG_BUFFER_SIZE 4096
KIWI_API void Logger::LogOutput(LogLevel InLevel, const char *InMessage, ...)
{
	// NOTE: I don't use LOG_BUFFER_SIZE to check because I want the extra
	// 96 char to be left spare for the prefix and the \n\0 at the end.
	// Since the limitation will be temporary (hopefully) we don't have to be
	// extra precise here
	Assert(strlen(InMessage) < 4000);
	// TODO: We are imposing a limit to the logging message
	// Address this once we have custom memory allocation (Arenas)
	char Temp[LOG_BUFFER_SIZE];
	memset(Temp, 0, LOG_BUFFER_SIZE);
	char OutMessage[LOG_BUFFER_SIZE];
	memset(Temp, 0, LOG_BUFFER_SIZE);

	const char *LevelPrefix[] = {
		"[FATAL]: ",
		"[ERROR]: ",
		"[WARN]:  ",
		"[INFO]:  ",
		"[DEBUG]: ",
		"",
	};
	// b8 IsError = InLevel < 2;

	va_list ArgsPtr;
	va_start(ArgsPtr, InMessage);
	vsnprintf(Temp, LOG_BUFFER_SIZE, InMessage, ArgsPtr);
	va_end(ArgsPtr);

	snprintf(OutMessage, LOG_BUFFER_SIZE, "%s%s\n", LevelPrefix[InLevel], Temp);

	printf("%s", OutMessage);
}
