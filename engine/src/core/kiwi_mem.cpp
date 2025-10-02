#include "core/kiwi_mem.h"
#include "core/logger.h"
#include "platform/platform.h"

local_var const char *MemTagStrings[MemTag_Count] = {
	"MemTag_Unknown",
	"MemTag_Application",
	"MemTag_Game",
	"MemTag_Array",
	"MemTag_String",
};

u32 MemSystem::PageSize = 0;
u32 MemSystem::AllocationGranularity = 0;
u64 MemSystem::TotalAllocated = 0;
u64 MemSystem::TaggedAllocated[MemTag_Count] = {};

void MemSystem::Initialize()
{
	Platform::GetMemoryInfo(PageSize, AllocationGranularity);
}

void MemSystem::Terminate()
{
	Report();
}

void *MemSystem::Allocate(void *Address, u64 Size, u8 Tag, u32 MemAllocFlags)
{
	if (Tag == MemTag_Unknown)
	{
		LogWarning("Allocation with memory tag Unknown. This should be tagged");
	}

	MemSystem::TotalAllocated += Size;
	MemSystem::TaggedAllocated[Tag] += Size;

	void *MemBlock = Platform::Allocate(Address, Size, MemAllocFlags);

	return MemBlock;
}

void *MemSystem::Allocate(u64 Size, u8 Tag, u32 MemAllocFlags)
{
	return Allocate(nullptr, Size, Tag, MemAllocFlags);
}

void MemSystem::Free(void *Address, u64 Size, u8 Tag, u8 MemDeallocFlag)
{
	if (Tag == MemTag_Unknown)
	{
		LogWarning("Free with memory tag Unknown. This should be tagged");
	}

	MemSystem::TotalAllocated -= Size;
	MemSystem::TaggedAllocated[Tag] -= Size;

	// TODO: Memory allignment
	Platform::Free(Address, Size, MemDeallocFlag);
}

void MemSystem::Set(void *Address, u64 Size, u32 Value)
{
	Platform::SetMem(Address, Size, Value);
}

void MemSystem::Zero(void *Address, u64 Size)
{
	Platform::ZeroMem(Address, Size);
}

void MemSystem::Copy(void *Dest, void *Source, u64 Size)
{
	Platform::CopyMem(Dest, Source, Size);
}

// TODO: Make this actually return a string once we have them
#include <string.h>
#include <stdio.h>
// TODO: Hardocded number matching what the logger espects
#define STRING_LENGTH 3999
char *MemSystem::Report()
{
	// TODO: This function is currently LEAKING MEMORY
	char Buffer[STRING_LENGTH] = "SYSTEM MEMORY USE:\n";
	u32 BufferOffset = (u32)strlen(Buffer);
	for (u32 Index = 0; Index < MemTag_Count; ++Index)
	{
		char Unit[4] = "_iB";
		f32 Amount = 1.0f;
		if (MemSystem::TaggedAllocated[Index] >= GiB(1))
		{
			Unit[0] = 'G';
			Amount = (f32)ToGiB(MemSystem::TaggedAllocated[Index]);
		}
		if (MemSystem::TaggedAllocated[Index] >= MiB(1))
		{
			Unit[0] = 'M';
			Amount = (f32)ToMiB(MemSystem::TaggedAllocated[Index]);
		}
		if (MemSystem::TaggedAllocated[Index] >= KiB(1))
		{
			Unit[0] = 'K';
			Amount = (f32)ToKiB(MemSystem::TaggedAllocated[Index]);
		}
		else
		{
			Unit[0] = 'B';
			Unit[1] = '\0';
			Amount = (f32)MemSystem::TaggedAllocated[Index];
		}
		BufferOffset += snprintf(Buffer + BufferOffset, 3999, "  %s: %.2f%s\n",
								 MemTagStrings[Index], Amount, Unit);
	}

	return _strdup(Buffer);
}