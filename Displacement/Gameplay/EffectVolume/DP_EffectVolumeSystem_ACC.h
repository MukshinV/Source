// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DP_EffectVolumeReceiver_ACC.h"
#include "DP_EffectVolume_CA.h"
#include "Components/ActorComponent.h"
#include "DP_EffectVolumeSystem_ACC.generated.h"


USTRUCT(BlueprintType)
struct FDP_EffectVolumeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@ReceiversArrayC")
	TArray<UDP_EffectVolumeReceiver_ACC*> ReceiversArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@VolumeLocationC")
	FVector VolumeLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@VolumeBoundsC")
	FVector VolumeBounds;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DISPLACEMENT_API UDP_EffectVolumeSystem_ACC : public UActorComponent
{
	GENERATED_BODY()

public:
	UDP_EffectVolumeSystem_ACC();

	UFUNCTION(BlueprintCallable, DisplayName="!RegisterEffectVolume(C)(DP_EffectVolume_CA)")
	void RegisterEffectVolume(ADP_EffectVolume_CA* _effectVolume);
	UFUNCTION(BlueprintCallable, DisplayName="!UnregisterEffectVolume(C)(DP_EffectVolume_CA)")
	void UnregisterEffectVolume(ADP_EffectVolume_CA* _effectVolume);

	UFUNCTION(BlueprintCallable, DisplayName="!RegisterEffectVolumeReceiver(C)(DP_EffectVolumeReceiver_ACC)")
	void RegisterEffectVolumeReceiver(UDP_EffectVolumeReceiver_ACC* _effectVolumeReceiver);
	UFUNCTION(BlueprintCallable, DisplayName="!UnregisterEffectVolumeReceiver(C)(DP_EffectVolumeReceiver_ACC)")
	void UnregisterEffectVolumeReceiver(UDP_EffectVolumeReceiver_ACC* _effectVolumeReceiver);
	
	UFUNCTION(BlueprintCallable, DisplayName="!UpdateVolumeReceivers(C)(DP_EffectVolume_CA)")
	void UpdateVolumeReceivers(ADP_EffectVolume_CA* _effectVolume);
	UFUNCTION(BlueprintCallable, DisplayName="!UpdateVolumeReceiver(C)(DP_EffectVolume_CA, DP_EffectVolumeReceiver_ACC)")
	void UpdateVolumeReceiver(UDP_EffectVolumeReceiver_ACC* _effectVolumeReceiver, const FVector& _volumeLocation, const FVector& _volumeBounds);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@VolumeReceiversC")
	TMap<FString, FDP_EffectVolumeData> VolumeReceivers;

};
