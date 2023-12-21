#include "PerfomanceTestTypes.h"
#include "PerfomanceTestUtils.h"

float FFPSMetricsCollector::GetFrameMilliseconds(float _passedSeconds) const
{
	return Displacement::Test::SecondsToMilliseconds(_passedSeconds / TickCounter.GetTicksAmount());
}