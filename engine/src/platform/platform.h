#pragma once

#include "defines.h"

namespace Platform
{
	struct State
	{
		void *InternalState;
	};

	b8 Startup(State *PlatState, const char *ApplicationName, i32 X, i32 Y, i32 Width, i32 Height);
	void Terminate(State *PlatState);

	b8 ProcessMessageQueue(State *PlatState);

	KIWI_API void *Allocate(u64 Size, b8 Alligned = false);
	void Free(void *Address, b8 Alligned = false);
	void SetMem(void *Address, u64 Size, u32 Value);
	void ZeroMem(void *Address, u64 Size);
	void CopyMem(void *Dest, void *Source, u64 Size);

	void ConsoleWrite(const char *Message, u8 Level);
	void ConsoleWriteError(const char *Message, u8 Level);

	f64 GetAbsoluteTime();

	void SleepMS(u64 ms);
}