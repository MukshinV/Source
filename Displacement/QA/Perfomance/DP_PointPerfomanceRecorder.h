// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DP_PerfomancePoint_Actor.h"
#include "PerfomanceTestTypes.h"
#include "UObject/Object.h"
#include "DP_PointPerfomanceRecorder.generated.h"

UCLASS()
class DISPLACEMENT_API UDP_PointPerfomanceRecorder : public UObject
{
	GENERATED_BODY()
public:
	bool IsRegionRecording() const;
	bool CanMoveToNextStage() const;

	bool IsOnRecordingPoint(const ADP_PerfomancePoint_Actor* _pointToCheck) const { return CurrentPoint == _pointToCheck; };
	void SetRecordingPoint(ADP_PerfomancePoint_Actor* _pointToRecord);
	void SetRecordingActor(AActor* _recordingActor);
	void EnterRecordingPoint();
	void ExitRecordingPoint();

	void UpdateTestMetrics(float _deltaTime);
	FPerfomanceTestRegionMetrics CollectTestMetrics();
	void MoveToNextPointStage();
private:
	UPROPERTY()
	TObjectPtr<ADP_PerfomancePoint_Actor> CurrentPoint;
	UPROPERTY()
	TObjectPtr<AActor> Recorder;
	
	FPerfomanceTestRegionMetrics PointMetrics;
	FFPSMetricsCollector MetricsCollector;
};
