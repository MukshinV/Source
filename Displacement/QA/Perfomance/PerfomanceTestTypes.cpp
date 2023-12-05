#include "PerfomanceTestTypes.h"

float FPerfomanceTestLevelData::CalculateAverageFPS() const
{
	float sumFps = 0.0f;
	for(int32 i = 0; i < RegionDatas.Num(); ++i)
	{
		sumFps += RegionDatas[i].AverageFPS;
	}
	return sumFps / static_cast<float>(RegionDatas.Num());
}

float FPerfomanceTestLevelData::GetMaxFPSDelta() const
{
	float maxDelta = 0.0f;
	for(int32 i = 0; i < RegionDatas.Num(); ++i)
	{
		maxDelta = FMath::Max(maxDelta, RegionDatas[i].MaxFPSDelta);
	}
	
	return maxDelta;
}
