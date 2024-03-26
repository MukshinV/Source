// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_EffectVolumeReceiver_ACC.h"

#include "DP_EffectVolumeSystem_ACC.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UDP_EffectVolumeReceiver_ACC::UDP_EffectVolumeReceiver_ACC()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDP_EffectVolumeReceiver_ACC::BeginPlay()
{
	Super::BeginPlay();

	RegisterInEffectVolumeSystem();
}

void UDP_EffectVolumeReceiver_ACC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterFromEffectVolumeSystem();
	
	Super::EndPlay(EndPlayReason);
}

void UDP_EffectVolumeReceiver_ACC::RegisterInEffectVolumeSystem()
{
	UDP_EffectVolumeSystem_ACC* effectVolumeSystem = TryGetEffectVolumeSystem();
	if(!effectVolumeSystem) return;

	effectVolumeSystem->RegisterEffectVolumeReceiver(this);
}

void UDP_EffectVolumeReceiver_ACC::UnregisterFromEffectVolumeSystem()
{
	UDP_EffectVolumeSystem_ACC* effectVolumeSystem = TryGetEffectVolumeSystem();
	if(!effectVolumeSystem) return;

	effectVolumeSystem->UnregisterEffectVolumeReceiver(this);
}

void UDP_EffectVolumeReceiver_ACC::UpdateEffectAppearance(const FVector& _volumePosition, const FVector& _volumeBounds)
{
	VolumePosition = _volumePosition;
	VolumeBounds = _volumeBounds;

	OnVolumePositionChanged();
	OnVolumeBoundsChanged();
}

void UDP_EffectVolumeReceiver_ACC::OnVolumePositionChanged_Implementation()
{
}

void UDP_EffectVolumeReceiver_ACC::OnVolumeBoundsChanged_Implementation()
{
}

UDP_EffectVolumeSystem_ACC* UDP_EffectVolumeReceiver_ACC::TryGetEffectVolumeSystem() const
{
	const UWorld* world = GetWorld();
	const AGameModeBase* gameMode = UGameplayStatics::GetGameMode(world);

	if(!gameMode) return nullptr;

	UDP_EffectVolumeSystem_ACC* effectVolumeSystem = gameMode->FindComponentByClass<UDP_EffectVolumeSystem_ACC>();

	return effectVolumeSystem;
}



