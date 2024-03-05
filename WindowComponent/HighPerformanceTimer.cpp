#include "windowComponent_64.h"

/**
	Author: Alwyn Dippenaar.
	(alwyn.j.dippenaar@gmail.com)

	This is the impl for HighPerformanceTimer.
**/


/**
	Constructor.
**/
HighPerformanceTimer::HighPerformanceTimer()
{
	ZeroMemory(this, sizeof(HighPerformanceTimer));

	QueryPerformanceFrequency(&scale);
}

/** 
	Ticks the timer one on. 
	Value to check after each tick is te 'diff' property.
**/
void HighPerformanceTimer::tick()
{
	if (!QueryPerformanceCounter(&counter))
		return;

	//Cater for first tick.
	if (tmp < 1)
		tmp = counter.QuadPart;

	//Calc diff in ms, since the last tick.
	diff = ((counter.QuadPart - tmp) / (scale.QuadPart/1000));

	//Setup tmp for the next tick.
	tmp = counter.QuadPart;

}

/** 
	Used to stamp the diff, as human readable to an output buffer. 
**/
void HighPerformanceTimer::stampDiff(LPWSTR p_tbuffer, int maxsize)
{
	if (!p_tbuffer) return;

	formatTimeFromMs((int)diff, p_tbuffer, maxsize);
}

/**
	Destructor.
**/
HighPerformanceTimer::~HighPerformanceTimer()
{
	ZeroMemory(this, sizeof(HighPerformanceTimer));
}