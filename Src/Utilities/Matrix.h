#pragma once
#include <DirectXMath.h>
#include "Vector.h"
#include "Quaternion.h"

struct Matrix : public DirectX::XMFLOAT4X4
{
public:
	Matrix() :DirectX::XMFLOAT4X4() { _11 = 1; _22 = 1; _33 = 1; _44 = 1; }
	Matrix(DirectX::XMFLOAT4X4& other) :DirectX::XMFLOAT4X4(other) {}
	Matrix(DirectX::XMMATRIX& m) { DirectX::XMStoreFloat4x4(this, m); }
	Matrix(const Matrix& other) {
		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				m[i][j] = other.m[i][j];
			}
		}
	}
	Matrix& operator=(const Matrix& other) {
		for (auto i = 0; i < 4; ++i)
		{
			for (auto j = 0; j < 4; ++j)
			{
				m[i][j] = other.m[i][j];
			}
		}
		return *this;
	}

	Matrix operator*(const Matrix& mat)
	{
		DirectX::XMFLOAT4X4 out;
		DirectX::XMStoreFloat4x4(&out, DirectX::XMLoadFloat4x4(this) * DirectX::XMLoadFloat4x4(&mat));
		return out;
	}

	friend Matrix operator*(const Matrix& m1, const Matrix& m2)
	{
		DirectX::XMFLOAT4X4 out;
		DirectX::XMStoreFloat4x4(&out, DirectX::XMLoadFloat4x4(&m1) * DirectX::XMLoadFloat4x4(&m2));
		return out;
	}

	Matrix& Store(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
		return *this;
	}

	Vector3 Transform(const Vector3& v)
	{
		Vector3 out;
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&v);
		DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(this);
		DirectX::XMStoreFloat3(&out, DirectX::XMVector3Transform(v1, m1));
		return out;
	}

	static Matrix GetRotationMatrix(const Vector3& v)
	{
		Matrix out;
		return out.Store(DirectX::XMMatrixRotationRollPitchYaw(v.x, v.y, v.z));
	}
	static Matrix GetTranslationMatrix(const Vector3& v)
	{
		Matrix out;
		return out.Store(DirectX::XMMatrixTranslation(v.x, v.y, v.z));
	}

	inline Vector3 GetXAxis() const
	{
		return Vector3::Normalize(Vector3(_11, _12, _13));
	}
	inline Vector3 GetYAxis() const
	{
		return Vector3::Normalize(Vector3(_21, _22, _23));
	}
	inline Vector3 GetZAxis() const
	{
		return Vector3::Normalize(Vector3(_31, _32, _33));
	}
	static Matrix CreateFromQuaternion(const Quaternion& q)
	{
		Matrix mat;
		mat._11 = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
		mat._12 = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
		mat._13 = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
		mat._14 = 0.0f;

		mat._21 = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
		mat._22 = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
		mat._23 = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
		mat._24 = 0.0f;

		mat._31 = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
		mat._32 = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
		mat._33 = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
		mat._34 = 0.0f;

		mat._41 = 0.0f;
		mat._42 = 0.0f;
		mat._43 = 0.0f;
		mat._44 = 1.0f;

		return mat;
	}
	
	void Invert()
	{
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(this);
		m = DirectX::XMMatrixInverse(nullptr, m);
		DirectX::XMStoreFloat4x4(this, m);
	}
};