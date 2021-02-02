#pragma once
#include "./Utilities/CommonInclude.h"
#include "Vector.h"
#include <string>
#include <Windows.h>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------------

namespace MathOp
{
	//----------------------------------------------------------------------------------------------------------------------------
	// @param float a : 変化前の値
	// @param float b : 変化後の値
	// @param float t : 媒介変数
	inline float Lerp(float a, float b, float t)
	{
		return a * (1 - t) + b * t;
	}

	//----------------------------------------------------------------------------------------------------------------------------

	// @param Vector3 a : 始点
	// @param Vector3 b : 終点
	// @param float   t : 線形補間用の媒介変数
	inline Vector3 Lerp(Vector3 a, Vector3 b, float t)
	{
		return a * (1 - t) + b * t;
	}

	//----------------------------------------------------------------------------------------------------------------------------

	inline XMMATRIX Lerp(const XMFLOAT4X4& a, const XMFLOAT4X4& b, float t)
	{
		return DirectX::XMLoadFloat4x4(&a) * (1.0f - t) + DirectX::XMLoadFloat4x4(&b) * t;
	}

	//----------------------------------------------------------------------------------------------------------------------------

	inline float* VectorToArray(Vector2& v)
	{
		float* arr[2] = { &v.x, &v.y };
		return arr[0];
	}

	//----------------------------------------------------------------------------------------------------------------------------
}

namespace StringOp
{
	void StringConvert(const std::string& from, std::wstring& to);

	void StringConvert(const std::wstring& from, std::string& to);

	int StringConvert(const char* from, wchar_t* to);

	int StringConvert(const wchar_t* from, char* to);

}

namespace FileOp
{
	std::string GetApplicationDirectory();

	std::string GetOriginalWorkingDirectory();

	std::string GetWorkingDirectory();

	void SetWorkingDirectory(const std::string& path);

	void SplitPath(const std::string& fullPath, std::string& dir, std::string& filename);

	std::string GetFilenameFromPath(const std::string& fullPath);

	std::string GetDirectoryFromPath(const std::string& fullPath);

	std::string GetExtensionFromFilename(const std::string& filename);

	void RemoveExtensionFromFilename(std::string& filename);

	std::string ExpandPath(const std::string& path);

	bool FileRead(const std::string& filename, std::vector<uint8_t>& data);

	bool FileWrite(const std::string& filename, const uint8_t* data, size_t size);

	bool FileExists(const std::string& filename);
}


//----------------------------------------------------------------------------------------------------------------------------

// @class FrameTimer
// @brief 汎用性タイマークラス
class FrameTimer final
{
private:
	unsigned int mTimer = 0;
	unsigned int mTimerMax = 0;


public:
	FrameTimer() = default;
	FrameTimer(unsigned int max) :mTimerMax(max) {}
	~FrameTimer() = default;

	inline void Init();
	inline void Tick();
	inline bool IsStopped();

	inline float        GetTimeRate();
	inline unsigned int GetTime();
	inline float        GetTimeFloat();
	inline unsigned int GetTimeMax();
	inline float        GetTimeMaxFloat();
	inline void         SetTime(unsigned int time);
	inline void         SetMaxTime(unsigned int time);
};

void FrameTimer::Init()
{
	mTimer = 0;
}

void FrameTimer::Tick()
{
	mTimer++;

	if (mTimer >= mTimerMax) mTimer = mTimerMax;
}

bool FrameTimer::IsStopped()
{
	return mTimer >= mTimerMax;
}

float        FrameTimer::GetTimeRate() { return static_cast<float>(mTimer) / static_cast<float>(mTimerMax); }
unsigned int FrameTimer::GetTime() { return mTimer; }
float        FrameTimer::GetTimeFloat() { return static_cast<float>(mTimer); }
unsigned int FrameTimer::GetTimeMax() { return mTimerMax; }
float        FrameTimer::GetTimeMaxFloat() { return static_cast<float>(mTimerMax); }
void         FrameTimer::SetTime(unsigned int time) { mTimer = time; }
void		 FrameTimer::SetMaxTime(unsigned int time) { mTimerMax = time; }

//----------------------------------------------------------------------------------------------------------------------------

