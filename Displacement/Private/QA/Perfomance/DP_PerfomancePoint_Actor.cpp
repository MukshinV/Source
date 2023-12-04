#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"


ADP_PerfomancePoint_Actor::ADP_PerfomancePoint_Actor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Test Point"));
}


