// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DP_PerfomancePoint_Actor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomancePoint_Actor : public AActor
{
	GENERATED_BODY()

public:
	ADP_PerfomancePoint_Actor();
	
	virtual FName GetRegionName() const { return FName{GetActorNameOrLabel()}; }
	float GetWaitAmount() const { return WaitAmountSeconds; }
	float GetPassedTimeSinceRecordingStart() const { return Timer.TimePassed; }

	virtual void MoveToNextStage() {}
	
	virtual bool IsRecording() const { return Timer.IsRunning(); }
	virtual bool CanMoveToNextPoint() const { return !IsRecording(); }
	virtual bool CanMoveToNextStage() const { return false; }
	
	virtual void OnRecorderEntered() {}
	virtual void OnStartedStageRecording();
	virtual void OnRecorderExit() {}
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data")
	float WaitAmountSeconds;

	virtual void Tick(float _deltaSeconds) override;
	virtual void OnFinishedStageRecording() {}
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
