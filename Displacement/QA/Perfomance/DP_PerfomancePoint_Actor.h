// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DP_PerfomancePointInterface.h"
#include "PerfomanceTestTypes.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DP_PerfomancePoint_Actor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomancePoint_Actor : public AActor, public IDP_PerfomancePointInterface
{
	GENERATED_BODY()

public:
	ADP_PerfomancePoint_Actor();
	virtual void BeginPlay() override;
	
	virtual FName GetRegionName() const;
	FDataTableRowHandle GetRowHandle() const { return PointTableHandle; }
	FPerfomancePointTransitionData* GetPointData() const;
	float GetWaitAmount() const { return WaitAmountSeconds; }
	float GetPassedTimeSinceRecordingStart() const { return Timer.TimePassed; }

	virtual void MoveToNextStage() {}
	
	virtual bool IsRecording() const { return Timer.IsRunning(); }
	virtual bool CanMoveToNextPoint() const override { return !IsRecording(); }
	virtual bool CanMoveToNextStage() const override { return false; }
	
	virtual void OnRecorderEntered()  override {}
	virtual void OnStartedStageRecording() override;
	virtual void OnRecorderExit() override {}
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data")
	float WaitAmountSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data")
	FDataTableRowHandle PointTableHandle;

	virtual void Tick(float _deltaSeconds) override;
	virtual void OnFinishedStageRecording() override;
private:
	FPerfomanceTestTimer Timer;
};
