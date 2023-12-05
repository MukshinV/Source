

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

	TArray<TObjectPtr<ADP_PerfomancePoint_Actor>> levelPerfomancePoints{};
	
	CollectPerfomanceTestPoints(levelPerfomancePoints);
	SetupActors(levelPerfomancePoints);
	check(PerfomanceRecorder)

	PerfomanceRecorder->OnFinishedRecording().AddUObject(this, &ADP_PerfomanceFunctionalTest_QActor::OnTestFinished);
	PerfomanceRecorder->BeginPerfomanceRecording(levelPerfomancePoints);
}

void ADP_PerfomanceFunctionalTest_QActor::CollectPerfomanceTestPoints(PerfPointsArray_T& _perfomancePoints) const
{
	check(GetWorld());

	for (TActorIterator<ADP_PerfomancePoint_Actor> It(GetWorld(), ADP_PerfomancePoint_Actor::StaticClass()); It; ++It)
	{
		_perfomancePoints.Add(*It);
	}
	
	if(_perfomancePoints.Num() == 0)
	{
		FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		UE_LOG(LogPerfomanceFunctionalTest, Warning, TEXT("No perfomance points was found at level %s"), *levelName);
	}
}

void ADP_PerfomanceFunctionalTest_QActor::SetupActors(PerfPointsArray_T& _perfomancePoints)
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
	
	PerfomanceRecorder = Cast<UDP_LevelPerfomanceRecorder_ACC>(pawn->AddComponentByClass(UDP_LevelPerfomanceRecorder_ACC::StaticClass(), false, FTransform::Identity, false));
}

void ADP_PerfomanceFunctionalTest_QActor::OnTestFinished(FPerfomanceTestLevelData _result)
{
	check(GetWorld())

	Displacement::Test::WritePerfomanceTestData(UGameplayStatics::GetCurrentLevelName(GetWorld()), _result);
	FinishTest(EFunctionalTestResult::Succeeded, "[PASSED] PerfomanceFunctionalTest: All perfomance points was iterated");
}
