// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DP_PerfomancePoint_Actor.h"
#include "Asset/PrefabricatorAsset.h"
#include "DP_PrefabPerfomancePoint_Actor.generated.h"

USTRUCT(BlueprintType)
struct FDP_PerfomanceTestPrefabUnit_QStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPrefabricatorAsset> PrefabUnit;
};

using TestPrefabUnit_F = FDP_PerfomanceTestPrefabUnit_QStruct;

UCLASS()
class DISPLACEMENT_API ADP_PrefabPerfomancePoint_Actor : public ADP_PerfomancePoint_Actor
{
	GENERATED_BODY()
public:
	ADP_PrefabPerfomancePoint_Actor();
	virtual void BeginPlay() override;
	virtual FName GetRegionName() const override { return PrefabIterator.GetCurrentRow(); }

	virtual bool IsRecording() const override;
	virtual bool CanMoveToNextStage() const override;
	virtual void MoveToNextStage() override;
	virtual void OnRecorderEntered() override;
	virtual void OnStartedStageRecording() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data")
	TObjectPtr<UDataTable> PrefabTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data")
	TObjectPtr<AActor> PrefabSpawnPoint;
	UPROPERTY()
	TObjectPtr<APrefabActor> CurrentPrefabActor;
	
	struct PrefabIterator
	{
		int32 CurrentPointer;
		TArray<FName> RowNames;
		
		void Setup(const UDataTable* _table) { CurrentPointer = 0; RowNames = _table->GetRowNames(); }
		void Next() { ++CurrentPointer; }
		bool IsPassedAll() const { return CurrentPointer >= RowNames.Num(); }
		bool IsLast() const { return CurrentPointer >= RowNames.Num() - 1; }
		FName GetCurrentRow() const;
		TestPrefabUnit_F* GetPrefab(UDataTable* _table) const;
	} PrefabIterator;

	virtual void OnFinishedStageRecording() override;
};

