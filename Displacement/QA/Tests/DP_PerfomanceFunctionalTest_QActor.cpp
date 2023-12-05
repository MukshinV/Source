

#include "QA/Tests/DP_PerfomanceFunctionalTest_QActor.h"

#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceFunctionalTest, All, All)

ADP_PerfomanceFunctionalTest_QActor::ADP_PerfomanceFunctionalTest_QActor()
{
	PrimaryActorTick.bCanEverTick = true;
	TimeLimit = 0.0f;
}

void ADP_PerfomanceFunctionalTest_QActor::PrepareTest()
{
	Super::PrepareTest();

	CollectPerfomanceTestPoints();
	SetupActors();
}


void ADP_PerfomanceFunctionalTest_QActor::CollectPerfomanceTestPoints()
{
	UWorld* world = GetWorld();
	if(!world)
	{
		FinishTest(EFunctionalTestResult::Failed, "[FAILED] PerfomanceFunctionalTest: World does not exist");
		return;
	}

	for (TActorIterator<ADP_PerfomancePoint_Actor> It(world, ADP_PerfomancePoint_Actor::StaticClass()); It; ++It)
	{
		LevelPerfomancePoints.Add(*It);
	}
	
	if(LevelPerfomancePoints.Num() == 0)
	{
		FString levelName = UGameplayStatics::GetCurrentLevelName(world);
		UE_LOG(LogPerfomanceFunctionalTest, Warning, TEXT("No perfomance points was found at level %s"), *levelName);
	}
}

void ADP_PerfomanceFunctionalTest_QActor::SetupActors()
{
	UWorld* world = GetWorld();
	
	APlayerController* playerController = world->GetFirstPlayerController();

	if(!playerController)
	{
		FinishTest(EFunctionalTestResult::Failed, "[FAILED] PerfomanceFunctionalTest: Player controller does not exist");
		return;
	}

	playerController->DisableInput(playerController);
	
	APawn* pawn = world->SpawnActor<APawn>(TestPawnSubclass, FVector{}, FRotator{}, {});
	
	if(!pawn)
	{
		FinishTest(EFunctionalTestResult::Failed, "[FAILED] PerfomanceFunctionalTest: Player pawn does not exist");
		return;
	}

	playerController->Possess(pawn);

	UCharacterMovementComponent* movementComponent = pawn->FindComponentByClass<UCharacterMovementComponent>();
	if(movementComponent)
	{
		movementComponent->GravityScale = 0.0f;
	}
	
	PerfomanceRecorder = Cast<UDP_PerfomanceRecorder_ACC>(pawn->AddComponentByClass(UDP_PerfomanceRecorder_ACC::StaticClass(), false, FTransform::Identity, false));
	PerfomanceRecorder->BeginPerfomanceRecording(LevelPerfomancePoints);
}


void ADP_PerfomanceFunctionalTest_QActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!IsRunning()) return;

	if(!PerfomanceRecorder->IsRecording())
	{
		FinishTest(EFunctionalTestResult::Succeeded, "[PASSED] PerfomanceFunctionalTest: All perfomance points was iterated");
	}
}

void ADP_PerfomanceFunctionalTest_QActor::FinishTest(EFunctionalTestResult TestResult, const FString& Message)
{
	if(PerfomanceRecorder)
	{
		FPerfomanceTestLevelData levelResult = PerfomanceRecorder->GetTestResult();

		UWorld* world = GetWorld();
		if(world)
		{
			Displacement::Test::WritePerfomanceTestData(UGameplayStatics::GetCurrentLevelName(GetWorld()), levelResult);
		}
	}
				
	UE_LOG(LogPerfomanceFunctionalTest, Log, TEXT("Perfomance test finished"));
	
	Super::FinishTest(TestResult, Message);
}
