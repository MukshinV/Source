// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PointPerfomanceRecorder.h"

void UDP_PointPerfomanceRecorder::StartRecordingRegion(ADP_PerfomancePoint_Actor* _pointToRecord)
{
	check(_pointToRecord)
	
	RegionData.Reset();
	FPSCounter.Reset();
	Timer.Reset();
	Timer.SetWaitDuration(_pointToRecord->GetWaitAmount());
	
	RegionData.RegionName = _pointToRecord->GetRegionName().ToString();
}

void UDP_PointPerfomanceRecorder::UpdateTestMetrics(float _deltaTime)
{
	Timer.AddDeltaTime(_deltaTime);
	RegionData.MaxFPSDelta = FMath::Max(_deltaTime, RegionData.MaxFPSDelta);
	FPSCounter.AddTick();
}

FPerfomanceTestRegionData UDP_PointPerfomanceRecorder::CollectTestMetrics()
{
	const float averageFps = FPSCounter.GetAverageFPS(Timer.TimePassed);
	RegionData.AverageFPS = averageFps;

	return RegionData;
}
