// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DP_PerfomancePointCollection_Actor.h"
#include "DP_PointPerfomanceRecorder.h"
#include "Components/ActorComponent.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"
#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"
#include "DP_LevelPerfomanceRecorder_ACC.generated.h"

USTRUCT()
struct FLevelPointsIterator
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPerfomancePointData> LevelPoints;
	uint32 CurrentRegionIndex;

	bool PassedAll() const { return CurrentRegionIndex == LevelPoints.Num();}
	const FPerfomancePointData* GetCurrent() const;
	const FPerfomancePointData* Next() { ++CurrentRegionIndex; return GetCurrent(); }
	const FPerfomancePointData* PeekPrevious();
	void SetNewPointsArray(const TArray<FPerfomancePointData>& _newArray) { LevelPoints = _newArray;}
	void ResetPointer() { CurrentRegionIndex = 0u; }
};

class FPerfomancePointTransitioner
{
public:
	FPerfomancePointTransitioner() = default;
	bool IsInTransition() const { return !PositionInterpolator.IsFinished() || !RotationInterpolator.IsFinished(); }
	bool IsNeedToCollectMetrics() const { return TransitionDuration > 0.0f; }
	void StartTransition(const ADP_PerfomancePoint_Actor* _fromPoint, const ADP_PerfomancePoint_Actor* _toPoint, float _transitionDuration);
	void Tick(float _deltaTime);
	void MoveToInterpolatedTransform(AActor* _targetActorToMove) const;
	void SetMaxInterpolationValue() { SetInterpolationValue(1.0f); }
	const FPerfomanceTestRegionMetrics& CollectTransitionMetrics();
private:
	FPositionInterpolator PositionInterpolator;
	FRotationInterpolator RotationInterpolator;
	
	FFPSMetricsCollector MetricsCollector;
	FPerfomanceTestRegionMetrics TransitionMetrics;

	float TimePassed;
	float TransitionDuration;

	void SetInterpolationValue(float _interpolationValue); 
	void Reset() { PositionInterpolator.Reset(); RotationInterpolator.Reset(); TimePassed = 0.0f; }
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DISPLACEMENT_API UDP_LevelPerfomanceRecorder_ACC : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UDP_LevelPerfomanceRecorder_ACC, FFinishedRecordingEvent, FPerfomanceTestLevelMetrics)
public:
	UDP_LevelPerfomanceRecorder_ACC();

	virtual void PostInitProperties() override;
	void BeginPerfomanceRecording(const TArray<FPerfomancePointData>& _levelRegions);
	FFinishedRecordingEvent& OnFinishedRecording() { return FinishedRecordingEvent; }
	
protected:
	void EndPerfomanceRecording();
	virtual void TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UDP_PointPerfomanceRecorder> PointRecorder;
	
	FFinishedRecordingEvent FinishedRecordingEvent;
	FPerfomanceTestLevelMetrics LevelTestResult;
	FLevelPointsIterator LevelPointsIterator;
	FPerfomancePointTransitioner PointTransitioner;

	void EnterToCurrentPoint() const;
	
	void StartMoveOwnerToNextPoint();
	void CollectPointRecordingResults();
	bool TryToSwitchToNextPoint();
};
