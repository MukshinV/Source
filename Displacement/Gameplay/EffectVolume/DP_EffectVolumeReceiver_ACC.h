// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DP_EffectVolumeReceiver_ACC.generated.h"

class UDP_EffectVolumeSystem_ACC;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DISPLACEMENT_API UDP_EffectVolumeReceiver_ACC : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@TargetVolumeHandleC")
	FDataTableRowHandle TargetVolumeHandle;
	
	UDP_EffectVolumeReceiver_ACC();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, DisplayName="!RegisterSelfInEffectVolumeSystem(C)")
	void RegisterInEffectVolumeSystem();
	UFUNCTION(BlueprintCallable, DisplayName="!UnregisterSelfFromEffectVolumeSystem(C)")
	void UnregisterFromEffectVolumeSystem();

	UFUNCTION(BlueprintCallable, DisplayName="!UpdateEffectAppearance(C)(FVector, FVector)")
	void UpdateEffectAppearance(const FVector& _volumePosition, const FVector& _volumeBounds);

	UFUNCTION(BlueprintNativeEvent, DisplayName="!OnVolumePositionChanged(Virtual)")
	void OnVolumePositionChanged();
	UFUNCTION(BlueprintNativeEvent, DisplayName="!OnVolumeBoundsChanged(Virtual)")
	void OnVolumeBoundsChanged();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@VolumePositionC")
	FVector VolumePosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="@VolumeBoundsC")
	FVector VolumeBounds;

private:
	UDP_EffectVolumeSystem_ACC* TryGetEffectVolumeSystem() const;
};
