#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomancePoint, All, All)

ADP_PerfomancePoint_Actor::ADP_PerfomancePoint_Actor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Test Point"));
}

void ADP_PerfomancePoint_Actor::OnStartedStageRecording()
{
	UE_LOG(LogPerfomancePoint, Log, TEXT("Started recording perfomance point stage: %s"), *GetRegionName().ToString())
	
	Timer.Reset();
	Timer.SetWaitDuration(WaitAmountSeconds);
}

void ADP_PerfomancePoint_Actor::Tick(float _deltaSeconds)
{
	Super::Tick(_deltaSeconds);

	if(!IsRecording()) return;
	
	Timer.AddDeltaTime(_deltaSeconds);
}


