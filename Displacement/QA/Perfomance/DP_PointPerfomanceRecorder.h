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
	void StartRecordingRegion(ADP_PerfomancePoint_Actor* _pointToRecord);
	bool IsRegionRecording() const { return TimePassed < WaitDurationSeconds; }
	void UpdateTestMetrics(float _deltaTime);
	FPerfomanceTestRegionData CollectTestMetrics();
private:
	UPROPERTY()
	TObjectPtr<ADP_PerfomancePoint_Actor> CurrentPoint;

	FPerfomanceTestRegionData RegionData;
	
	int32 TickCounter;
	float TimePassed;
	float WaitDurationSeconds;
};
