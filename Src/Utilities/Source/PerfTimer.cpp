#include "PerfTimer.h"

#include <iostream>

#include "Log.h"

//===================================================================================================================
//-----------------------------------------------------------------------------------------------

PerfTimer::PerfTimer()
	:mbIsStopped(false),
	mDt(std::chrono::duration<float, std::milli>()),
	mPausedTime(std::chrono::duration<float, std::milli>())
{
}

//-----------------------------------------------------------------------------------------------

float PerfTimer::GetTotalTime() const
{
	Duration_t totalTime = Duration_t::zero();
	// Base	  Stop		Start	 Stop	   Curr
	//--*-------*----------*------*---------|
	//			<---------->
	//			   Paused
	if (mbIsStopped)	totalTime = (mStopTime - mBaseTime) - mPausedTime;

	// Base			Stop	  Start			Curr
	//--*------------*----------*------------|
	//				 <---------->
	//					Paused
	else totalTime = (mCurrTime - mBaseTime) - mPausedTime;

	return totalTime.count();
}

//-----------------------------------------------------------------------------------------------

GETimer_t PerfTimer::GetNow()
{
	return std::chrono::system_clock::now();
}

//-----------------------------------------------------------------------------------------------

float PerfTimer::GetDeltaTime()
{
	return mDt.count();
}

//-----------------------------------------------------------------------------------------------

void PerfTimer::Reset()
{
	mBaseTime = mPrevTime = GetNow();
	mbIsStopped = true;
	mDt = Duration_t::zero();
}

//-----------------------------------------------------------------------------------------------

void PerfTimer::Start()
{
	if (mbIsStopped)
	{
		mPausedTime = mStartTime - mStopTime;
		mPrevTime = GetNow();
		mbIsStopped = false;
	}
	Tick();
}

//-----------------------------------------------------------------------------------------------

void PerfTimer::Stop()
{
	Tick();
	if (!mbIsStopped)
	{
		mStopTime = GetNow();
		mbIsStopped = true;
	}
}

//-----------------------------------------------------------------------------------------------

float PerfTimer::Tick()
{
	if (mbIsStopped)
	{
		mDt = Duration_t::zero();
		return mDt.count();
	}

	mCurrTime = GetNow();
	mDt = mCurrTime - mPrevTime;

	mPrevTime = mCurrTime;
	mDt = mDt.count() < 0.0f ? mDt.zero() : mDt;

	return mDt.count();
}

//-----------------------------------------------------------------------------------------------
//===================================================================================================================
