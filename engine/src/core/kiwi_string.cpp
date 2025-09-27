#include "kiwi_string.h"
#include "core/kiwi_mem.h"

// TODO: Custom strings one day
#include <string.h>

u64 KStr::Length(const char *Str)
{
	return strlen(Str);
}

char *KStr::Duplicate(const char *Str)
{
	// NOTE: +1 for the null terminator
	u64 Len = KStr::Length(Str) + 1;

	// TODO: This had MakeZeroes set to true
	char *Copy = (char *)MemSystem::Allocate(Len, MemTag_String);
	MemSystem::Copy(Copy, (void *)Str, Len);

	return Copy;
}

b8 KStr::Equal(const char *StrA, const char *StrB)
{
	return strcmp(StrA, StrB) == 0;
}