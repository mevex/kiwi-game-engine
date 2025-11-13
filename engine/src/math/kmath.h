#pragma once

#include "defines.h"
#include "math_types.h"

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
#define RadToDeg(Value) (Value) * (180.0 / K_PI)
#define DegToRad(Value) (Value) * (K_PI / 180.0)
#define SecToMs(Value) (Value) * 1000.0f
#define MsToSec(Value) (Value) * 0.001f

template <typename T>
KIWI_INLINE T Max(T A, T B) { return A > B ? A : B; }
template <typename T>
KIWI_INLINE T Min(T A, T B) { return A < B ? A : B; }
template <typename T>
KIWI_INLINE T Clamp(T Value, T Low, T High) { return Max(Min(Value, High), Low); }

KIWI_INLINE b8 IsPowerOfTwo(u64 Value) { return (Value != 0) && ((Value & (Value - 1)) == 0); }

// Misc Function
// TODO: FIX
KIWI_INLINE f32 KSqrt(f32 Value) { return Value; }
KIWI_INLINE f32 KAbs(f32 Value) { return Value; }

// Trig Function
KIWI_INLINE f32 KSin(f32 Angle);
KIWI_INLINE f32 KCos(f32 Angle);
KIWI_INLINE f32 KTan(f32 Angle);
KIWI_INLINE f32 KAsin(f32 Angle);
KIWI_INLINE f32 KAcos(f32 Angle);
KIWI_INLINE f32 KAtan(f32 Angle);

// Random Function
KIWI_INLINE i32 KRandI();
KIWI_INLINE i32 KRandI(i32 Min, i32 Max);
KIWI_INLINE f32 KRandF();
KIWI_INLINE f32 KRandF(f32 Min, f32 Max);

/*
NOTE: Vector 2
*/

// Add
KIWI_INLINE Vec2 operator+(Vec2 A, Vec2 B)
{
	Vec2 Result = {A.x + B.x, A.y + B.y};
	return Result;
}
KIWI_INLINE Vec2 &operator+=(Vec2 &A, Vec2 B)
{
	A = A + B;
	return A;
}

// Subtract
KIWI_INLINE Vec2 operator-(Vec2 A)
{
	Vec2 Result = {-A.x, -A.y};
	return Result;
}
KIWI_INLINE Vec2 operator-(Vec2 A, Vec2 B)
{
	Vec2 Result = {A.x - B.x, A.y - B.y};
	return Result;
}
KIWI_INLINE Vec2 &operator-=(Vec2 &A, Vec2 B)
{
	A = A - B;
	return A;
}

// Multiply by scalar
KIWI_INLINE Vec2 operator*(f32 R, Vec2 A)
{
	Vec2 Result = {R * A.x, R * A.y};
	return Result;
}
KIWI_INLINE Vec2 operator*(Vec2 A, f32 R)
{
	Vec2 Result = R * A;
	return Result;
}
KIWI_INLINE Vec2 &operator*=(Vec2 &A, f32 R)
{
	A = R * A;
	return A;
}

// Member functions
f32 Vec2::Distance(Vec2 V)
{
	Vec2 Result = *this - V;
	return Result.Length();
}

f32 Vec2::LengthSquared()
{
	return (x * x) + (y * y);
}

f32 Vec2::Length()
{
	return KSqrt(LengthSquared());
}

void Vec2::Normalize()
{
	f32 Len = Length();
	x /= Len;
	y /= Len;
}

Vec2 Vec2::Normalized()
{
	Vec2 Result = *this;
	Result.Normalize();
	return Result;
}

b8 Vec2::Compare(Vec2 V, f32 Tollerance = VERY_SMALL_FLOAT)
{
	if (KAbs(x - V.x) > Tollerance)
		return false;
	if (KAbs(y - V.y) > Tollerance)
		return false;

	return true;
}

/*
NOTE: Vector 3
*/

// Add
KIWI_INLINE Vec3 operator+(Vec3 A, Vec3 B)
{
	Vec3 Result = {A.x + B.x, A.y + B.y, A.z + B.z};
	return Result;
}
KIWI_INLINE Vec3 &operator+=(Vec3 &A, Vec3 B)
{
	A = A + B;
	return A;
}

