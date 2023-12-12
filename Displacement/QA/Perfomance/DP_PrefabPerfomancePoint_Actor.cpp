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

	check(PrefabTable)
	check(PrefabSpawnPoint)
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
	check(world)

	const TestPrefabUnit_F* unit = PrefabIterator.GetPrefab(PrefabTable);
	check(unit)

	const FTransform& spawnTransform = PrefabSpawnPoint->GetActorTransform();
	
	CurrentPrefabActor = UPrefabricatorBlueprintLibrary::SpawnPrefab(world, unit->PrefabUnit, spawnTransform, 0);
}

inline void ADP_PrefabPerfomancePoint_Actor::OnFinishedStageRecording()
{
	CurrentPrefabActor->Destroy();
}

FName ADP_PrefabPerfomancePoint_Actor::PrefabIterator::GetCurrentRow() const
{
	if(IsPassedAll()) return FName{};

	return RowNames[CurrentPointer];
}

TestPrefabUnit_F* ADP_PrefabPerfomancePoint_Actor::PrefabIterator::GetPrefab(UDataTable* _targetTable) const
{
	if(IsPassedAll()) return nullptr;

	const FName targetRow = RowNames[CurrentPointer];
	return _targetTable->FindRow<FDP_PerfomanceTestPrefabUnit_QStruct>(targetRow, "");
}

