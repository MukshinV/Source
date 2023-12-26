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

UCLASS(Abstract)
class DISPLACEMENT_API ADP_PrefabPerfomancePoint_Actor : public ADP_PerfomancePoint_Actor
{
	GENERATED_BODY()
public:
	virtual FName GetRegionName() const override;

	virtual bool IsValidForTesting() const override; 
	virtual bool IsRecording() const override;
	virtual bool CanMoveToNextStage() const override;
	virtual void MoveToNextStage() override;
	virtual void OnRecorderEntered(AActor* _recorder) override;
	virtual void OnStartedStageRecording() override;

	virtual AActor* GetSpawnPoint() { return nullptr; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data", DisplayName="@PrefabTable")
	TObjectPtr<UDataTable> PrefabTable;
	UPROPERTY()
	TObjectPtr<APrefabActor> CurrentPrefabActor;
	
	struct PrefabIterator
	{
		uint32 CurrentPointer;
		TArray<FName> RowNames;
		
		void Setup(const UDataTable* _table) { CurrentPointer = 0u; RowNames = _table->GetRowNames(); }
		void Next() { ++CurrentPointer; }
		bool IsPassedAll() const { return CurrentPointer >= static_cast<uint32>(RowNames.Num()); }
		bool IsLast() const { return CurrentPointer >= static_cast<uint32>(RowNames.Num() - 1); }
		FName GetCurrentRow() const;
		FName GetPreviousRow() const { return CurrentPointer == 0 ? FName{} : RowNames[CurrentPointer - 1]; }
		TestPrefabUnit_F* GetPrefab(const UDataTable* _table) const;
	} PrefabIterator;

	virtual void OnFinishedStageRecording() override;
};

