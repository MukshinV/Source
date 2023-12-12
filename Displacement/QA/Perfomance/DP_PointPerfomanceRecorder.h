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
	void EnterRecordingPoint(ADP_PerfomancePoint_Actor* _pointToRecord);
	void ExitRecordingPoint();
	bool IsRegionRecording() const;
	bool CanMoveToNextPoint() const;
	void UpdateTestMetrics(float _deltaTime);
	FPerfomanceTestRegionData CollectTestMetrics();
	void MoveToNextPointStage();
private:
	UPROPERTY()
	TObjectPtr<ADP_PerfomancePoint_Actor> CurrentPoint;
	FPerfomanceTestRegionData RegionData;

	struct FPSCounter
	{
		int32 TickCounter;

		float GetAverageFPS(float _timePassed) const { return static_cast<float>(TickCounter) / _timePassed; }
		void Reset() { TickCounter = 0; }
		void AddTick() { ++TickCounter; }
	} FPSCounter;
};
