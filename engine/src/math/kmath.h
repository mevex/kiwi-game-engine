#pragma once

#include "defines.h"

// Trig
#define K_PI 3.14159265358979323846f
#define K_2PI 2.0f * K_PI
#define K_HPI 0.5f * K_PI
#define K_QPI 0.25f * K_PI
#define K_1_OVER_PI 1.0f / K_PI
#define K_1_OVER_2PI 1.0f / K_PI_2

// Sqrt
#define K_SQRT_2 1.41421356237309504880f
#define K_SQRT_3 1.73205080756887729352f
#define K_SQRT_1_OVER_2 0.70710678118654752440f
#define K_SQRT_1_OVER_3 0.57735026918962576450f

// Misc
#define VERY_SMALL_FLOAT 1.192092896e-07f
#define INFINITY_FLOAT 1e30f

// Utility Functions
#define DegFromRad(Value) (Value) * (180.0 / K_PI)
#define RadFromDeg(Value) (Value) * (K_PI / 180.0)
#define MsFromSec(Value) (Value) * 1000.0f
#define SecFromMs(Value) (Value) * 0.001f

KIWI_INLINE b8 IsPowerOfTwo(u64 Value) { return (Value != 0) && ((Value & (Value - 1)) == 0); }

// Misc Function
KIWI_INLINE f32 KSqrt(f32 Value);
KIWI_INLINE f32 KAbs(f32 Value);

// Trig Function
KIWI_INLINE f32 KSin(f32 AngleInRadiants);
KIWI_INLINE f32 KCos(f32 AngleInRadiants);
KIWI_INLINE f32 KTan(f32 AngleInRadiants);
KIWI_INLINE f32 KAsin(f32 AngleInRadiants);
KIWI_INLINE f32 KAcos(f32 AngleInRadiants);
KIWI_INLINE f32 KAtan(f32 AngleInRadiants);

// Random Function
KIWI_INLINE i32 KRandI();
KIWI_INLINE i32 KRandI(i32 Min, i32 Max);
KIWI_INLINE f32 KRandF();
KIWI_INLINE f32 KRandF(f32 Min, f32 Max);
