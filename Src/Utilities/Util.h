#pragma once
#include "Vector.h"

//----------------------------------------------------------------------------------------------------------------------------

namespace Math
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

	inline float* VectorToArray(Vector2& v)
	{
		float* arr[2] = { &v.x, &v.y };
		return arr[0];
	}

	//----------------------------------------------------------------------------------------------------------------------------

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

