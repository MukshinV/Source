// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/DP_LevelPerfomanceRecorder_ACC.h"

#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"
#include "QA/Perfomance/PerfomanceTestUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceRecorder, All, All)

UDP_LevelPerfomanceRecorder_ACC::UDP_LevelPerfomanceRecorder_ACC()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDP_LevelPerfomanceRecorder_ACC::PostInitProperties()
{
	Super::PostInitProperties();
	PointRecorder = NewObject<UDP_PointPerfomanceRecorder>(this);
}

void UDP_LevelPerfomanceRecorder_ACC::BeginPerfomanceRecording(const TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>& _levelRegions)
{
	if(!GetWorld()) return;
	if(!GetOwner()) return;

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

void UDP_LevelPerfomanceRecorder_ACC::TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction)
{
	Super::TickComponent(_deltaTime, _tickType, _thisTickFunction);

	if(!bIsRecording) return;

	if(PointRecorder->IsRegionRecording())
	{
		PointRecorder->UpdateTestMetrics(_deltaTime);
		return;
	}

	LevelTestResult.RegionDatas[CurrentRegionIndex] = PointRecorder->CollectTestMetrics();

	if(!NextRegion())
	{
		EndPerfomanceRecording();
	}
}

void UDP_LevelPerfomanceRecorder_ACC::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
	if(bIsRecording)
	{
		EndPerfomanceRecording();	
	}
	
	Super::EndPlay(_endPlayReason);
}

void UDP_LevelPerfomanceRecorder_ACC::EndPerfomanceRecording()
{
	bIsRecording = false;

	float sumFps = 0.0f;
	for(int32 i = 0; i < LevelTestResult.RegionDatas.Num(); ++i)
	{
		sumFps += LevelTestResult.RegionDatas[i].AverageFPS;
	}

	LevelTestResult.AverageFPS = sumFps / static_cast<float>(LevelTestResult.RegionDatas.Num());

	float maxDelta = 0.0f;
	for(int32 i = 0; i < LevelTestResult.RegionDatas.Num(); ++i)
	{
		maxDelta = FMath::Max(maxDelta, LevelTestResult.RegionDatas[i].MaxFPSDelta);
	}
	
	LevelTestResult.MaxFPSDelta = maxDelta;
	
	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Stopped recording"))
}

bool UDP_LevelPerfomanceRecorder_ACC::NextRegion()
{
	if(CurrentRegionIndex == LevelRegions.Num() - 1) return false;

	++CurrentRegionIndex;

	MoveOwnerToNextRegion();
	PointRecorder->StartRecordingRegion(LevelRegions[CurrentRegionIndex]);
	
	return true;
}

void UDP_LevelPerfomanceRecorder_ACC::MoveOwnerToNextRegion()
{
	AActor* owner = GetOwner();
	check(owner)
	
	const ADP_PerfomancePoint_Actor* targetRegion = LevelRegions[CurrentRegionIndex];
	const FVector targetLocation = targetRegion->GetActorLocation();
	const FRotator targetRotation = targetRegion->GetActorRotation();

	//@TODO: add smooth transition

	owner->SetActorLocation(targetLocation);
	owner->SetActorRotation(targetRotation);
}
