#include "core/kiwi_mem.h"
#include "core/logger.h"
#include "platform/platform.h"

// MEMORY SYSTEM

local_var const char *MemTagStrings[MemTag_Count] = {
	"MemTag_Unknown",
	"MemTag_Application",
	"MemTag_Game",
	"MemTag_Array",
	"MemTag_String",
};

u32 MemSystem::PageSize = 0;
u32 MemSystem::AllocatorGranularity = 0;
#ifdef KIWI_SLOW
u64 MemSystem::TotalReserved = 0;
u64 MemSystem::TaggedReserved[MemTag_Count] = {};
u64 MemSystem::TotalCommitted = 0;
u64 MemSystem::TaggedCommitted[MemTag_Count] = {};
#endif

void MemSystem::Initialize()
{
	Platform::GetMemoryInfo(PageSize, AllocatorGranularity);
}

void MemSystem::Terminate()
{
	Report();
}

SUPPRESS_WARNING(4100)
void *MemSystem::Allocate(void *Address, u64 Size, u8 Tag, u32 MemAllocFlags)
{
	return nullptr;
}

void *MemSystem::Allocate(u64 Size, u8 Tag, u32 MemAllocFlags)
{
	return Allocate(nullptr, Size, Tag, MemAllocFlags);
}

SUPPRESS_WARNING(4100)
void MemSystem::Free(void *Address, u64 Size, u8 Tag, u8 MemDeallocFlag)
{
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
#if 0
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
#endif
	return (char *)"Not working right now";
}

// MEMORY ARENA

void MemArena::Allocate(u64 Size, u8 Tag)
{
	if (Tag == MemTag_Unknown)
	{
		LogWarning("Creating arena with memory tag Unknown. This should be tagged");
	}

	MemTag = Tag;

	// NOTE: To keep this as platform agnostic as possible, we first reserve the pages
	// and if it succedes we then commit the amount we need to cover the size required.
	// Since this is not expected to happen often, we can afford the double syscall.
	// NOTE: We reserve a multiple of MemSystem::AllocatorGranularity.
	u64 ReserveSize = ((Size / MemSystem::AllocatorGranularity) + 1) * MemSystem::AllocatorGranularity;
	BasePtr = Platform::Allocate(nullptr, ReserveSize, MemAlloc_Reserve | MemAlloc_NoAccess);

	if (!BasePtr)
	{
		LogFatal("Allocation of %d bytes with tag %s failed!", Size, MemTagStrings[Tag]);
		LogFatal(Platform::GetLastErrorMessage());
		KDebugBreak();
		return;
	}

	Platform::Allocate(BasePtr, Size, MemAlloc_Commit | MemAlloc_ReadWrite);

	u32 PagesGranularity = MemSystem::AllocatorGranularity / MemSystem::PageSize;
	u32 CommittedPages = ((u32)Size / MemSystem::PageSize) + 1;
	u32 ReservedPages = ((CommittedPages / PagesGranularity) + 1) * PagesGranularity;

	ReservedMem = ReservedPages * MemSystem::PageSize;
	CommittedMem = CommittedPages * MemSystem::PageSize;

#ifdef KIWI_SLOW
	MemSystem::TotalReserved += ReservedMem;
	MemSystem::TaggedReserved[MemTag] += ReservedMem;
	MemSystem::TotalCommitted += CommittedMem;
	MemSystem::TaggedCommitted[MemTag] += CommittedMem;
#endif
}

void MemArena::Free()
{
	Platform::Free(BasePtr, ReservedMem, MemDealloc_Release);

#ifdef KIWI_SLOW
	MemSystem::TotalReserved -= ReservedMem;
	MemSystem::TaggedReserved[MemTag] -= ReservedMem;
	MemSystem::TotalCommitted -= CommittedMem;
	MemSystem::TaggedCommitted[MemTag] -= CommittedMem;
#endif

	BasePtr = nullptr;
	ReservedMem = 0;
	CommittedMem = 0;
	OccupiedMem = 0;
	MemTag = MemTag_Unknown;
}

void *MemArena::PushNoZero(u64 Size)
{
	u64 NewOccupiedMem = OccupiedMem + Size;
	void *Result = (u8 *)BasePtr + OccupiedMem;

	if (NewOccupiedMem > CommittedMem)
	{
		u64 MemoryToCommit = Size - (CommittedMem - OccupiedMem);
		if ((CommittedMem + MemoryToCommit) < ReservedMem)
		{
			u64 PagesToCommit = (MemoryToCommit / MemSystem::PageSize) + 1;
			u64 MemoryToCommitPageAlligned = (PagesToCommit * MemSystem::PageSize);
			void *CommitAddress = (void *)((u8 *)BasePtr + CommittedMem);
			void *AllocResult = Platform::Allocate(CommitAddress, MemoryToCommitPageAlligned, MemAlloc_Commit | MemAlloc_ReadWrite);

			if (!AllocResult)
			{
				LogError(Platform::GetLastErrorMessage());
				return nullptr;
			}

			CommittedMem += MemoryToCommitPageAlligned;
#ifdef KIWI_SLOW
			MemSystem::TotalCommitted += MemoryToCommitPageAlligned;
			MemSystem::TaggedCommitted[MemTag] += MemoryToCommitPageAlligned;
#endif
		}
		else
		{
			// TODO: We need a strategy for this:
			// - Big contiguos reserve?
			// - Chained Arenas?
			LogFatal("No more Reserved Memory left in arena with tag %s", MemTagStrings[MemTag]);
			KDebugBreak();
			return nullptr;
		}
	}

	OccupiedMem = NewOccupiedMem;
	return Result;
}

void *MemArena::Push(u64 Size)
{
	void *Result = PushNoZero(Size);
	Platform::ZeroMem(Result, Size);
	return Result;
}

void MemArena::Pop(u64 Size)
{
	// TODO: Do we want to decommit the pages at some point
	OccupiedMem -= Size;
}

void MemArena::Clear()
{
	// TODO: Do we want to decommit the pages at some point
	OccupiedMem = 0;
}
