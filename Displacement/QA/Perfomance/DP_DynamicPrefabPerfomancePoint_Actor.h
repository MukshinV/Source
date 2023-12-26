// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DP_PrefabPerfomancePoint_Actor.h"
#include "DP_DynamicPrefabPerfomancePoint_Actor.generated.h"

USTRUCT(BlueprintType)
struct FPerfomancePrefabRecordPoints
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@SpawnPoint")
	TObjectPtr<AActor> SpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@LocationPoint")
	TObjectPtr<AActor> LocationPoint;
};

USTRUCT(BlueprintType)
struct FRecordPointsIterator
{
	GENERATED_BODY()

	uint32 CurrentPointer{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@RecordPoints")
	TArray<FPerfomancePrefabRecordPoints> RecordPoints;

	const FPerfomancePrefabRecordPoints* Get() const { return &RecordPoints[CurrentPointer]; }
	const FPerfomancePrefabRecordPoints* GetPrevious() { return &RecordPoints[CurrentPointer == 0 ? RecordPoints.Num() - 1 : CurrentPointer - 1]; }
	void Next() { CurrentPointer = (CurrentPointer + 1) % RecordPoints.Num(); }
};

UCLASS()
class DISPLACEMENT_API ADP_DynamicPrefabPerfomancePoint_Actor : public ADP_PrefabPerfomancePoint_Actor
{
	GENERATED_BODY()

public:
	virtual FName GetRegionName() const override;
	virtual bool IsRecording() const override;
	virtual bool IsValidForTesting() const override;
	virtual AActor* GetSpawnPoint() override { return RecordPointsIterator.Get()->SpawnPoint; }

	virtual void OnRecorderEntered(AActor* _recorder) override;
	virtual void OnStartedStageRecording() override;
	virtual void MoveToNextStage() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data", DisplayName="@Iterator")
	FRecordPointsIterator RecordPointsIterator;

	virtual void Tick(float _deltaSeconds) override;
private:
	UPROPERTY()
	TObjectPtr<AActor> Recorder;
	FPerfomancePointTransitioner PrefabTransitioner;
};
