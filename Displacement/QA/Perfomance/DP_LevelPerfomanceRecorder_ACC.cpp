// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/DP_LevelPerfomanceRecorder_ACC.h"

#include "DP_PerfomancePointCollection_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceRecorder, All, All)

ADP_PerfomancePoint_Actor* FLevelPointsIterator::GetCurrent() const
{
	// Возвращаю только точку, пока не плавного перехода
	return CurrentRegionIndex >= static_cast<uint32>(LevelPoints.Num()) ? nullptr : LevelPoints[CurrentRegionIndex].Point;
}

UDP_LevelPerfomanceRecorder_ACC::UDP_LevelPerfomanceRecorder_ACC()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDP_LevelPerfomanceRecorder_ACC::PostInitProperties()
{
	Super::PostInitProperties();
	PointRecorder = NewObject<UDP_PointPerfomanceRecorder>(this);
}

void UDP_LevelPerfomanceRecorder_ACC::BeginPerfomanceRecording(const TArray<FPerfomancePointData>& _levelRegions)
{
	check(GetWorld())
	check(GetOwner())

	const FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	
	if(_levelRegions.Num() == 0)
	{
		UE_LOG(LogPerfomanceRecorder, Log, TEXT("Level with name %s has no perfomance points"), *levelName)
		return;
	}

	LevelTestResult.LevelName = levelName;
	
	LevelPointsIterator.ResetPointer();
	LevelPointsIterator.SetNewPointsArray(_levelRegions);

	MoveOwnerToNextRegion();
	ADP_PerfomancePoint_Actor* point = LevelPointsIterator.GetCurrent();
	PointRecorder->SetRecordingPoint(point);
	PointRecorder->EnterRecordingPoint();

	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Started recording"))
}

void UDP_LevelPerfomanceRecorder_ACC::CollectPointRecordingResults()
{
	LevelTestResult.RegionDatas.Add(PointRecorder->CollectTestMetrics());
}

bool UDP_LevelPerfomanceRecorder_ACC::TryToSwitchToNextPoint()
{
	PointRecorder->ExitRecordingPoint();
	
	if(!LevelPointsIterator.Next()) return false;

	MoveOwnerToNextRegion();
	ADP_PerfomancePoint_Actor* point = LevelPointsIterator.GetCurrent();
	PointRecorder->SetRecordingPoint(point);
	PointRecorder->EnterRecordingPoint();
	
	return true;
}

void UDP_LevelPerfomanceRecorder_ACC::TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction)
{
	Super::TickComponent(_deltaTime, _tickType, _thisTickFunction);

	if(LevelPointsIterator.PassedAll()) return;
	
	if(PointRecorder->IsRegionRecording())
	{
		PointRecorder->UpdateTestMetrics(_deltaTime);
		return;
	}

	CollectPointRecordingResults();

	if(PointRecorder->CanMoveToNextStage())
	{
		PointRecorder->MoveToNextPointStage();
		return;
	}
	
	if(TryToSwitchToNextPoint()) return;
	
	EndPerfomanceRecording();
}

void UDP_LevelPerfomanceRecorder_ACC::EndPerfomanceRecording()
{
	LevelTestResult.PerfomanceTestDate = FDateTime::Now().ToString();
	
	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Stopped recording"))

	FinishedRecordingEvent.Broadcast(LevelTestResult);
}

void UDP_LevelPerfomanceRecorder_ACC::MoveOwnerToNextRegion() const
{
	AActor* owner = GetOwner();
	check(owner)
	
	const ADP_PerfomancePoint_Actor* targetPoint = LevelPointsIterator.GetCurrent();
	const FVector targetLocation = targetPoint->GetActorLocation();
	const FRotator targetRotation = targetPoint->GetActorRotation();

	//@TODO: add smooth transition

	owner->SetActorLocation(targetLocation);
	owner->SetActorRotation(targetRotation);
}