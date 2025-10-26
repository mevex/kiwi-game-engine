#include "kiwi_string.h"
#include "core/kiwi_mem.h"

// TODO: Custom strings one day
#include <string.h>

u64 KStr::Length(const char *Str)
{
	return strlen(Str);
}

// TODO: Fix this
char *KStr::Duplicate(const char *Str)
{
	KDebugBreak();
#if 0
	// NOTE: +1 for the null terminator
	u64 Len = KStr::Length(Str) + 1;

	char *Copy = (char *)MemSystem::Allocate(Len, MemTag_String);
	MemSystem::Copy(Copy, (void *)Str, Len);

	return Copy;
#else
	Str;
	return (char *)"";
#endif
}

b8 KStr::Equal(const char *StrA, const char *StrB)
{
	return strcmp(StrA, StrB) == 0;
}