#pragma once

#include "Vector.h"
#include "Util.h"

class Quaternion
{
public:
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

	Quaternion()
	{
		
	}

	explicit Quaternion(float inX, float inY, float inZ, float inW)
	{
		Set(inX, inY, inZ, inW);
	}

	explicit Quaternion(const Vector3& axis, float angle)
	{
		float scalar = sinf(angle / 2.0f);
		x = axis.x * scalar;
		y = axis.y * scalar;
		z = axis.z * scalar;
		w = cosf(angle / 2.0f);
	}

	inline void Set(float inX, float inY, float inZ, float inW)
	{
		x = inX;
		y = inY;
		z = inZ;
		w = inW;
	}
	
	inline void SetToRotateX(float theta)
	{
		float thetaOver2 = theta * .5f;

		w = cosf(thetaOver2);
		x = sinf(thetaOver2);
		y = 0.0f;
		z = 0.0f;
	}

	inline void SetToRotateY(float theta)
	{
		float thetaOver2 = theta * .5f;

		w = cosf(thetaOver2);
		x = 0.0f;
		y = sinf(thetaOver2);
		z = 0.0f;
	}
	inline void SetToRotateZ(float theta)
	{
		float thetaOver2 = theta * 0.5f;
		w = cosf(thetaOver2);
		x = 0.0f;
		y = 0.0f;
		z = sinf(thetaOver2);
	}
	inline void SetToRotateAxis(const Vector3& axis, float theta)
	{
		float thetaOver2 = theta * .5f;
		float sinThetaOver2 = sinf(thetaOver2);

		w = cosf(thetaOver2);
		x = axis.x * sinThetaOver2;
		y = axis.y * sinThetaOver2;
		z = axis.z * sinThetaOver2;
	}

	inline void Conjugate()
	{
		x *= -1;
		y *= -1;
		z *= -1;
	}

	inline float LengthSq() const
	{
		return (x * x + y * y + z * z + w * w);
	}

	inline float Length() const
	{
		return sqrtf(LengthSq());
	}

	inline void Normalize()
	{
		float length = Length();
		x /= length;
		y /= length;
		z /= length;
		w /= length;
	}

	static Quaternion Normalize(const Quaternion& q)
	{
		Quaternion result = q;
		result.Normalize();
		return result;
	}

	static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t)
	{
		Quaternion result;
		result.x = MathOp::Lerp(a.x, b.x, t);
		result.y = MathOp::Lerp(a.y, b.y, t);
		result.z = MathOp::Lerp(a.z, b.z, t);
		result.w = MathOp::Lerp(a.w, b.w, t);
		result.Normalize();
		return result;
	}

	static float Dot(const Quaternion& a, const Quaternion& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t)
	{
		float rawCosm = Quaternion::Dot(a, b);

		float cosom = -rawCosm;
		if (rawCosm >= 0.0f)
		{
			cosom = rawCosm;
		}

		float scale0, scale1;

		if (cosom < 0.9999f)
		{
			const float omega = acosf(cosom);
			const float invSin = 1.0f / sinf(omega);
			scale0 = sinf((1.0f - t) * omega) * invSin;
			scale1 = sinf(t * omega) * invSin;
		}
		else
		{
			scale0 = 1.0f - t;
			scale1 = t;
		}

		if (rawCosm < 0.0f)
		{
			scale1 = -scale1;
		}

		Quaternion result;
		result.x = scale0 * a.x + scale1 * b.x;
		result.y = scale0 * a.y + scale1 * b.y;
		result.z = scale0 * a.z + scale1 * b.z;
		result.w = scale0 * a.w + scale1 * b.w;
		result.Normalize();
		return result;
	}

	static Quaternion Concatenate(const Quaternion& q, const Quaternion& p)
	{
		Quaternion result;

		Vector3 qv(q.x, q.y, q.z);
		Vector3 pv(p.x, p.y, p.z);
		Vector3 newVec = p.w * qv + q.w * pv + pv.cross(qv);
		result.x = newVec.x;
		result.y = newVec.y;
		result.z = newVec.z;

		result.w = p.w * q.w - pv.dot(qv);
		return Quaternion::Normalize(result);
	}

	static Quaternion CreateFromEulerAngle(float pitch, float yaw, float roll)
	{
		Quaternion result, aboutX, aboutY, aboutZ;
		aboutX.SetToRotateX(pitch);
		aboutY.SetToRotateY(yaw);
		aboutZ.SetToRotateZ(roll);
		result = Concatenate(Concatenate(aboutX, aboutY), aboutZ);
		return result;
	}
};