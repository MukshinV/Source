#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomancePoint, All, All)

ADP_PerfomancePoint_Actor::ADP_PerfomancePoint_Actor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Test Point"));
}

void ADP_PerfomancePoint_Actor::BeginPlay()
{
	Super::BeginPlay();
	
	PrimaryActorTick.SetTickFunctionEnable(false);
}

void ADP_PerfomancePoint_Actor::OnStartedStageRecording()
{
	UE_LOG(LogPerfomancePoint, Log, TEXT("Started recording perfomance point stage: %s"), *GetRegionName().ToString());
	
	Timer.Reset();
	Timer.SetWaitDuration(WaitAmountSeconds);

	SetActorTickEnabled(true);
}

void ADP_PerfomancePoint_Actor::Tick(float _deltaSeconds)
{
	Super::Tick(_deltaSeconds);

	Timer.AddDeltaTime(_deltaSeconds);

	if(!IsRecording())
	{
		OnFinishedStageRecording();
	}
}

void ADP_PerfomancePoint_Actor::OnFinishedStageRecording()
{
	SetActorTickEnabled(false);
}


