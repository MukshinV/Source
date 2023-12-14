// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DP_PerfomancePointInterface.h"
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
	
	virtual FName GetRegionName() const { return FName{GetActorNameOrLabel()}; }
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

	virtual void Tick(float _deltaSeconds) override;
	virtual void OnFinishedStageRecording() override;
private:
	struct Timer
	{
		float TimePassed;
		float WaitDurationSeconds;

		void AddDeltaTime(float _deltaTime) { TimePassed += _deltaTime;}
		bool IsRunning() const { return TimePassed < WaitDurationSeconds; }
		void Reset() { TimePassed = 0.0f; WaitDurationSeconds = 0.0f; }
		void SetWaitDuration(float _waitDuration) { WaitDurationSeconds = _waitDuration; }
	} Timer;
};
