// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/DP_LevelPerfomanceRecorder_ACC.h"

#include "DP_PerfomancePointCollection_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceRecorder, All, All)

const FPerfomancePointData* FLevelPointsIterator::GetCurrent() const
{
	return CurrentRegionIndex >= static_cast<uint32>(LevelPoints.Num()) ? nullptr : &LevelPoints[CurrentRegionIndex];
}

const FPerfomancePointData* FLevelPointsIterator::PeekPrevious()
{
	if(CurrentRegionIndex == 0) return nullptr;

	return &LevelPoints[CurrentRegionIndex - 1];
}

void FPerfomancePointTransitioner::MoveToInterpolatedTransform(AActor* _targetActorToMove) const
{
	check(_targetActorToMove);
	
	_targetActorToMove->SetActorLocation(PositionInterpolator.GetInterpolatedData());
	_targetActorToMove->SetActorRotation(RotationInterpolator.GetInterpolatedData().Quaternion());
}

void FPerfomancePointTransitioner::SetInterpolationValue(float _interpolationValue)
{
	PositionInterpolator.SetInterpolationValue(_interpolationValue);
	RotationInterpolator.SetInterpolationValue(_interpolationValue);
}

const FPerfomanceTestRegionMetrics& FPerfomancePointTransitioner::CollectTransitionMetrics()
{
	TransitionMetrics.TicksPerSecond = MetricsCollector.GetTickAmount() / TransitionDuration;
	TransitionMetrics.MaxFPSDelta = MetricsCollector.GetMaxFPSDelta();
	return TransitionMetrics;
}

void FPerfomancePointTransitioner::StartTransition(const ADP_PerfomancePoint_Actor* _fromPoint, const ADP_PerfomancePoint_Actor* _toPoint, float _transitionDuration)
{
	check(_fromPoint);
	check(_toPoint);

	Reset();

	TransitionDuration = _transitionDuration;
	MetricsCollector.StartCollect();

	const FString fromPointName = _fromPoint->GetRegionName().ToString();
	const FString toPointName = _toPoint->GetRegionName().ToString();
	TransitionMetrics.RegionName = FString::Printf(TEXT("Transition:%s--%s"), *fromPointName, *toPointName);

	const FVector fromPosition = _fromPoint->GetActorLocation();
	const FVector toPosition = _toPoint->GetActorLocation();

	PositionInterpolator.SetData(fromPosition, toPosition);

	const FRotator fromRotation = _fromPoint->GetActorRotation();
	const FRotator toRotation = _toPoint->GetActorRotation();

	RotationInterpolator.SetData(fromRotation, toRotation);
}

void FPerfomancePointTransitioner::Tick(float _deltaTime)
{
	MetricsCollector.Tick(_deltaTime);

	TimePassed += _deltaTime;
	SetInterpolationValue(TimePassed / TransitionDuration);
}

UDP_LevelPerfomanceRecorder_ACC::UDP_LevelPerfomanceRecorder_ACC()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDP_LevelPerfomanceRecorder_ACC::PostInitProperties()
{
	Super::PostInitProperties();
	PointRecorder = NewObject<UDP_PointPerfomanceRecorder>(this);
	SetComponentTickEnabled(false);
}

void UDP_LevelPerfomanceRecorder_ACC::EnterToCurrentPoint() const
{
	const FPerfomancePointData* pointData = LevelPointsIterator.GetCurrent();

	PointRecorder->SetRecordingPoint(pointData->Point);
	PointRecorder->EnterRecordingPoint();
}

void UDP_LevelPerfomanceRecorder_ACC::BeginPerfomanceRecording(const TArray<FPerfomancePointData>& _levelRegions)
{
	check(GetWorld());
	check(GetOwner());

	const FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	
	if(_levelRegions.Num() == 0)
	{
		UE_LOG(LogPerfomanceRecorder, Log, TEXT("Level with name %s has no perfomance points"), *levelName);
		return;
	}

	LevelTestResult.LevelName = levelName;
	
	LevelPointsIterator.ResetPointer();
	LevelPointsIterator.SetNewPointsArray(_levelRegions);

	StartMoveOwnerToNextPoint();
	SetComponentTickEnabled(true);
	
	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Started recording"));
}

void UDP_LevelPerfomanceRecorder_ACC::CollectPointRecordingResults()
{
	LevelTestResult.RegionDatas.Add(PointRecorder->CollectTestMetrics());
}

bool UDP_LevelPerfomanceRecorder_ACC::TryToSwitchToNextPoint()
{
	PointRecorder->ExitRecordingPoint();
	
	if(!LevelPointsIterator.Next()) return false;

	StartMoveOwnerToNextPoint();
	return true;
}

void UDP_LevelPerfomanceRecorder_ACC::TickComponent(float _deltaTime, ELevelTick _tickType, FActorComponentTickFunction* _thisTickFunction)
{
	Super::TickComponent(_deltaTime, _tickType, _thisTickFunction);

	if(PointTransitioner.IsInTransition())
	{
		PointTransitioner.Tick(_deltaTime);
		PointTransitioner.MoveToInterpolatedTransform(GetOwner());
		return;
	}

	const ADP_PerfomancePoint_Actor* currentPoint = LevelPointsIterator.GetCurrent()->Point;
	if(!PointRecorder->IsOnRecordingPoint(currentPoint))
	{
		if(PointTransitioner.IsNeedToCollectMetrics())
		{
			LevelTestResult.RegionDatas.Add(PointTransitioner.CollectTransitionMetrics()); 
		}
		
		EnterToCurrentPoint();
	}
	
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
	SetComponentTickEnabled(false);
	
	LevelTestResult.PerfomanceTestDate = FDateTime::Now().ToString();
	
	UE_LOG(LogPerfomanceRecorder, Log, TEXT("Stopped recording"));

	FinishedRecordingEvent.Broadcast(LevelTestResult);
}

void UDP_LevelPerfomanceRecorder_ACC::StartMoveOwnerToNextPoint()
{
	AActor* owner = GetOwner();
	check(owner);

	const FPerfomancePointData* fromPointData = LevelPointsIterator.PeekPrevious();
	const FPerfomancePointData* toPointData = LevelPointsIterator.GetCurrent();

	const ADP_PerfomancePoint_Actor* toPoint = toPointData->Point;
	const FVector targetLocation = toPoint->GetActorLocation();
	const FRotator targetRotation = toPoint->GetActorRotation();

	if(!fromPointData)
	{
		owner->SetActorLocation(targetLocation);
		owner->SetActorRotation(targetRotation);
		return;
	}

	const ADP_PerfomancePoint_Actor* fromPoint = fromPointData->Point;
	const EPerfomancePointTransitionMode transitionMode = toPointData->TransitionToPointMode;

	PointTransitioner.StartTransition(fromPoint, toPoint, transitionMode == EPerfomancePointTransitionMode::Instant ? 0.0f : fromPointData->TransitionDuration);
	
	if(transitionMode == EPerfomancePointTransitionMode::Instant)
	{
		PointTransitioner.SetMaxInterpolationValue();
		PointTransitioner.MoveToInterpolatedTransform(owner);
	}
}