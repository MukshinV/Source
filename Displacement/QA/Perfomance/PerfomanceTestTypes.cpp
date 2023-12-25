#include "PerfomanceTestTypes.h"
#include "PerfomanceTestUtils.h"

void FPerfomanceTestRegionMetrics::SetFrameMilliseconds(float _frameTime)
{
	Displacement::Test::ConvertFloatToDisplayString(FrameMilliseconds, _frameTime);
}

void FPerfomanceTestRegionMetrics::SetFPSDelta(float _fpsDelta)
{
	Displacement::Test::ConvertFloatToDisplayString(MaxFPSDelta, _fpsDelta);
}

void FPerfomanceTestRegionMetrics::SetTicksPerSecond(float _tickPerSecond)
{
	Displacement::Test::ConvertFloatToDisplayString(TicksPerSecond, _tickPerSecond);
}

float FFPSMetricsCollector::GetFrameMilliseconds(float _passedSeconds) const
{
	return Displacement::Test::SecondsToMilliseconds(_passedSeconds / TickCounter.GetTicksAmount());
}
