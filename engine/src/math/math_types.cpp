#include "math_types.h"

/*
NOTE: Vector 2
*/

// Add
KIWI_INLINE Vec2 operator+(const Vec2 &A, const Vec2 &B)
{
	return {A.x + B.x, A.y + B.y};
}
KIWI_INLINE Vec2 &operator+=(Vec2 &A, const Vec2 &B)
{
	A = A + B;
	return A;
}

// Subtract
KIWI_INLINE Vec2 operator-(const Vec2 &A)
{
	return {-A.x, -A.y};
}
KIWI_INLINE Vec2 operator-(const Vec2 &A, const Vec2 &B)
{
	return {A.x - B.x, A.y - B.y};
}
KIWI_INLINE Vec2 &operator-=(Vec2 &A, const Vec2 &B)
{
	A = A - B;
	return A;
}

// Multiply by scalar
KIWI_INLINE Vec2 operator*(f32 R, Vec2 &A)
{
	return {R * A.x, R * A.y};
}
KIWI_INLINE Vec2 operator*(Vec2 &A, f32 R)
{
	return R * A;
}
KIWI_INLINE Vec2 &operator*=(Vec2 &A, f32 R)
{
	A = R * A;
	return A;
}

// Member functions
f32 Vec2::Distance(const Vec2 &V)
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

Vec2 Vec2::Normalized()
{
	Vec2 Result;

	f32 Len = Length();
	Result.x = x / Len;
	Result.y = y / Len;

	return Result;
}

void Vec2::Normalize()
{
	*this = Normalized();
}

b8 Vec2::Compare(const Vec2 &V, f32 Tollerance)
{
	return KAbs(x - V.x) < Tollerance &&
		   KAbs(y - V.y) < Tollerance;
}

/*
NOTE: Vector 3
*/

// Add
KIWI_INLINE Vec3 operator+(const Vec3 &A, const Vec3 &B)
{
	return {A.x + B.x, A.y + B.y, A.z + B.z};
}
KIWI_INLINE Vec3 &operator+=(Vec3 &A, const Vec3 &B)
{
	A = A + B;
	return A;
}

// Subtract
KIWI_INLINE Vec3 operator-(const Vec3 &A)
{
	return {-A.x, -A.y, -A.z};
}
KIWI_INLINE Vec3 operator-(const Vec3 &A, const Vec3 &B)
{
	return {A.x - B.x, A.y - B.y, A.z - B.z};
}
KIWI_INLINE Vec3 &operator-=(Vec3 &A, const Vec3 &B)
{
	A = A - B;
	return A;
}

