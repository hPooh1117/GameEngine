#pragma once
#include "Quaternion.h"
#include <vector>

class MyArrayFromVector
{
private:
	std::vector<float*> m_pArr;
public:
	MyArrayFromVector(Vector2& v)
	{ 
		m_pArr.emplace_back(&v.x);
		m_pArr.emplace_back(&v.y);
	}
	MyArrayFromVector(DirectX::XMFLOAT2& v)
	{
		m_pArr.emplace_back(&v.x);
		m_pArr.emplace_back(&v.y);
	}

	MyArrayFromVector(Vector3& v)
	{
		m_pArr.emplace_back(&v.x);
		m_pArr.emplace_back(&v.y);
		m_pArr.emplace_back(&v.z);
	}
	MyArrayFromVector(DirectX::XMFLOAT3& v)
	{
		m_pArr.emplace_back(&v.x);
		m_pArr.emplace_back(&v.y);
		m_pArr.emplace_back(&v.z);
	}

	MyArrayFromVector(Vector4& v)
	{
		m_pArr.emplace_back(&v.x);
		m_pArr.emplace_back(&v.y);
		m_pArr.emplace_back(&v.z);
		m_pArr.emplace_back(&v.w);
	}
	MyArrayFromVector(DirectX::XMFLOAT4& v)
	{
		m_pArr.emplace_back(&v.x);
		m_pArr.emplace_back(&v.y);
		m_pArr.emplace_back(&v.z);
		m_pArr.emplace_back(&v.w);
	}
	MyArrayFromVector(Quaternion& q)
	{
		m_pArr.emplace_back(&q.x);
		m_pArr.emplace_back(&q.y);
		m_pArr.emplace_back(&q.z);
		m_pArr.emplace_back(&q.w);
	}



public:
	inline float* SetArray() { return m_pArr[0]; }
};