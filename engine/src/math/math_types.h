#pragma once

#include "defines.h"
#include "kmath.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_JUSTIFIED(4201, "nameless struct/union are supported on the compilers we care about using extensions")

struct Vec2;
struct Vec3;
struct Vec4;
struct Mat4;
typedef Vec4 Quat;

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

	Vec2() = default;

	Vec2(const Vec2 &V) = default;

	Vec2 &operator=(const Vec2 &V) = default;

	Vec2(f32 InX, f32 InY)
	{
		x = InX;
		y = InY;
	}

	Vec2(f32 Value)
	{
		x = Value;
		y = Value;
	}

	KIWI_INLINE static Vec2 Zero() { return {0.0f, 0.0f}; }
	KIWI_INLINE static Vec2 One() { return {1.0f, 1.0f}; }
	KIWI_INLINE static Vec2 Up() { return {0.0f, 1.0f}; }
	KIWI_INLINE static Vec2 Down() { return {0.0f, -1.0f}; }
	KIWI_INLINE static Vec2 Left() { return {-1.0f, 0.0f}; }
	KIWI_INLINE static Vec2 Right() { return {1.0f, 0.0f}; }

	KIWI_INLINE f32 LengthSquared();
	KIWI_INLINE f32 Length();
	KIWI_INLINE Vec2 Normalized();
	KIWI_INLINE void Normalize();
	KIWI_INLINE b8 Compare(const Vec2 &V, f32 Tollerance = VERY_SMALL_FLOAT);
	KIWI_INLINE f32 Distance(const Vec2 &V);
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

	Vec3() = default;

	Vec3(const Vec3 &V) = default;

	Vec3 &operator=(const Vec3 &V) = default;

	Vec3(f32 InX, f32 InY, f32 InZ)
	{
		x = InX;
		y = InY;
		z = InZ;
	}

	Vec3(const Vec2 &V, f32 InZ)
	{
		x = V.x;
		y = V.y;
		z = InZ;
	}

	Vec3(f32 Value)
	{
		x = Value;
		y = Value;
		z = Value;
	}

	KIWI_INLINE static Vec3 Zero() { return {0.0f, 0.0f, 0.0f}; }
	KIWI_INLINE static Vec3 One() { return {1.0f, 1.0f, 1.0f}; }
	KIWI_INLINE static Vec3 Up() { return {0.0f, 1.0f, 0.0f}; }
	KIWI_INLINE static Vec3 Down() { return {0.0f, -1.0f, 0.0f}; }
	KIWI_INLINE static Vec3 Left() { return {-1.0f, 0.0f, 0.0f}; }
	KIWI_INLINE static Vec3 Right() { return {1.0f, 0.0f, 0.0f}; }
	KIWI_INLINE static Vec3 Forward() { return {0.0f, 0.0f, -1.0f}; }
	KIWI_INLINE static Vec3 Back() { return {0.0f, 0.0f, 1.0f}; }

	KIWI_INLINE f32 LengthSquared();
	KIWI_INLINE f32 Length();
	KIWI_INLINE Vec3 Normalized();
	KIWI_INLINE void Normalize();
	KIWI_INLINE b8 Compare(const Vec3 &V, f32 Tollerance = VERY_SMALL_FLOAT);
	KIWI_INLINE f32 Distance(const Vec3 &V);
	KIWI_INLINE f32 Dot(const Vec3 &V);
	KIWI_INLINE Vec3 Cross(const Vec3 &V);
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
			f32 _x0;
			Vec2 yz;
			f32 _w0;
		};
		struct
		{
			Vec3 xyz;
			f32 _w1;
		};
		struct
		{
			f32 _x1;
			Vec3 yzw;
		};
	};

	Vec4() = default;

	Vec4(const Vec4 &V) = default;

	Vec4 &operator=(const Vec4 &V) = default;

	Vec4(f32 InX, f32 InY, f32 InZ, f32 InW)
	{
		x = InX;
		y = InY;
		z = InZ;
		w = InW;
	}

	Vec4(const Vec3 &V, f32 InW)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = InW;
	}

	Vec4(const Vec2 &A, const Vec2 &B)
	{
		x = A.x;
		y = A.y;
		z = B.x;
		w = B.y;
	}

	Vec4(f32 Value)
	{
		x = Value;
		y = Value;
		z = Value;
		w = Value;
	}

	KIWI_INLINE static Vec4 Zero() { return {0.0f, 0.0f, 0.0f, 0.0f}; }
	KIWI_INLINE static Vec4 One() { return {1.0f, 1.0f, 1.0f, 1.0f}; }

	KIWI_INLINE f32 LengthSquared();
	KIWI_INLINE f32 Length();
	KIWI_INLINE Vec4 Normalized();
	KIWI_INLINE void Normalize();
	KIWI_INLINE b8 Compare(const Vec4 &V, f32 Tollerance = VERY_SMALL_FLOAT);
	KIWI_INLINE f32 Distance(const Vec4 &V);
	KIWI_INLINE f32 Dot(const Vec4 &V);

	// Quaternion methods
	// TODO: add the quaternion components to Vec4?
	KIWI_INLINE static Quat Identity() { return {0.0f, 0.0f, 0.0f, 1.0f}; }
	KIWI_INLINE static Quat FromAxisAngle(Vec3 Axis, f32 AngleInRadiants, b8 Normalize);
	KIWI_INLINE Quat Slerp(Quat A, Quat B, f32 Percentage);

	KIWI_INLINE f32 Normal() { return Length(); }
	KIWI_INLINE Quat Conjugated();
	KIWI_INLINE void Conjugate();
	KIWI_INLINE Quat Inverse();
	KIWI_INLINE Mat4 ToMat4();
	KIWI_INLINE Mat4 ToMat4WithCenter(Vec3 Center);
};

struct Mat4
{
	f32 e[16];

	Mat4() = default;

	Mat4(const Mat4 &V) = default;

	Mat4 &operator=(const Mat4 &V) = default;

	KIWI_INLINE f32 &operator[](const i32 Index)
	{
		return e[Index];
	}

	KIWI_INLINE static Mat4 Identity();
	KIWI_INLINE static Mat4 Orthographic(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 NearClip, f32 FarClip);
	KIWI_INLINE static Mat4 Perspective(f32 FovRadians, f32 AspectRatio, f32 NearClip, f32 FarClip);
	KIWI_INLINE static Mat4 LookAt(Vec3 Position, Vec3 Target, Vec3 Up);
	KIWI_INLINE static Mat4 Traslation(Vec3 Position);
	KIWI_INLINE static Mat4 Scale(Vec3 Scale);
	KIWI_INLINE static Mat4 EulerX(f32 AngleRadiants);
	KIWI_INLINE static Mat4 EulerY(f32 AngleRadiants);
	KIWI_INLINE static Mat4 EulerZ(f32 AngleRadiants);
	KIWI_INLINE static Mat4 Euler(f32 AngleXRadiants, f32 AngleYRadiants, f32 AngleZRadiants);

	KIWI_INLINE Mat4 Transposed();
	KIWI_INLINE void Transpose();
	KIWI_INLINE Mat4 Inversed();
	KIWI_INLINE void Inverse();
	KIWI_INLINE Vec3 Forward();
	KIWI_INLINE Vec3 Backward();
	KIWI_INLINE Vec3 Up();
	KIWI_INLINE Vec3 Down();
	KIWI_INLINE Vec3 Left();
	KIWI_INLINE Vec3 Right();
};

DISABLE_WARNING_POP