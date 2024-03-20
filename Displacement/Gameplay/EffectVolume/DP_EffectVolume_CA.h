// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DP_EffectVolume_CA.generated.h"

class UDP_EffectVolumeSystem_ACC;

UCLASS(Blueprintable)
class DISPLACEMENT_API ADP_EffectVolume_CA : public AActor
{
	GENERATED_BODY()
	DECLARE_EVENT_OneParam(ADP_EffectVolume_CA, FVolumeChanged, ADP_EffectVolume_CA*)

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@VolumeHandleC")
	FDataTableRowHandle VolumeHandle;
	
	ADP_EffectVolume_CA();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FVolumeChanged& OnVolumeChangedEvent() { return VolumeChangedEvent; }
	
	UFUNCTION(BlueprintCallable, DisplayName="!RegisterSelfInEffectVolumeSystem(C)")
	void RegisterInEffectVolumeSystem();
	UFUNCTION(BlueprintCallable, DisplayName="!UnregisterSelfFromEffectVolumeSystem(C)")
	void UnregisterFromEffectVolumeSystem();
	UFUNCTION(BlueprintCallable, DisplayName="!GetVolumeBounds(C)")
	FVector GetVolumeBounds() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@StaticMeshComponentC")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UFUNCTION(BlueprintCallable, DisplayName="!OnVolumeChanged(C)")
	void OnVolumeChanged();

private:
	FVolumeChanged VolumeChangedEvent;
	UDP_EffectVolumeSystem_ACC* TryGetEffectVolumeSystem() const;
};
