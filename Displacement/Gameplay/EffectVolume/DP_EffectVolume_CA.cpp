// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_EffectVolume_CA.h"

#include "DP_EffectVolumeSystem_ACC.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"


ADP_EffectVolume_CA::ADP_EffectVolume_CA()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(L"EffectVolumeMesh");
}

void ADP_EffectVolume_CA::BeginPlay()
{
	Super::BeginPlay();

	RegisterInEffectVolumeSystem();
}

void ADP_EffectVolume_CA::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterFromEffectVolumeSystem();
	
	Super::EndPlay(EndPlayReason);
}

void ADP_EffectVolume_CA::RegisterInEffectVolumeSystem()
{
	UDP_EffectVolumeSystem_ACC* effectVolumeSystem = TryGetEffectVolumeSystem();
	if(!effectVolumeSystem) return;

	effectVolumeSystem->RegisterEffectVolume(this);
}

void ADP_EffectVolume_CA::UnregisterFromEffectVolumeSystem()
{
	UDP_EffectVolumeSystem_ACC* effectVolumeSystem = TryGetEffectVolumeSystem();
	if(!effectVolumeSystem) return;

	effectVolumeSystem->UnregisterEffectVolume(this);
}

FVector ADP_EffectVolume_CA::GetVolumeBounds() const
{
	FVector min{};
	FVector max{};
	
	StaticMeshComponent->GetLocalBounds(min, max);

	const FVector boundsSize = (max - min).GetAbs();
	const FVector meshScale = StaticMeshComponent->GetComponentScale();

	return boundsSize * meshScale;
}

void ADP_EffectVolume_CA::OnVolumeChanged()
{
	VolumeChangedEvent.Broadcast(this);
}

UDP_EffectVolumeSystem_ACC* ADP_EffectVolume_CA::TryGetEffectVolumeSystem() const
{
	const UWorld* world = GetWorld();
	const AGameModeBase* gameMode = UGameplayStatics::GetGameMode(world);

	if(!gameMode) return nullptr;

	UDP_EffectVolumeSystem_ACC* effectVolumeSystem = gameMode->FindComponentByClass<UDP_EffectVolumeSystem_ACC>();

	return effectVolumeSystem;
}
