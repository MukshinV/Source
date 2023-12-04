// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/DP_PerfomanceRecorder_ACC.h"

#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"
#include "QA/Perfomance/PerfomanceTestUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceRecorder, All, All)

UDP_PerfomanceRecorder_ACC::UDP_PerfomanceRecorder_ACC()
{
	PrimaryComponentTick.bCanEverTick = true;
	WaitDurationSeconds = 2.0f;
}

void UDP_PerfomanceRecorder_ACC::SetWaitDuration(float _newWaitingDuration)
{
	check(_newWaitingDuration >= 0.0f)
	WaitDurationSeconds = _newWaitingDuration;
}

void UDP_PerfomanceRecorder_ACC::BeginPerfomanceRecording(const TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>& _levelRegions)
{
	check(GetWorld());
	check(GetOwner());

	const FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	
	if(_levelRegions.Num() == 0)
	{
		UE_LOG(LogPerfomanceRecorder, Log, TEXT("Level with name %s has no perfomance points"), *levelName)
		return;
	}
	
	LevelRegions = _levelRegions;
	CurrentRegionIndex = -1;

	LevelTestResult.LevelName = levelName;
	LevelTestResult.RegionDatas.SetNum(LevelRegions.Num());

	NextRegion();

	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Started recording"))
	
	bIsRecording = true;
}

void UDP_PerfomanceRecorder_ACC::TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction)
{
	Super::TickComponent(_deltaTime, _tickType, _thisTickFunction);

	if(!bIsRecording) return;

	if(TimePassed < WaitDurationSeconds)
	{
		TimePassed += _deltaTime;
		UpdateTestMetrics();
		return;
	}

	CollectTestMetrics();

	if(!NextRegion())
	{
		EndPerfomanceRecording();
	}
}

void UDP_PerfomanceRecorder_ACC::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
	if(bIsRecording)
	{
		EndPerfomanceRecording();	
	}
	
	Super::EndPlay(_endPlayReason);
}

void UDP_PerfomanceRecorder_ACC::EndPerfomanceRecording()
{
	bIsRecording = false;

	float sumFps = 0.0f;
	for(int32 i = 0; i < LevelTestResult.RegionDatas.Num(); ++i)
	{
		sumFps += LevelTestResult.RegionDatas[i].AverageFPS;
	}

	LevelTestResult.AverageFPS = sumFps / static_cast<float>(LevelTestResult.RegionDatas.Num());

	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Stopped recording"))
}

bool UDP_PerfomanceRecorder_ACC::NextRegion()
{
	if(CurrentRegionIndex == LevelRegions.Num()) return false;

	++CurrentRegionIndex;
	TimePassed = 0.0f;
	
	return true;
}

void UDP_PerfomanceRecorder_ACC::MoveOwnerToNextRegion()
{
	AActor* owner = GetOwner();
	check(owner)
	
	const ADP_PerfomancePoint_Actor* targetRegion = LevelRegions[CurrentRegionIndex];
	const FVector targetLocation = targetRegion->GetActorLocation();

	//@TODO: add smooth transition

	owner->SetActorLocation(targetLocation);
}

void UDP_PerfomanceRecorder_ACC::UpdateTestMetrics()
{
	++TickCounter;
}

void UDP_PerfomanceRecorder_ACC::CollectTestMetrics()
{
	const float averageFps = static_cast<float>(TickCounter) / WaitDurationSeconds;

	LevelTestResult.RegionDatas[CurrentRegionIndex].AverageFPS = averageFps;
	
	TickCounter = 0;
}
