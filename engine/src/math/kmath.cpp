#include "kmath.h"
// TODO:  from math.h?
#include <math.h>

// TODO: Implement a good random
#include "platform/platform.h"
#include <stdlib.h>

static b8 RandomSeeded = false;

// Misc Function
f32 KSqrt(f32 Value) { return sqrtf(Value); }
f32 KAbs(f32 Value) { return fabsf((f32)Value); }

// Trig Function
f32 KSin(f32 AngleInRadiants) { return sinf(AngleInRadiants); }
f32 KCos(f32 AngleInRadiants) { return cosf(AngleInRadiants); }
f32 KTan(f32 AngleInRadiants) { return tanf(AngleInRadiants); }
f32 KAsin(f32 AngleInRadiants) { return asinf(AngleInRadiants); }
f32 KAcos(f32 AngleInRadiants) { return acosf(AngleInRadiants); }
f32 KAtan(f32 AngleInRadiants) { return atanf(AngleInRadiants); }

// Random Function
i32 KRandI()
{
	if (!RandomSeeded)
	{
		srand((u32)Platform::GetAbsoluteTime());
		RandomSeeded = true;
	}
	return rand();
}

i32 KRandI(i32 Min, i32 Max)
{
	return (KRandI() % (Max - Min + 1)) + Min;
}

f32 KRandF()
{
	// NOTE: Result between 0 and 1
	return (f32)KRandI() / (f32)RAND_MAX;
}

f32 KRandF(f32 Min, f32 Max)
{
	return (KRandF() * (Max - Min)) + Min;
}
