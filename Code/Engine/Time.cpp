#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include "Time.hpp"

//----------------------------------------------------------------------------------------------------
static double g_secondsPerCount = 0.0;

//----------------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	assert( g_secondsPerCount != 0.0 );

	LARGE_INTEGER performanceCount;
	QueryPerformanceCounter( &performanceCount );

	double timeSeconds = static_cast< double >( performanceCount.QuadPart ) * g_secondsPerCount;
	return timeSeconds;
}

//----------------------------------------------------------------------------------------------------
void InitializeTimer()
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	g_secondsPerCount = 1.0 / static_cast< double>( countsPerSecond.QuadPart );
}
