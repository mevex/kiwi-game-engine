#pragma once

#include "defines.h"

namespace Platform
{
	struct State
	{
		void *InternalState;
	};

	KIWI_API b8 Startup(State *PlatState, const char *ApplicationName, i32 X, i32 Y, i32 Width, i32 Height);
	KIWI_API void Terminate(State *PlatState);

	KIWI_API void ProcessMessageQueue(State *PlatState);

	void *Allocate(u64 Size, b8 Alligned);
	void Free(void *Address, b8 Alligned);
	void SetMem(void *Address, u64 Size, u32 Value);
	void ZeroMem(void *Address, u64 Size);
	void CopyMem(void *Dest, void *Source, u64 Size);

	void ConsoleWrite(const char *Message, u8 Level);
	void ConsoleWriteError(const char *Message, u8 Level);

	f64 GetAbsoluteTime();

	KIWI_API void SleepMS(u64 ms);
}