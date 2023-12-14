// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PointPerfomanceRecorder.h"

void UDP_PointPerfomanceRecorder::SetRecordingPoint(ADP_PerfomancePoint_Actor* _pointToRecord)
{
	check(_pointToRecord)
	
	CurrentPoint = _pointToRecord;
}

void UDP_PointPerfomanceRecorder::EnterRecordingPoint()
{
	check(CurrentPoint)
	
	RegionData.Reset();
	FPSCounter.Reset();
	
	CurrentPoint->OnRecorderEntered();
	CurrentPoint->OnStartedStageRecording();

	RegionData.RegionName = CurrentPoint->GetRegionName().ToString();
}

void UDP_PointPerfomanceRecorder::ExitRecordingPoint()
{
	check(CurrentPoint)
	
	if(!CurrentPoint->IsRecording()) return;

	CurrentPoint->OnRecorderExit();
}

bool UDP_PointPerfomanceRecorder::IsRegionRecording() const
{
	check(CurrentPoint)

	return CurrentPoint->IsRecording();
}

bool UDP_PointPerfomanceRecorder::CanMoveToNextStage() const
{
	check(CurrentPoint)

	return CurrentPoint->CanMoveToNextStage();
}

void UDP_PointPerfomanceRecorder::MoveToNextPointStage()
{
	check(CurrentPoint)

	CurrentPoint->MoveToNextStage();

	RegionData.Reset();
	FPSCounter.Reset();
	
	RegionData.RegionName = CurrentPoint->GetRegionName().ToString();
	
	CurrentPoint->OnStartedStageRecording();
}

void UDP_PointPerfomanceRecorder::UpdateTestMetrics(float _deltaTime)
{
	RegionData.MaxFPSDelta = FMath::Max(_deltaTime, RegionData.MaxFPSDelta);
	FPSCounter.AddTick();
}

FPerfomanceTestRegionData UDP_PointPerfomanceRecorder::CollectTestMetrics()
{
	const float secondsSinceStart = CurrentPoint->GetPassedTimeSinceRecordingStart();
	const float averageFps = FPSCounter.GetAverageFPS(secondsSinceStart);
	RegionData.AverageFPS = averageFps;

	return RegionData;
}
