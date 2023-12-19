// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_PrefabPerfomancePoint_Actor.h"

#include "Prefab/PrefabActor.h"
#include "PrefabricatorRuntime/Public/Utils/PrefabricatorFunctionLibrary.h"

ADP_PrefabPerfomancePoint_Actor::ADP_PrefabPerfomancePoint_Actor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADP_PrefabPerfomancePoint_Actor::BeginPlay()
{
	Super::BeginPlay();

	check(PrefabTable);
	check(PrefabSpawnPoint);
}

FName ADP_PrefabPerfomancePoint_Actor::GetRegionName() const
{
	if(IsRecording())
	{
		return PrefabIterator.GetCurrentRow();
	}

	return Super::GetRegionName();
}

bool ADP_PrefabPerfomancePoint_Actor::IsRecording() const
{
	return Super::IsRecording() && CurrentPrefabActor;
}

bool ADP_PrefabPerfomancePoint_Actor::CanMoveToNextStage() const
{
	return !PrefabIterator.IsLast();
}

void ADP_PrefabPerfomancePoint_Actor::MoveToNextStage()
{
	PrefabIterator.Next();
}

void ADP_PrefabPerfomancePoint_Actor::OnRecorderEntered()
{
	PrefabIterator.Setup(PrefabTable);
}

void ADP_PrefabPerfomancePoint_Actor::OnStartedStageRecording()
{
	Super::OnStartedStageRecording();

	const UWorld* world = GetWorld();
	const TestPrefabUnit_F* unit = PrefabIterator.GetPrefab(PrefabTable);
	check(unit);

	const FTransform& spawnTransform = PrefabSpawnPoint->GetActorTransform();
	
	CurrentPrefabActor = UPrefabricatorBlueprintLibrary::SpawnPrefab(world, unit->PrefabUnit, spawnTransform, 0);
}

void ADP_PrefabPerfomancePoint_Actor::OnFinishedStageRecording()
{
	CurrentPrefabActor->Destroy();

	Super::OnFinishedStageRecording();
}

FName ADP_PrefabPerfomancePoint_Actor::PrefabIterator::GetCurrentRow() const
{
	return CurrentPointer >= static_cast<uint32>(RowNames.Num()) ? FName{} : RowNames[CurrentPointer];
}

TestPrefabUnit_F* ADP_PrefabPerfomancePoint_Actor::PrefabIterator::GetPrefab(const UDataTable* _targetTable) const
{
	const FName targetRow = GetCurrentRow();
	return _targetTable->FindRow<FDP_PerfomanceTestPrefabUnit_QStruct>(targetRow, "");
}

