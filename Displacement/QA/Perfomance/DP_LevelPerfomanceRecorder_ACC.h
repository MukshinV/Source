// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DP_PointPerfomanceRecorder.h"
#include "Components/ActorComponent.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"
#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"
#include "DP_LevelPerfomanceRecorder_ACC.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DISPLACEMENT_API UDP_LevelPerfomanceRecorder_ACC : public UActorComponent
{
	GENERATED_BODY()

public:
	UDP_LevelPerfomanceRecorder_ACC();
	bool IsRecording() const { return bIsRecording; }

	virtual void PostInitProperties() override;
	void BeginPerfomanceRecording(const TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>& _levelRegions);
	FPerfomanceTestLevelData GetTestResult() const { return LevelTestResult; }

protected:
	void EndPerfomanceRecording();

	virtual void TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<UDP_PointPerfomanceRecorder> PointRecorder;
	UPROPERTY()
	TArray<TObjectPtr<ADP_PerfomancePoint_Actor>> LevelRegions;
	
	FPerfomanceTestLevelData LevelTestResult;

	int32 CurrentRegionIndex;
	bool bIsRecording;

	bool NextRegion();
	void MoveOwnerToNextRegion();
};