// Multiply by scalar
KIWI_INLINE Vec3 operator*(f32 R, const Vec3 &A)
{
	return {R * A.x, R * A.y, R * A.z};
}
KIWI_INLINE Vec3 operator*(const Vec3 &A, f32 R)
{
	return R * A;
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

Vec3 Vec3::Normalized()
{
	Vec3 Result;

	f32 Len = Length();
	Result.x = x / Len;
	Result.y = y / Len;
	Result.z = z / Len;

	return Result;
}

void Vec3::Normalize()
{
	*this = Normalized();
}

b8 Vec3::Compare(const Vec3 &V, f32 Tollerance)
{
	return KAbs(x - V.x) < Tollerance &&
		   KAbs(y - V.y) < Tollerance &&
		   KAbs(z - V.z) < Tollerance;
}

f32 Vec3::Distance(const Vec3 &V)
{
	Vec3 Result = *this - V;
	return Result.Length();
}

f32 Vec3::Dot(const Vec3 &V)
{
	return (x * V.x) + (y * V.y) + (z * V.z);
}

Vec3 Vec3::Cross(const Vec3 &V)
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
KIWI_INLINE Vec4 operator+(const Vec4 &A, const Vec4 &B)
{
	return {A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w};
}
KIWI_INLINE Vec4 &operator+=(Vec4 &A, const Vec4 &B)
{
	A = A + B;
	return A;
}

// Subtract
KIWI_INLINE Vec4 operator-(const Vec4 &A)
{
	return {-A.x, -A.y, -A.z, -A.w};
}
KIWI_INLINE Vec4 operator-(const Vec4 &A, const Vec4 &B)
{
	return {A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w};
}
KIWI_INLINE Vec4 &operator-=(Vec4 &A, Vec4 B)
{
	A = A - B;
	return A;
}

// Multiply by scalar
KIWI_INLINE Vec4 operator*(f32 R, const Vec4 &A)
{
	return {R * A.x, R * A.y, R * A.z, R * A.w};
}
KIWI_INLINE Vec4 operator*(const Vec4 &A, f32 R)
{
	return R * A;
}
KIWI_INLINE Vec4 &operator*=(Vec4 &A, f32 R)
{
	A = R * A;
	return A;
}

// Member functions
f32 Vec4::LengthSquared()
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

f32 Vec4::Length()
{
	return KSqrt(LengthSquared());
}

Vec4 Vec4::Normalized()
{
	Vec4 Result;

	f32 Len = Length();
	Result.x = x / Len;
	Result.y = y / Len;
	Result.z = z / Len;
	Result.w = w / Len;

	return Result;
}

void Vec4::Normalize()
{
	*this = Normalized();
}

b8 Vec4::Compare(const Vec4 &V, f32 Tollerance)
{
	return KAbs(x - V.x) < Tollerance &&
		   KAbs(y - V.y) < Tollerance &&
		   KAbs(z - V.z) < Tollerance &&
		   KAbs(w - V.w) < Tollerance;
}

f32 Vec4::Distance(const Vec4 &V)
{
	Vec4 Result = *this - V;
	return Result.Length();
}

f32 Vec4::Dot(const Vec4 &V)
{
	return (x * V.x) + (y * V.y) + (z * V.z) + (w * V.w);
}

/*
NOTE: Mat4
*/

KIWI_INLINE Mat4 operator*(const Mat4 &A, const Mat4 &B)
{
	Mat4 Result = Mat4::Identity();

	const f32 *PtrA = A.e;
	const f32 *PtrB = B.e;
	f32 *PtrC = Result.e;

	for (i32 i = 0; i < 4; ++i)
	{
		for (i32 j = 0; j < 4; ++j)
		{
			*PtrC =
				PtrA[0] * PtrB[0 + j] +
				PtrA[1] * PtrB[4 + j] +
				PtrA[2] * PtrB[8 + j] +
				PtrA[3] * PtrB[12 + j];
			PtrC++;
		}
		PtrA += 4;
	}

	return Result;
}

Mat4 Mat4::Identity()
{
	Mat4 Result = {};
	Result[0] = 1.0f;
	Result[5] = 1.0f;
	Result[10] = 1.0f;
	Result[15] = 1.0f;
	return Result;
}

Mat4 Mat4::Orthographic(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 NearClip, f32 FarClip)
{
	Mat4 Result = Mat4::Identity();

	f32 LR = 1.0f / (Left - Right);
	f32 BT = 1.0f / (Bottom - Top);
	f32 NF = 1.0f / (NearClip - FarClip);

	Result[0] = -2.0f * LR;
	Result[5] = -2.0f * BT;
	Result[10] = 2.0f * NF;

	Result[12] = (Left + Right) * LR;
	Result[13] = (Top + Bottom) * BT;
	Result[14] = (FarClip + NearClip) * NF;

	return Result;
}

Mat4 Mat4::Perspective(f32 FovRadians, f32 AspectRatio, f32 NearClip, f32 FarClip)
{
	Mat4 Result = {};

	f32 HalfTanFov = KTan(FovRadians * 0.5f);
	Result[0] = 1.0f / (AspectRatio * HalfTanFov);
	Result[5] = 1.0f / HalfTanFov;
	Result[10] = -((FarClip + NearClip) / (FarClip - NearClip));
	Result[11] = -1.0f;
	Result[14] = -((2.0f * FarClip * NearClip) / (FarClip - NearClip));

	return Result;
}

Mat4 Mat4::LookAt(Vec3 Position, Vec3 Target, Vec3 Up)
{
	Mat4 Result;

	Vec3 ZAxis = Vec3(Target.x - Position.x, Target.y - Position.y, Target.z - Position.z).Normalized();
	Vec3 XAxis = ZAxis.Cross(Up).Normalized();
	Vec3 YAxis = XAxis.Cross(ZAxis);

	Result[0] = XAxis.x;
	Result[1] = YAxis.x;
	Result[2] = ZAxis.x;
	Result[3] = 0.0f;
	Result[4] = XAxis.y;
	Result[5] = YAxis.y;
	Result[6] = ZAxis.y;
	Result[7] = 0.0f;
	Result[8] = XAxis.z;
	Result[9] = YAxis.z;
	Result[10] = ZAxis.z;
	Result[11] = 0.0f;
	Result[12] = -XAxis.Dot(Position);
	Result[13] = -YAxis.Dot(Position);
	Result[14] = -ZAxis.Dot(Position);
	Result[15] = 1.0f;

	return Result;
}

Mat4 Mat4::Traslation(Vec3 Position)
{
	Mat4 Result = Mat4::Identity();
	Result[12] = Position.x;
	Result[13] = Position.y;
	Result[14] = Position.z;
	return Result;
}

Mat4 Mat4::Scale(Vec3 Scale)
{
	Mat4 Result = Mat4::Identity();
	Result[0] = Scale.x;
	Result[5] = Scale.y;
	Result[10] = Scale.z;
	return Result;
}
Mat4 Mat4::EulerX(f32 AngleRadiants)
{
	Mat4 Result = Mat4::Identity();

	f32 c = KCos(AngleRadiants);
	f32 s = KSin(AngleRadiants);

	Result[5] = c;
	Result[6] = s;
	Result[9] = -s;
	Result[10] = c;

	return Result;
}

Mat4 Mat4::EulerY(f32 AngleRadiants)
{
	Mat4 Result = Mat4::Identity();

	f32 c = KCos(AngleRadiants);
	f32 s = KSin(AngleRadiants);

	Result[0] = c;
	Result[2] = -s;
	Result[8] = s;
	Result[10] = c;

	return Result;
}

Mat4 Mat4::EulerZ(f32 AngleRadiants)
{
	Mat4 Result = Mat4::Identity();

	f32 c = KCos(AngleRadiants);
	f32 s = KSin(AngleRadiants);

	Result[0] = c;
	Result[1] = s;
	Result[4] = -s;
	Result[5] = c;

	return Result;
}

Mat4 Mat4::Euler(f32 AngleXRadiants, f32 AngleYRadiants, f32 AngleZRadiants)
{
	Mat4 RX = Mat4::EulerX(AngleXRadiants);
	Mat4 RY = Mat4::EulerY(AngleYRadiants);
	Mat4 RZ = Mat4::EulerZ(AngleZRadiants);

	Mat4 Result = RX * RY * RZ;
	return Result;
}

Mat4 Mat4::Transposed()
{
	Mat4 Result = Mat4::Identity();

	Result[0] = e[0];
	Result[1] = e[4];
	Result[2] = e[8];
	Result[3] = e[12];
	Result[4] = e[1];
	Result[5] = e[5];
	Result[6] = e[9];
	Result[7] = e[13];
	Result[8] = e[2];
	Result[9] = e[6];
	Result[10] = e[10];
	Result[11] = e[14];
	Result[12] = e[3];
	Result[13] = e[7];
	Result[14] = e[11];
	Result[15] = e[15];

	return Result;
}

void Mat4::Transpose()
{
	*this = Transposed();
}

Mat4 Mat4::Inversed()
{
	f32 t[23];
	t[0] = e[10] * e[15];
	t[1] = e[14] * e[11];
	t[2] = e[6] * e[15];
	t[3] = e[14] * e[7];
	t[4] = e[6] * e[11];
	t[5] = e[10] * e[7];
	t[6] = e[2] * e[15];
	t[7] = e[14] * e[3];
	t[8] = e[2] * e[11];
	t[9] = e[10] * e[3];
	t[10] = e[2] * e[7];
	t[11] = e[6] * e[3];
	t[12] = e[8] * e[13];
	t[13] = e[12] * e[9];
	t[14] = e[4] * e[13];
	t[15] = e[12] * e[5];
	t[16] = e[4] * e[9];
	t[17] = e[8] * e[5];
	t[18] = e[0] * e[13];
	t[19] = e[12] * e[1];
	t[20] = e[0] * e[9];
	t[21] = e[8] * e[1];
	t[22] = e[0] * e[5];
	t[23] = e[4] * e[1];

	Mat4 Res;

	Res[0] = (t[0] * e[5] + t[3] * e[9] + t[4] * e[13]) - (t[1] * e[5] + t[2] * e[9] + t[5] * e[13]);
	Res[1] = (t[1] * e[1] + t[6] * e[9] + t[9] * e[13]) - (t[0] * e[1] + t[7] * e[9] + t[8] * e[13]);
	Res[2] = (t[2] * e[1] + t[7] * e[5] + t[10] * e[13]) - (t[3] * e[1] + t[6] * e[5] + t[11] * e[13]);
	Res[3] = (t[5] * e[1] + t[8] * e[5] + t[11] * e[9]) - (t[4] * e[1] + t[9] * e[5] + t[10] * e[9]);

	f32 d = 1.0f / (e[0] * Res[0] + e[4] * Res[1] + e[8] * Res[2] + e[12] * Res[3]);

	Res[0] = d * Res[0];
	Res[1] = d * Res[1];
	Res[2] = d * Res[2];
	Res[3] = d * Res[3];
	Res[4] = d * ((t[1] * e[4] + t[2] * e[8] + t[5] * e[12]) - (t[0] * e[4] + t[3] * e[8] + t[4] * e[12]));
	Res[5] = d * ((t[0] * e[0] + t[7] * e[8] + t[8] * e[12]) - (t[1] * e[0] + t[6] * e[8] + t[9] * e[12]));
	Res[6] = d * ((t[3] * e[0] + t[6] * e[4] + t[11] * e[12]) - (t[2] * e[0] + t[7] * e[4] + t[10] * e[12]));
	Res[7] = d * ((t[4] * e[0] + t[9] * e[4] + t[10] * e[8]) - (t[5] * e[0] + t[8] * e[4] + t[11] * e[8]));
	Res[8] = d * ((t[12] * e[7] + t[15] * e[11] + t[16] * e[15]) - (t[13] * e[7] + t[14] * e[11] + t[17] * e[15]));
	Res[9] = d * ((t[13] * e[3] + t[18] * e[11] + t[21] * e[15]) - (t[12] * e[3] + t[19] * e[11] + t[20] * e[15]));
	Res[10] = d * ((t[14] * e[3] + t[19] * e[7] + t[22] * e[15]) - (t[15] * e[3] + t[18] * e[7] + t[23] * e[15]));
	Res[11] = d * ((t[17] * e[3] + t[20] * e[7] + t[23] * e[11]) - (t[16] * e[3] + t[21] * e[7] + t[22] * e[11]));
	Res[12] = d * ((t[14] * e[10] + t[17] * e[14] + t[13] * e[6]) - (t[16] * e[14] + t[12] * e[6] + t[15] * e[10]));
	Res[13] = d * ((t[20] * e[14] + t[12] * e[2] + t[19] * e[10]) - (t[18] * e[10] + t[21] * e[14] + t[13] * e[2]));
	Res[14] = d * ((t[18] * e[6] + t[23] * e[14] + t[15] * e[2]) - (t[22] * e[14] + t[14] * e[2] + t[19] * e[6]));
	Res[15] = d * ((t[22] * e[10] + t[16] * e[2] + t[21] * e[6]) - (t[20] * e[6] + t[23] * e[10] + t[17] * e[2]));

	return Res;
}

void Mat4::Inverse()
{
	*this = Inversed();
}

Vec3 Mat4::Forward()
{
	Vec3 Result;

	Result.x = -e[2];
	Result.y = -e[6];
	Result.z = -e[10];
	Result.Normalize();

	return Result;
}

Vec3 Mat4::Backward()
{
	Vec3 Result;

	Result.x = e[2];
	Result.y = e[6];
	Result.z = e[10];
	Result.Normalize();

	return Result;
}

Vec3 Mat4::Up()
{
	Vec3 Result;

	Result.x = e[1];
	Result.y = e[5];
	Result.z = e[9];
	Result.Normalize();

	return Result;
}

Vec3 Mat4::Down()
{
	Vec3 Result;

	Result.x = -e[1];
	Result.y = -e[5];
	Result.z = -e[9];
	Result.Normalize();

	return Result;
}

Vec3 Mat4::Left()
{
	Vec3 Result;

	Result.x = -e[0];
	Result.y = -e[4];
	Result.z = -e[12];
	Result.Normalize();

	return Result;
}

Vec3 Mat4::Right()
{
	Vec3 Result;

	Result.x = e[0];
	Result.y = e[4];
	Result.z = e[12];
	Result.Normalize();

	return Result;
}

/*
NOTE: Quaternion
*/

KIWI_INLINE Quat operator*(const Quat &A, const Quat &B)
{
	Quat Result;

	Result.x = A.x * B.w + A.y * B.z - A.z * B.y + A.w * B.x;
	Result.y = -A.x * B.z + A.y * B.w + A.z * B.x + A.w * B.y;
	Result.z = A.x * B.y - A.y * B.x + A.z * B.w + A.w * B.z;
	Result.w = -A.x * B.x - A.y * B.y - A.z * B.z + A.w * B.w;

	return Result;
}

Quat Quat::FromAxisAngle(Vec3 Axis, f32 AngleInRadiants, b8 Normalize)
{
	const f32 HalfAngle = 0.5f * AngleInRadiants;
	const f32 s = KSin(HalfAngle);
	const f32 c = KCos(HalfAngle);

	Quat Result = {s * Axis.x, s * Axis.y, s * Axis.z, c};

	return Normalize ? Result.Normalized() : Result;
}

Quat Quat::Slerp(Quat A, Quat B, f32 Percentage)
{
	Quat Result;
	// NOTE: Only unit quaternions are valid rotations.
	// Normalize to avoid undefined behavior.
	A.Normalize();
	B.Normalize();

	// Compute the cosine of the angle between the two vectors.
	f32 Dot = A.Dot(B);

	// NOTE: If the dot product is negative, slerp won't take the shorter path.
	// NOTE: B and -B are equivalent when the negation is applied to all four components.
	// Fix by reversing one quaternion.
	if (Dot < 0.0f)
	{
		B.x = -B.x;
		B.y = -B.y;
		B.z = -B.z;
		B.w = -B.w;
		Dot = -Dot;
	}

	const f32 DotThreshold = 0.9995f;
	if (Dot > DotThreshold)
	{
		// If the inputs are too close for comfort, linearly interpolate
		// and normalize the result.
		Result = {A.x + ((B.x - A.x) * Percentage),
				  A.y + ((B.y - A.y) * Percentage),
				  A.z + ((B.z - A.z) * Percentage),
				  A.w + ((B.w - A.w) * Percentage)};

		Result.Normalize();
	}
	else
	{
		// Since Dot is in range [0, DotThreshold], acos is safe
		f32 ThetaZero = KAcos(Dot);			// ThetaZero = angle between input vectors
		f32 Theta = ThetaZero * Percentage; // Theta = angle between A and result
		f32 SinTheta = KSin(Theta);			// compute this value only once
		f32 SinThetaZero = KSin(ThetaZero); // compute this value only once

		f32 s0 = KCos(Theta) - Dot * SinTheta / SinThetaZero; // == sin(ThetaZero - Theta) / sin(ThetaZero)
		f32 s1 = SinTheta / SinThetaZero;

		Result = {(A.x * s0) + (B.x * s1),
				  (A.y * s0) + (B.y * s1),
				  (A.z * s0) + (B.z * s1),
				  (A.w * s0) + (B.w * s1)};
	}

	return Result;
}

Quat Quat::Conjugated()
{
	return {-x, -y, -z, w};
}

void Quat::Conjugate()
{
	*this = Conjugated();
}

Quat Quat::Inverse()
{
	Quat Result = Conjugated();
	Result.Normalize();
	return Result;
}

Mat4 Quat::ToMat4()
{
	Mat4 Result = Mat4::Identity();
	Quat n = Normalized();

	Result[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
	Result[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
	Result[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;
	Result[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
	Result[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
	Result[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;
	Result[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
	Result[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
	Result[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

	return Result;
}

Mat4 Quat::ToMat4WithCenter(Vec3 Center)
{
	Mat4 Result = Mat4::Identity();

	Result[0] = (x * x) - (y * y) - (z * z) + (w * w);
	Result[1] = 2.0f * ((x * y) + (z * w));
	Result[2] = 2.0f * ((x * z) - (y * w));
	Result[3] = Center.x - Center.x * Result[0] - Center.y * Result[1] - Center.z * Result[2];

	Result[4] = 2.0f * ((x * y) - (z * w));
	Result[5] = -(x * x) + (y * y) - (z * z) + (w * w);
	Result[6] = 2.0f * ((y * z) + (x * w));
	Result[7] = Center.y - Center.x * Result[4] - Center.y * Result[5] - Center.z * Result[6];

	Result[8] = 2.0f * ((x * z) + (y * w));
	Result[9] = 2.0f * ((y * z) - (x * w));
	Result[10] = -(x * x) - (y * y) + (z * z) + (w * w);
	Result[11] = Center.z - Center.x * Result[8] - Center.y * Result[9] - Center.z * Result[10];

	return Result;
}
