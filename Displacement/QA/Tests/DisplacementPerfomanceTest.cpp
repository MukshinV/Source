#include "QA/Perfomance/DP_PerfomancePointCollection_Actor.h"
#if WITH_AUTOMATION_TESTS

#include "EngineUtils.h"
#include "Misc/AutomationTest.h"
#include "TestConstants.h"
#include "TestUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "QA/Perfomance/DP_LevelPerfomanceRecorder_ACC.h"
#include "QA/Perfomance/DP_PerfomancePoint_Actor.h"
#include "QA/Perfomance/PerfomanceTestUtils.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(DisplacementPerfomanceTest, "Displacement.Displacement.QA.Tests.PerfomanceTest",
                                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::PerfFilter)

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceTest, All, All)

namespace
{
	using LevelRecorder_ACC = UDP_LevelPerfomanceRecorder_ACC;
	using PointsCollection_A = ADP_PerfomancePointCollection_Actor;

	const TArray<FPerfomancePointData>& GetPerfomanceTestPoints(UWorld* _world)
	{
		AActor* foundActor = UGameplayStatics::GetActorOfClass(_world, PointsCollection_A::StaticClass());
		check(foundActor)
		const PointsCollection_A* pointsCollection = CastChecked<PointsCollection_A>(foundActor);
		return pointsCollection->GetPointsCollection();
	}

	void SetupActors(UWorld* _world, APawn* _testPawn)
	{
		APlayerController* playerController = _world->GetFirstPlayerController();
		check(playerController)

		playerController->DisableInput(playerController);
		playerController->Possess(_testPawn);

		UCharacterMovementComponent* movementComponent = _testPawn->FindComponentByClass<UCharacterMovementComponent>();
		if(movementComponent)
		{
			movementComponent->GravityScale = 0.0f;
		}
	}

	class FUntilEndOfPerfomanceTestLatentCommand : public IAutomationLatentCommand
	{
	public:
		FUntilEndOfPerfomanceTestLatentCommand(LevelRecorder_ACC* _levelRecorder) :
			levelRecorder(_levelRecorder),
			bIsTestFinished(false)
		{
			levelRecorder->OnFinishedRecording().AddRaw(this, &FUntilEndOfPerfomanceTestLatentCommand::OnTestFinished);
		}
		
		virtual bool Update() override
		{
			return bIsTestFinished;
		}

		void OnTestFinished(FPerfomanceTestLevelData _result)
		{
			levelRecorder->OnFinishedRecording().Clear();
			
			const UWorld* world = levelRecorder->GetWorld();
			check(world)
			
			Displacement::Test::WritePerfomanceTestData(UGameplayStatics::GetCurrentLevelName(world), _result);
			
			bIsTestFinished = true;
		}

	private:
		LevelRecorder_ACC* levelRecorder;
		bool bIsTestFinished;
	};
	
}

void DisplacementPerfomanceTest::GetTests(TArray<FString>& _outBeautifiedNames, TArray<FString>& _outTestCommands) const
{
	FPerfomanceTestRequestCollection perfomanceTestRequest{};
	Displacement::Test::ReadPerfomanceTestList(perfomanceTestRequest);

	TArray<int32> enabledMapsIndices{};

	for(int32 i = 0; i < perfomanceTestRequest.Data.Num(); ++i)
	{
		if(perfomanceTestRequest.Data[i].bIsEnabled)
		{
			enabledMapsIndices.Add(i);
		}
	}

	for (int32 i = 0; i < enabledMapsIndices.Num(); ++i)
	{
		const uint32 enabledTestIndex = enabledMapsIndices[i];
		FPerfomanceTestRequest& request = perfomanceTestRequest.Data[enabledTestIndex];

		_outBeautifiedNames.Add(request.Name);
		_outTestCommands.Add(request.Path);
	}
}

bool DisplacementPerfomanceTest::RunTest(const FString& _parameters)
{
	TArray<FString> parsedParams{};
	_parameters.ParseIntoArray(parsedParams, TEXT(","));

	if (!TestTrue("Map name should exist", parsedParams.Num() == 1)) return false;

	const Displacement::Test::LevelScope level{parsedParams[0]};

	UWorld* world = Displacement::Test::GetTestWorld();
	if (!TestNotNull("World not exists", world)) return false;

	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!TestNotNull("Character exists", playerController)) return false;

	const TArray<FPerfomancePointData>& pointsCollection = GetPerfomanceTestPoints(world);

	APawn* testPawn = Displacement::Test::CreateBlueprint<APawn>(world, Displacement::Test::DisplacementPerfomancePawn);
	check(testPawn)
	
	SetupActors(world, testPawn);
		
	UActorComponent* foundComponent = testPawn->AddComponentByClass(UDP_LevelPerfomanceRecorder_ACC::StaticClass(), false, FTransform::Identity, false);
	LevelRecorder_ACC* levelRecorderComponent = CastChecked<LevelRecorder_ACC>(foundComponent);
	
	levelRecorderComponent->BeginPerfomanceRecording(pointsCollection);
	ADD_LATENT_AUTOMATION_COMMAND(FUntilEndOfPerfomanceTestLatentCommand{levelRecorderComponent})
	
	return true;
}

#endif