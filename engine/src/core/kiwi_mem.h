#pragma once

#include "defines.h"
enum MemTag
{
	// TODO: Some temp placeholders, with time we will populate this enum
	MemTag_Unknown,
	MemTag_Application,
	MemTag_Game,
	MemTag_Array,
	MemTag_String,

	MemTag_Count
};

// NOTE: this is a singleton
class MemSystem
{
public:
	KIWI_API static void Initialize();
	KIWI_API static void Terminate();

	KIWI_API static void *Allocate(u64 Size, MemTag Tag, b8 MakeZero = true);
	KIWI_API static void Free(void *Address, u64 Size, MemTag Tag);
	KIWI_API static void Set(void *Address, u64 Size, u32 Value);
	KIWI_API static void Zero(void *Address, u64 Size);
	KIWI_API static void Copy(void *Dest, void *Source, u64 Size);
	KIWI_API static char *Report();

private:
	static u64 TotalAllocated;
	static u64 TaggedAllocated[MemTag_Count];
};