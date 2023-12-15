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
	ADP_PerfomancePoint_Actor* GetCurrent() const;
	ADP_PerfomancePoint_Actor* Next() { ++CurrentRegionIndex; return GetCurrent(); }
	void SetNewPointsArray(const TArray<FPerfomancePointData>& _newArray) { LevelPoints = _newArray;}
	void ResetPointer() { CurrentRegionIndex = 0u; }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DISPLACEMENT_API UDP_LevelPerfomanceRecorder_ACC : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UDP_LevelPerfomanceRecorder_ACC, FFinishedRecordingEvent, FPerfomanceTestLevelData)
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
	FPerfomanceTestLevelData LevelTestResult;
	FLevelPointsIterator LevelPointsIterator;

	void MoveOwnerToNextRegion() const;
	void CollectPointRecordingResults();
	bool TryToSwitchToNextPoint();
};
