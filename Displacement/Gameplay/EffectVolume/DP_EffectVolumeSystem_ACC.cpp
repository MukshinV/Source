// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_EffectVolumeSystem_ACC.h"

#include "EventSystem/U5_EventSystem_ACC.h"
#include "Utils/U5_Utils.h"

UDP_EffectVolumeSystem_ACC::UDP_EffectVolumeSystem_ACC()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDP_EffectVolumeSystem_ACC::RegisterEffectVolume(ADP_EffectVolume_CA* _effectVolume)
{
	if(!_effectVolume) return;
	
	FString volumeHandleString{};
	UU5_EventSystem_ACC::EventHandleToString(volumeHandleString, _effectVolume->VolumeHandle);

	_effectVolume->OnVolumeChangedEvent().AddUObject(this, &ThisClass::UpdateVolumeReceivers);

	FDP_EffectVolumeData* foundData = VolumeReceivers.Find(volumeHandleString);;

	const FVector effectVolumeLocation = _effectVolume->GetActorLocation();
	const FVector effectVolumeBounds = _effectVolume->GetVolumeBounds();
	
	if(foundData)
	{
		foundData->VolumeLocation = effectVolumeLocation;
		foundData->VolumeBounds = effectVolumeBounds;
	}
	else
	{
		FDP_EffectVolumeData volumeData{};
		volumeData.VolumeLocation = effectVolumeLocation;
		volumeData.VolumeBounds = effectVolumeBounds;
		
		VolumeReceivers.Add(volumeHandleString, volumeData);
	}

	UpdateVolumeReceivers(_effectVolume);
}

void UDP_EffectVolumeSystem_ACC::UnregisterEffectVolume(ADP_EffectVolume_CA* _effectVolume)
{
	if(!_effectVolume) return;

	_effectVolume->OnVolumeChangedEvent().RemoveAll(this);
}

void UDP_EffectVolumeSystem_ACC::RegisterEffectVolumeReceiver(UDP_EffectVolumeReceiver_ACC* _effectVolumeReceiver)
{
	FString volumeHandleString{};
	UU5_EventSystem_ACC::EventHandleToString(volumeHandleString, _effectVolumeReceiver->TargetVolumeHandle);
	
	FDP_EffectVolumeData* foundData = VolumeReceivers.Find(volumeHandleString);

	if(foundData)
	{
		foundData->ReceiversArray.Add(_effectVolumeReceiver);
		_effectVolumeReceiver->UpdateEffectAppearance(foundData->VolumeLocation, foundData->VolumeBounds);
	}
	else
	{
		FDP_EffectVolumeData volumeData{};
		volumeData.ReceiversArray.Add(_effectVolumeReceiver);
		
		VolumeReceivers.Add(volumeHandleString, volumeData);
	}
}

void UDP_EffectVolumeSystem_ACC::UnregisterEffectVolumeReceiver(UDP_EffectVolumeReceiver_ACC* _effectVolumeReceiver)
{
	FString volumeHandleString{};
	UU5_EventSystem_ACC::EventHandleToString(volumeHandleString, _effectVolumeReceiver->TargetVolumeHandle);
	FDP_EffectVolumeData* foundData = VolumeReceivers.Find(volumeHandleString);


	if(!foundData) return;

	foundData->ReceiversArray.RemoveSwap(_effectVolumeReceiver);
}

void UDP_EffectVolumeSystem_ACC::UpdateVolumeReceivers(ADP_EffectVolume_CA* _effectVolume)
{
	FString volumeHandleString{};
	UU5_EventSystem_ACC::EventHandleToString(volumeHandleString, _effectVolume->VolumeHandle);
	
	FDP_EffectVolumeData* foundData = VolumeReceivers.Find(volumeHandleString);

	if(!foundData) return;

	foundData->VolumeLocation = _effectVolume->GetActorLocation();
	foundData->VolumeBounds = _effectVolume->GetVolumeBounds();

	for(int32 i = 0; i < foundData->ReceiversArray.Num(); ++i)
	{
		UpdateVolumeReceiver(foundData->ReceiversArray[i], foundData->VolumeLocation, foundData->VolumeBounds);
	}
}

void UDP_EffectVolumeSystem_ACC::UpdateVolumeReceiver(UDP_EffectVolumeReceiver_ACC* _effectVolumeReceiver, const FVector& _volumeLocation, const FVector& _volumeBounds)
{
	mU5_DEBUGOUT(true, mDNAME(_effectVolumeReceiver) << " " << _volumeLocation.ToString() << " " << _volumeBounds.ToString());
	_effectVolumeReceiver->UpdateEffectAppearance(_volumeLocation, _volumeBounds);
}