// Subtract
KIWI_INLINE Vec3 operator-(Vec3 A)
{
	Vec3 Result = {-A.x, -A.y, -A.z};
	return Result;
}
KIWI_INLINE Vec3 operator-(Vec3 A, Vec3 B)
{
	Vec3 Result = {A.x - B.x, A.y - B.y, A.z - B.z};
	return Result;
}
KIWI_INLINE Vec3 &operator-=(Vec3 &A, Vec3 B)
{
	A = A - B;
	return A;
}

// Multiply by scalar
KIWI_INLINE Vec3 operator*(f32 R, Vec3 A)
{
	Vec3 Result = {R * A.x, R * A.y, R * A.z};
	return Result;
}
KIWI_INLINE Vec3 operator*(Vec3 A, f32 R)
{
	Vec3 Result = R * A;
	return Result;
}
KIWI_INLINE Vec3 &operator*=(Vec3 &A, f32 R)
{
	A = R * A;
	return A;
}

// Member functions
f32 Vec3::LengthSquared()
{
	return (x * x) + (y * y) + (z * z);
}

f32 Vec3::Length()
{
	return KSqrt(LengthSquared());
}

void Vec3::Normalize()
{
	f32 Len = Length();
	x /= Len;
	y /= Len;
	z /= Len;
}

Vec3 Vec3::Normalized()
{
	Vec3 Result = *this;
	Result.Normalize();
	return Result;
}

b8 Vec3::Compare(Vec3 V, f32 Tollerance = VERY_SMALL_FLOAT)
{
	if (KAbs(x - V.x) > Tollerance)
		return false;
	if (KAbs(y - V.y) > Tollerance)
		return false;
	if (KAbs(z - V.z) > Tollerance)
		return false;

	return true;
}

f32 Vec3::Distance(Vec3 V)
{
	Vec3 Result = *this - V;
	return Result.Length();
}

f32 Vec3::Dot(Vec3 V)
{
	return (x * V.x) + (y * V.y) + (z * V.z);
}

Vec3 Vec3::Cross(Vec3 V)
{
	return {
		y * V.z - z * V.y,
		z * V.x - x * V.z,
		x * V.y - y * V.x,
	};
}

/*
NOTE: Vector 4
*/

// Add
KIWI_INLINE Vec4 operator+(Vec4 A, Vec4 B)
{
	Vec4 Result = {A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w};
	return Result;
}
KIWI_INLINE Vec4 &operator+=(Vec4 &A, Vec4 B)
{
	A = A + B;
	return A;
}

// Subtract
KIWI_INLINE Vec4 operator-(Vec4 A)
{
	Vec4 Result = {-A.x, -A.y, -A.z, -A.w};
	return Result;
}
KIWI_INLINE Vec4 operator-(Vec4 A, Vec4 B)
{
	Vec4 Result = {A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w};
	return Result;
}
KIWI_INLINE Vec4 &operator-=(Vec4 &A, Vec4 B)
{
	A = A - B;
	return A;
}

// Multiply by scalar
KIWI_INLINE Vec4 operator*(f32 R, Vec4 A)
{
	Vec4 Result = {R * A.x, R * A.y, R * A.z, R * A.w};
	return Result;
}
KIWI_INLINE Vec4 operator*(Vec4 A, f32 R)
{
	Vec4 Result = R * A;
	return Result;
}
KIWI_INLINE Vec4 &operator*=(Vec4 &A, f32 R)
{
	A = R * A;
	return A;
}

f32 Vec4::LengthSquared()
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}
f32 Vec4::Length()
{
	return KSqrt(LengthSquared());
}
void Vec4::Normalize()
{
	f32 Len = Length();
	x /= Len;
	y /= Len;
	z /= Len;
	w /= Len;
}
Vec4 Vec4::Normalized()
{
	Vec4 Result = *this;
	Result.Normalize();
	return Result;
}
b8 Vec4::Compare(Vec4 V, f32 Tollerance = VERY_SMALL_FLOAT)
{
	if (KAbs(x - V.x) > Tollerance)
		return false;
	if (KAbs(y - V.y) > Tollerance)
		return false;
	if (KAbs(z - V.z) > Tollerance)
		return false;
	if (KAbs(w - V.w) > Tollerance)
		return false;

	return true;
}

f32 Vec4::Distance(Vec4 V)
{
	Vec4 Result = *this - V;
	return Result.Length();
}

f32 Vec4::Dot(Vec4 V)
{
	return (x * V.x) + (y * V.y) + (z * V.z) + (w * V.w);
}
