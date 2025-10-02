#pragma once

#include "defines.h"

enum MemTag
{
	// TODO: Some temp placeholders, with time we will populate this enum
	MemTag_Unknown,
	MemTag_Application,
	MemTag_Game,
	MemTag_KArray,
	MemTag_Renderer,
	MemTag_String,

	MemTag_Count
};

enum MemAlloc
{
	// Type
	MemAlloc_Commit = 0x1,
	MemAlloc_Reserve = 0x2,
	MemAlloc_Reset = 0x4,
	MemAlloc_ResetUndo = 0x8,

	// Misc type
	MemAlloc_LargePages = 0x10,
	MemAlloc_Physical = 0x20,
	MemAlloc_TopDown = 0x40,
	MemAlloc_WriteWatch = 0x80,

	// Protection
	MemAlloc_Execute = 0x100,
	MemAlloc_ExecuteRead = 0x200,
	MemAlloc_ExecuteReadWrite = 0x400,
	MemAlloc_ExecuteWriteCopy = 0x800,
	MemAlloc_NoAccess = 0x1000,
	MemAlloc_ReadOnly = 0x2000,
	MemAlloc_ReadWrite = 0x4000,
	MemAlloc_WriteCopy = 0x8000,

	// Misc protection
	MemAlloc_Guard = 0x10000,
	MemAlloc_NoCache = 0x20000,
	MemAlloc_WriteCombine = 0x40000,
};

enum MemDealloc
{
	MemDealloc_Decommit = 0x1,
	MemDealloc_Release = 0x2,
};

// NOTE: this is a singleton
class KIWI_API MemSystem
{
public:
	static void Initialize();
	static void Terminate();

	static void *Allocate(void *Address, u64 Size, u8 Tag, u32 MemAllocFlags);
	static void *Allocate(u64 Size, u8 Tag,
						  u32 MemAllocFlags = MemAlloc_Reserve | MemAlloc_Commit | MemAlloc_ReadWrite);
	static void Free(void *Address, u64 Size, u8 Tag,
					 u8 MemDeallocFlag = MemDealloc_Release);
	static void Set(void *Address, u64 Size, u32 Value);
	static void Zero(void *Address, u64 Size);
	static void Copy(void *Dest, void *Source, u64 Size);
	static char *Report();

private:
	static u32 PageSize;
	static u32 AllocationGranularity;
	static u64 TotalAllocated;
	static u64 TaggedAllocated[MemTag_Count];
};