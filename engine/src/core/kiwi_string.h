#pragma once

#include "defines.h"

namespace KStr
{
	// return the length of the given string.
	KIWI_API u64 Length(const char *Str);

	KIWI_API char *Duplicate(const char *Str);

	// Case sensitive string comparison
	KIWI_API b8 Equal(const char *StrA, const char *StrB);
}