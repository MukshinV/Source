// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PointPerfomanceRecorder.h"

void UDP_PointPerfomanceRecorder::SetRecordingPoint(ADP_PerfomancePoint_Actor* _pointToRecord)
{
	check(_pointToRecord);
	
	CurrentPoint = _pointToRecord;
}

void UDP_PointPerfomanceRecorder::EnterRecordingPoint()
{
	check(CurrentPoint);
	
	PointMetrics.Reset();
	MetricsCollector.StartCollect();
	
	CurrentPoint->OnRecorderEntered();
	CurrentPoint->OnStartedStageRecording();

	PointMetrics.RegionName = CurrentPoint->GetRegionName().ToString();
}

void UDP_PointPerfomanceRecorder::ExitRecordingPoint()
{
	check(CurrentPoint)
	
	if(!CurrentPoint->IsRecording()) return;

	CurrentPoint->OnRecorderExit();
}

void FFPSMetricsCollector::Tick(float _deltaTime)
{
	MaxFPSDelta = FMath::Max(_deltaTime, MaxFPSDelta);
	TickCounter.AddTick();
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

	PointMetrics.Reset();
	MetricsCollector.StartCollect();
	
	PointMetrics.RegionName = CurrentPoint->GetRegionName().ToString();
	
	CurrentPoint->OnStartedStageRecording();
}

void UDP_PointPerfomanceRecorder::UpdateTestMetrics(float _deltaTime)
{
	MetricsCollector.Tick(_deltaTime);
}

FPerfomanceTestRegionMetrics UDP_PointPerfomanceRecorder::CollectTestMetrics()
{
	PointMetrics.TicksPerSecond = MetricsCollector.GetTickAmount() / CurrentPoint->GetWaitAmount();
	PointMetrics.MaxFPSDelta = MetricsCollector.GetMaxFPSDelta();
	
	return PointMetrics;
}
