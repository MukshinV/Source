// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"
#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"
#include "DP_PerfomanceRecorder_ACC.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DISPLACEMENT_API UDP_PerfomanceRecorder_ACC : public UActorComponent
{
	GENERATED_BODY()

public:
	UDP_PerfomanceRecorder_ACC();
	bool IsRecording() const { return bIsRecording; }

	void BeginPerfomanceRecording(const TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>& _levelRegions);
	FPerfomanceTestLevelData GetTestResult() const { return LevelTestResult; }

protected:
	void EndPerfomanceRecording();
	void SetWaitDuration(float _newWaitingDuration);

	virtual void TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

private:
	UPROPERTY()
	FPerfomanceTestLevelData LevelTestResult;
	UPROPERTY()
	TArray<TObjectPtr<ADP_PerfomancePoint_Actor>> LevelRegions;
	
	int32 TickCounter;
	
	int32 CurrentRegionIndex;
	float TimePassed;
	float WaitDurationSeconds;
	bool bIsRecording;

	bool NextRegion();
	void MoveOwnerToNextRegion();
	void UpdateTestMetrics(float _deltaTime);
	void CollectTestMetrics();
};
