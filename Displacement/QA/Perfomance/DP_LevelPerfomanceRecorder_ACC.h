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

	using PerfPointsArray_T = TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>;
	
	UPROPERTY()
	TArray<TObjectPtr<ADP_PerfomancePoint_Actor>> LevelPoints;
	int32 CurrentRegionIndex;

	bool PassedAll() const { return CurrentRegionIndex == LevelPoints.Num();}
	bool Next() { return ++CurrentRegionIndex < LevelPoints.Num();}
	ADP_PerfomancePoint_Actor* GetPerfomancePoint() const { return LevelPoints[CurrentRegionIndex]; }
	void SetNewPointsArray(const PerfPointsArray_T& _newArray) { LevelPoints = _newArray;}
	void ResetPointer() { CurrentRegionIndex = 0; }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DISPLACEMENT_API UDP_LevelPerfomanceRecorder_ACC : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UDP_LevelPerfomanceRecorder_ACC, FFinishedRecordingEvent, FPerfomanceTestLevelData)
public:
	UDP_LevelPerfomanceRecorder_ACC();

	virtual void PostInitProperties() override;
	void BeginPerfomanceRecording(const TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>& _levelRegions);
	FFinishedRecordingEvent& OnFinishedRecording() { return FinishedRecordingEvent; }
	
protected:
	void EndPerfomanceRecording();
	virtual void TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UDP_PointPerfomanceRecorder> PointRecorder;
	
	FFinishedRecordingEvent FinishedRecordingEvent;
	FPerfomanceTestLevelData LevelTestResult;
	FLevelPointsIterator LevelPointsIterator;

	void MoveOwnerToNextRegion() const;
	void CollectPointRecordingResults();
	bool CanSwitchToNextPoint();
	bool TryToSwitchToNextPoint();
};
