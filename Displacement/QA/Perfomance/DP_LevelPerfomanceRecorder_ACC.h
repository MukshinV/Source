// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	FPerfomanceCollectResult LevelPoints;
	TArray<FName> RowNames;
	uint32 CurrentRegionIndex;

	bool PassedAll() const { return CurrentRegionIndex == LevelPoints.PointsCollection.Num();}
	FPerfomancePointTransitionData* GetCurrentTransitionData() const;
	ADP_PerfomancePoint_Actor* GetCurrent() const;
	ADP_PerfomancePoint_Actor* Next();
	ADP_PerfomancePoint_Actor* PeekPrevious();
	void SetNewPointsArray(const FPerfomanceCollectResult& _newArray);
	void ResetPointer() { CurrentRegionIndex = 0u; }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DISPLACEMENT_API UDP_LevelPerfomanceRecorder_ACC : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UDP_LevelPerfomanceRecorder_ACC, FFinishedRecordingEvent, FPerfomanceTestLevelMetrics)
public:
	UDP_LevelPerfomanceRecorder_ACC();

	virtual void PostInitProperties() override;
	void BeginPerfomanceRecording(const FPerfomanceCollectResult& _levelRegions);
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
