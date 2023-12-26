// Fill out your copyright notice in the Description page of Project Settings.


#include "DP_DynamicPrefabPerfomancePoint_Actor.h"

#include "PerfomanceTestUtils.h"


FName ADP_DynamicPrefabPerfomancePoint_Actor::GetRegionName() const
{
	if(IsRecording())
	{
		const FString prefabName = PrefabIterator.GetCurrentRow().ToString();
		return FName{"Entering: " + prefabName };
	}

	return ADP_PerfomancePoint_Actor::GetRegionName();
}

bool ADP_DynamicPrefabPerfomancePoint_Actor::IsRecording() const
{
	return PrefabTransitioner.IsInTransition();
}

bool ADP_DynamicPrefabPerfomancePoint_Actor::IsValidForTesting() const
{
	return RecordPointsIterator.RecordPoints.Num() >= 2 && Super::IsValidForTesting();
}

void ADP_DynamicPrefabPerfomancePoint_Actor::OnRecorderEntered(AActor* _recorder)
{
	Super::OnRecorderEntered(_recorder);
	
	Recorder = _recorder;
}

void ADP_DynamicPrefabPerfomancePoint_Actor::OnStartedStageRecording()
{
	Super::OnStartedStageRecording();

	const AActor* fromPoint = RecordPointsIterator.GetPrevious()->LocationPoint;
	const AActor* toPoint = RecordPointsIterator.Get()->LocationPoint;
	PrefabTransitioner.StartTransition(fromPoint, toPoint, {}, WaitAmountSeconds);
}

void ADP_DynamicPrefabPerfomancePoint_Actor::MoveToNextStage()
{
	Super::MoveToNextStage();
	RecordPointsIterator.Next();
}

void ADP_DynamicPrefabPerfomancePoint_Actor::Tick(float _deltaSeconds)
{
	if(PrefabTransitioner.IsInTransition())
	{
		PrefabTransitioner.Tick(_deltaSeconds);
		PrefabTransitioner.MoveToInterpolatedTransform(Recorder);
	}

	Super::Tick(_deltaSeconds);
}
