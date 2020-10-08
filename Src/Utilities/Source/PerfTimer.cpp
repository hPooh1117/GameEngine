#include "PerfTimer.h"

#include <iostream>

#include "Log.h"

//===================================================================================================================
//-----------------------------------------------------------------------------------------------

PerfTimer::PerfTimer()
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
	if (m_bIsStopped)	totalTime = (mStopTime - mBaseTime) - mPausedTime;

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
	m_bIsStopped = true;
	mDt = Duration_t::zero();
}

//-----------------------------------------------------------------------------------------------

void PerfTimer::Start()
{
	if (m_bIsStopped)
	{
		mPausedTime = mStartTime - mStopTime;
		mPrevTime = GetNow();
		m_bIsStopped = false;
	}
	Tick();
}

//-----------------------------------------------------------------------------------------------

void PerfTimer::Stop()
{
	Tick();
	if (!m_bIsStopped)
	{
		mStopTime = GetNow();
		m_bIsStopped = true;
	}
}

//-----------------------------------------------------------------------------------------------

float PerfTimer::Tick()
{
	if (m_bIsStopped)
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
