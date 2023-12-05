// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PointPerfomanceRecorder.h"

void UDP_PointPerfomanceRecorder::StartRecordingRegion(ADP_PerfomancePoint_Actor* _pointToRecord)
{
	if(!_pointToRecord) return;

	RegionData = {};
	
	TimePassed = 0.0f;
	TickCounter = 0;

	WaitDurationSeconds = _pointToRecord->GetWaitAmount();
	RegionData.RegionName = _pointToRecord->GetRegionName().ToString();
}

void UDP_PointPerfomanceRecorder::UpdateTestMetrics(float _deltaTime)
{
	TimePassed += _deltaTime;
	
	RegionData.MaxFPSDelta = FMath::Max(_deltaTime, RegionData.MaxFPSDelta);

	++TickCounter;
}

FPerfomanceTestRegionData UDP_PointPerfomanceRecorder::CollectTestMetrics()
{
	const float averageFps = static_cast<float>(TickCounter) / WaitDurationSeconds;
	RegionData.AverageFPS = averageFps;

	return RegionData;
}
