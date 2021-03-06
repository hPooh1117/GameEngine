#pragma once
#include <chrono>

using GETimer_t = std::chrono::time_point<std::chrono::system_clock>;
using Duration_t = std::chrono::duration<float, std::milli>;

class PerfTimer final
{
public:
	static constexpr unsigned int AVERAGE_DENOMINATOR = 10;

private:
	GETimer_t   mBaseTime, mPrevTime, mCurrTime, mStartTime, mStopTime;
	Duration_t  mPausedTime, mDt;
	bool        mbIsStopped;
public:
	PerfTimer();
	~PerfTimer() = default;

	float        GetTotalTime() const;
	GETimer_t    GetNow();
	float        GetDeltaTime();

	void     Reset();
	void     Start();
	void     Stop();
	float    Tick();


private:
	PerfTimer(const PerfTimer&) = delete;
	PerfTimer& operator=(const PerfTimer&) = delete;
};

