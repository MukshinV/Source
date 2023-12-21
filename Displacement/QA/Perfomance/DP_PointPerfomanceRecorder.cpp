// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PointPerfomanceRecorder.h"

void UDP_PointPerfomanceRecorder::SetRecordingPoint(ADP_PerfomancePoint_Actor* _pointToRecord)
{
	CurrentPoint = _pointToRecord;
}

void UDP_PointPerfomanceRecorder::EnterRecordingPoint()
{
	PointMetrics.Reset();
	MetricsCollector.StartCollect();
	
	CurrentPoint->OnRecorderEntered();
	CurrentPoint->OnStartedStageRecording();

	PointMetrics.RegionName = CurrentPoint->GetRegionName().ToString();
}

void UDP_PointPerfomanceRecorder::ExitRecordingPoint()
{
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
	return CurrentPoint->IsRecording();
}

bool UDP_PointPerfomanceRecorder::CanMoveToNextStage() const
{
	return CurrentPoint->CanMoveToNextStage();
}

void UDP_PointPerfomanceRecorder::MoveToNextPointStage()
{
	CurrentPoint->MoveToNextStage();

	PointMetrics.Reset();
	MetricsCollector.StartCollect();
	
	CurrentPoint->OnStartedStageRecording();

	PointMetrics.RegionName = CurrentPoint->GetRegionName().ToString();
}

void UDP_PointPerfomanceRecorder::UpdateTestMetrics(float _deltaTime)
{
	MetricsCollector.Tick(_deltaTime);
}

FPerfomanceTestRegionMetrics UDP_PointPerfomanceRecorder::CollectTestMetrics()
{
	PointMetrics.TicksPerSecond = MetricsCollector.GetTickAmount() / CurrentPoint->GetWaitAmount();
	PointMetrics.FrameMilliseconds = MetricsCollector.GetFrameMilliseconds(CurrentPoint->GetWaitAmount());
	PointMetrics.MaxFPSDelta = MetricsCollector.GetMaxFPSDelta();
	
	return PointMetrics;
}
