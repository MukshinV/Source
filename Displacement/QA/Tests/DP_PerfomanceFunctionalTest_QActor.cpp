

#include "QA/Tests/DP_PerfomanceFunctionalTest_QActor.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceFunctionalTest, All, All)

ADP_PerfomanceFunctionalTest_QActor::ADP_PerfomanceFunctionalTest_QActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADP_PerfomanceFunctionalTest_QActor::PrepareTest()
{
	Super::PrepareTest();

	UWorld* world = GetWorld();
	if(!world)
	{
		FinishTest(EFunctionalTestResult::Failed, "[FAILED] PerfomanceFunctionalTest: World does not exist");
		return;
	}

	TArray<AActor*> foundActors{};
	UGameplayStatics::GetAllActorsOfClass(world, ADP_PerfomancePoint_Actor::StaticClass(), foundActors);
	
	if(foundActors.Num() == 0)
	{
		FString levelName = UGameplayStatics::GetCurrentLevelName(world);
		UE_LOG(LogPerfomanceFunctionalTest, Warning, TEXT("No perfomance points was found at level %s"), *levelName);
	}

	LevelPerfomancePoints.SetNum(foundActors.Num());
	for (int32 i = 0; i < foundActors.Num(); ++i)
	{
		LevelPerfomancePoints[i] = Cast<ADP_PerfomancePoint_Actor>(foundActors[i]);
	}
}

void ADP_PerfomanceFunctionalTest_QActor::StartTest()
{
	Super::StartTest();

	UWorld* world = GetWorld();
	APawn* pawn = UGameplayStatics::GetPlayerPawn(world, 0);
	if(!pawn)
	{
		FinishTest(EFunctionalTestResult::Failed, "[FAILED] PerfomanceFunctionalTest: Player pawn does not exist");
		return;
	}

	PerfomanceRecorder = Cast<UDP_PerfomanceRecorder_ACC>(pawn->AddComponentByClass(UDP_PerfomanceRecorder_ACC::StaticClass(), true, FTransform::Identity, true));
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
		FPerfomanceTestData generalTestData{};
		FPerfomanceTestLevelData levelResult = PerfomanceRecorder->GetTestResult();
		generalTestData.LevelDatas.Add(levelResult);

		UWorld* world = GetWorld();
		if(world)
		{
			Displacement::Test::WritePerfomanceTestData(UGameplayStatics::GetCurrentLevelName(GetWorld()), generalTestData);
		}
	}
				
	UE_LOG(LogPerfomanceFunctionalTest, Warning, TEXT("Perfomance test finished"));
	
	Super::FinishTest(TestResult, Message);
}
