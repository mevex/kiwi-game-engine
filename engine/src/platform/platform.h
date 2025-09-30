#pragma once

#include "defines.h"

struct PlatformState
{
	void *InternalState;
};

namespace Platform
{
	b8 Startup(PlatformState *PlatState, const char *ApplicationName, i32 X, i32 Y, i32 Width, i32 Height);
	void Terminate(PlatformState *PlatState);

	b8 ProcessMessageQueue(PlatformState *PlatState);

	void GetMemoryInfo(u32 &OutPageSize, u32 &OutAllocationGranularity);
	void TranslateAllocSpecifiers(u32 MemAllocFlags, u32 &OutAllocType, u32 &OutProtectionType);
	void *Allocate(u64 Size, u32 MemAllocFlags);
	void Free(void *Address, u64 Size, u8 MemDeallocFlag);
	void SetMem(void *Address, u64 Size, u32 Value);
	void ZeroMem(void *Address, u64 Size);
	void CopyMem(void *Dest, void *Source, u64 Size);

	void ConsoleWrite(const char *Message, u8 Level);
	void ConsoleWriteError(const char *Message, u8 Level);

	f64 GetAbsoluteTime();

	void SleepMS(u64 ms);
}