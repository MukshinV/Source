#include "PerfomanceTestTypes.h"
#include "PerfomanceTestUtils.h"

void FPerfomanceTestRegionMetrics::SetFrameMilliseconds(float _frameTime)
{
	Displacement::Test::ConvertFloatToDisplayString(Metrics[static_cast<uint8>(EMetricType::FrameMilliseconds)], _frameTime);
}

void FPerfomanceTestRegionMetrics::SetFPSDelta(float _fpsDelta)
{
	Displacement::Test::ConvertFloatToDisplayString(Metrics[static_cast<uint8>(EMetricType::MaxFPSDelta)], _fpsDelta);
}

void FPerfomanceTestRegionMetrics::SetTicksPerSecond(float _tickPerSecond)
{
	Displacement::Test::ConvertFloatToDisplayString(Metrics[static_cast<uint8>(EMetricType::TicksPerSecond)], _tickPerSecond);
}

float FFPSMetricsCollector::GetFrameMilliseconds(float _passedSeconds) const
{
	return Displacement::Test::SecondsToMilliseconds(_passedSeconds / TickCounter.GetTicksAmount());
}
