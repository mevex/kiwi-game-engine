#pragma once

#include "defines.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_JUSTIFIED(4201, "nameless struct/union are supported on the compilers we care about using extensions")

struct Vec2
{
	union
	{
		f32 e[2];

		struct
		{
			f32 x, y;
		};
		struct
		{
			f32 u, v;
		};
	};

	Vec2(f32 InX, f32 InY)
	{
		x = InX;
		y = InY;
	}

	// Utility
	KIWI_INLINE Vec2 Zero() { return {0.0f, 0.0f}; }
	KIWI_INLINE Vec2 One() { return {1.0f, 1.0f}; }
	KIWI_INLINE Vec2 Up() { return {0.0f, 1.0f}; }
	KIWI_INLINE Vec2 Down() { return {0.0f, -1.0f}; }
	KIWI_INLINE Vec2 Left() { return {-1.0f, 0.0f}; }
	KIWI_INLINE Vec2 Right() { return {1.0f, 0.0f}; }

	KIWI_INLINE f32 LengthSquared();
	KIWI_INLINE f32 Length();
	KIWI_INLINE void Normalize();
	KIWI_INLINE Vec2 Normalized();
	KIWI_INLINE b8 Compare(Vec2 V, f32 Tollerance);
	KIWI_INLINE f32 Distance(Vec2 V);
};

struct Vec3
{
	union
	{
		f32 e[3];

		struct
		{
			f32 x, y, z;
		};
		struct
		{
			f32 u, v, w;
		};
		struct
		{
			f32 r, g, b;
		};
		struct
		{
			Vec2 xy;
			f32 _z;
		};
		struct
		{
			f32 _x;
			Vec2 yz;
		};
	};

	Vec3(f32 InX, f32 InY, f32 InZ)
	{
		x = InX;
		y = InY;
		z = InZ;
	}

	Vec3(Vec2 V, f32 InZ)
	{
		x = V.x;
		y = V.y;
		z = InZ;
	}

	// Utility
	KIWI_INLINE Vec3 Zero() { return {0.0f, 0.0f, 0.0f}; }
	KIWI_INLINE Vec3 One() { return {1.0f, 1.0f, 1.0f}; }
	KIWI_INLINE Vec3 Up() { return {0.0f, 1.0f, 0.0f}; }
	KIWI_INLINE Vec3 Down() { return {0.0f, -1.0f, 0.0f}; }
	KIWI_INLINE Vec3 Left() { return {-1.0f, 0.0f, 0.0f}; }
	KIWI_INLINE Vec3 Right() { return {1.0f, 0.0f, 0.0f}; }
	KIWI_INLINE Vec3 Forward() { return {0.0f, 0.0f, -1.0f}; }
	KIWI_INLINE Vec3 Back() { return {0.0f, 0.0f, 1.0f}; }

	KIWI_INLINE f32 LengthSquared();
	KIWI_INLINE f32 Length();
	KIWI_INLINE void Normalize();
	KIWI_INLINE Vec3 Normalized();
	KIWI_INLINE b8 Compare(Vec3 V, f32 Tollerance);
	KIWI_INLINE f32 Distance(Vec3 V);
	KIWI_INLINE f32 Dot(Vec3 V);
	KIWI_INLINE Vec3 Cross(Vec3 V);
};

struct Vec4
{
	union
	{
		f32 e[3];

		struct
		{
			f32 x, y, z, w;
		};
		struct
		{
			f32 r, g, b, a;
		};
		struct
		{
			Vec2 xy;
			Vec2 zw;
		};
		struct
		{
			f32 _x;
			Vec2 yz;
			f32 _w;
		};
		struct
		{
			Vec3 xyz;
			f32 _w;
		};
		struct
		{
			f32 _x;
			Vec3 yzw;
		};
	};

	Vec4(f32 InX, f32 InY, f32 InZ, f32 InW)
	{
		x = InX;
		y = InY;
		z = InZ;
		w = InW;
	}

	Vec4(Vec3 V, f32 InW)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = InW;
	}

	// Utility
	KIWI_INLINE Vec4 Zero() { return {0.0f, 0.0f, 0.0f, 0.0f}; }
	KIWI_INLINE Vec4 One() { return {1.0f, 1.0f, 1.0f, 1.0f}; }

	KIWI_INLINE f32 LengthSquared();
	KIWI_INLINE f32 Length();
	KIWI_INLINE void Normalize();
	KIWI_INLINE Vec4 Normalized();
	KIWI_INLINE b8 Compare(Vec4 V, f32 Tollerance);
	KIWI_INLINE f32 Distance(Vec4 V);
	KIWI_INLINE f32 Dot(Vec4 V);
};

DISABLE_WARNING_POP