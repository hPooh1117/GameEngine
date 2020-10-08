#pragma once
#include <DirectXMath.h>

// @brief Vector2 structure for vector calculation
// @var  x
// @var  y
class Vector2 : public DirectX::XMFLOAT2
{
public:
	Vector2() :DirectX::XMFLOAT2(0, 0) {}
	Vector2(float x, float y) :DirectX::XMFLOAT2(x, y) {}
	Vector2(const Vector2& v) { x = v.x; y = v.y; }
	~Vector2() {}

	inline Vector2& operator=(const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return (*this);
	}

	inline Vector2& operator+=(const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return (*this);
	}
	inline Vector2& operator-=(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return (*this);
	}	
	inline Vector2& operator*=(float f)
	{
		x *= f;
		y *= f;
		return (*this);
	}
	inline Vector2& operator/=(float f)
	{
		x /= f;
		y /= f;
		return (*this);
	}

	inline Vector2 operator+()
	{
		return Vector2(x, y);
	}
	inline Vector2 operator-()
	{
		return Vector2(-x, -y);
	}

	inline Vector2 operator+(const Vector2& v)
	{
		return Vector2(x + v.x, y + v.y);
	}
	inline Vector2 operator-(const Vector2& v)
	{
		return Vector2(x - v.x, y - v.y);
	}
	inline Vector2 operator*(float f) const
	{
		return Vector2(x * f, y * f);
	}
	inline friend Vector2 operator*(float f, const Vector2& vec)
	{
		return Vector2(vec.x * f, vec.y * f);
	}

	inline Vector2 operator/(float f) const
	{
		return Vector2(x / f, y / f);
	}

	inline bool operator==(const Vector2& v)
	{
		return (x == v.x && y == v.y);
	}
	inline bool operator!=(const Vector2& v)
	{
		return !(x == v.x && y == v.y);
	}
};

// @brief 3ŽŸŒ³VectorŒvŽZ—pƒNƒ‰ƒX(XMFLOAT3Œp³)
// @var  x
// @var  y
// @var  z
class Vector3 : public DirectX::XMFLOAT3
{
public:
	Vector3() :DirectX::XMFLOAT3(0, 0, 0) {}
	Vector3(float x, float y, float z) :DirectX::XMFLOAT3(x, y, z) {}
	Vector3(DirectX::XMFLOAT3& v) :DirectX::XMFLOAT3(v) {}
	Vector3(const Vector3& v) { x = v.x; y = v.y; z = v.z; }
	~Vector3() {}

	inline Vector3& operator=(const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	inline Vector3& operator+=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return (*this);
	}
	inline Vector3& operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return (*this);
	}
	inline Vector3& operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return (*this);
	}
	inline Vector3& operator/=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		return (*this);
	}

	inline Vector3 operator+() 
	{
		return Vector3(x, y, z);
	}

	inline Vector3 operator-()
	{
		return Vector3(-x, -y, -z);
	}

	inline Vector3 operator+(const Vector3& v) const 
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	inline Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	inline Vector3 operator*(float f) const
	{
		return Vector3(x * f, y * f, z * f);
	}
	inline friend Vector3 operator*(float f, const Vector3& v)
	{
		return Vector3(f * v.x, f * v.y, f * v.z);
	}
	inline Vector3 operator/(float f) const
	{
		return Vector3(x / f, y / f, z / f);
	}

	inline bool operator==(const Vector3& v)
	{
		return (x == v.x && y == v.y && z == v.z);
	}
	inline bool operator!=(const Vector3& v)
	{
		return !(x == v.x && y == v.y && z == v.z);
	}

	inline float dot(const Vector3& v1) const 
	{
		float dot = x * v1.x + y * v1.y + z * v1.z;
		return dot;
	}
	static float Dot(const Vector3& v1, const Vector3& v2)
	{
		return v1.dot(v2);
	}

	inline Vector3 cross(const Vector3& v1) const
	{
		Vector3 out;
		out.x = y * v1.z - v1.y * z;
		out.y = z * v1.x - v1.z * x;
		out.z = x * v1.y - v1.x * y;
		return out;
	}

	static Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		
		Vector3 out;
		out.x = v1.y * v2.z - v2.y * v1.z;
		out.y = v1.z * v2.x - v2.z * v1.x;
		out.z = v1.x * v2.y - v2.x * v1.y;
		return out;
	}

	inline float getLength() const
	{
		float l = sqrtf(x * x + y * y + z * z);
		return l;
	}
	inline void normalize()
	{
		(*this) /= getLength();
	}
	static Vector3 Normalize(const Vector3& v)
	{
		Vector3 result = v;
		result.normalize();
		return result;
	}
	inline bool IsAcute(const Vector3& other) const 
	{
		return (this->dot(other) >= 0);
	}
	inline bool IsParallel(const Vector3& other) const
	{
		float d = this->cross(other).dot(this->cross(other));
		return (d > -0.00001f && d < 0.00001f);
	}

	inline XMFLOAT3 getXMFLOAT3() const
	{
		return XMFLOAT3(x, y, z);
	}
};


// @brief Vector4 structure for vector calculation
// @var  x
// @var  y
// @var  z
// @var  w
class Vector4 : public DirectX::XMFLOAT4
{
public:
	Vector4() :DirectX::XMFLOAT4(0, 0, 0, 0) {}
	Vector4(float x, float y, float z, float w) :DirectX::XMFLOAT4(x, y, z, w) {}
	Vector4(const Vector3& v) :DirectX::XMFLOAT4(v.x, v.y, v.z, 1){}
	Vector4(const Vector4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline Vector4& operator=(const Vector4& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}
	inline Vector4& operator += (const Vector4 & v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}
	inline Vector4& operator -=(const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return (*this);
	}
	inline Vector4& operator *=(const float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return (*this);
	}

	inline Vector4& operator/=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		w /= f;
		return (*this);
	}
	inline bool operator==(const Vector4& v)
	{
		return (x == v.x && y == v.y && z == v.z && w == v.w);
	}
	inline bool operator!=(const Vector4& v)
	{
		return !(x == v.x && y == v.y && z == v.z && w == v.w);
	}
	inline Vector3 getXYZ() { return Vector3(x, y, z); }
	~Vector4() {}
};