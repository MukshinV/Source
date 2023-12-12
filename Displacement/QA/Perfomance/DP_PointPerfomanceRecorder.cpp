// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PointPerfomanceRecorder.h"

void UDP_PointPerfomanceRecorder::EnterRecordingPoint(ADP_PerfomancePoint_Actor* _pointToRecord)
{
	check(_pointToRecord)
	
	RegionData.Reset();
	FPSCounter.Reset();
	
	RegionData.RegionName = _pointToRecord->GetRegionName().ToString();
	CurrentPoint = _pointToRecord;

	CurrentPoint->OnRecorderEntered();
	CurrentPoint->OnStartedStageRecording();
}

void UDP_PointPerfomanceRecorder::ExitRecordingPoint()
{
	if(!CurrentPoint || !CurrentPoint->IsRecording()) return;

	CurrentPoint->OnRecorderExit();
}

bool UDP_PointPerfomanceRecorder::IsRegionRecording() const
{
	if(!CurrentPoint) return false;

	return CurrentPoint->IsRecording();
}

bool UDP_PointPerfomanceRecorder::CanMoveToNextPoint() const
{
	if(!CurrentPoint) return true;

	return CurrentPoint->CanMoveToNextPoint();
}

void UDP_PointPerfomanceRecorder::MoveToNextPointStage()
{
	if(!CurrentPoint) return;

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
