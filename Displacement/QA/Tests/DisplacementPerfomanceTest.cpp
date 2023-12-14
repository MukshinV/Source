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
	using PerfPointsArray_T = TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>;
	using LevelRecorder_ACC = UDP_LevelPerfomanceRecorder_ACC;

	void CollectPerfomanceTestPoints(UWorld* _world, PerfPointsArray_T& _perfomancePoints)
	{
		for (TActorIterator<ADP_PerfomancePoint_Actor> It(_world, ADP_PerfomancePoint_Actor::StaticClass()); It; ++It)
		{
			_perfomancePoints.Add(*It);
		}
	
		if(_perfomancePoints.Num() == 0)
		{
			FString levelName = UGameplayStatics::GetCurrentLevelName(_world);
			UE_LOG(LogPerfomanceTest, Warning, TEXT("No perfomance points was found at level %s"), *levelName);
		}
	}

	LevelRecorder_ACC* SetupActors(UWorld* _world, PerfPointsArray_T& _perfomancePoints, APawn* _testPawn)
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
	
		UActorComponent* levelRecorderComponent = _testPawn->AddComponentByClass(UDP_LevelPerfomanceRecorder_ACC::StaticClass(), false, FTransform::Identity, false);
		return CastChecked<LevelRecorder_ACC>(levelRecorderComponent);
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
	struct FTestData
	{
		FString TestName;
		FString MapPath;
	};
	
	const TArray<FTestData> testData =  
	{
		{"PerfomanceMap", Displacement::Test::DisplacementPerfomanceLevel},  
	};

	for (const FTestData& oneTestData : testData)
	{
		_outBeautifiedNames.Add(oneTestData.TestName);
		_outTestCommands.Add(FString::Printf(TEXT("%s"), *oneTestData.MapPath));
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

	PerfPointsArray_T levelPerfomancePoints{};

	CollectPerfomanceTestPoints(world, levelPerfomancePoints);

	APawn* testPawn = Displacement::Test::CreateBlueprint<APawn>(world, Displacement::Test::DisplacementPerfomancePawn);
	check(testPawn)
	
	LevelRecorder_ACC* levelRecorderComponent = SetupActors(world, levelPerfomancePoints, testPawn);
	
	levelRecorderComponent->BeginPerfomanceRecording(levelPerfomancePoints);
	ADD_LATENT_AUTOMATION_COMMAND(FUntilEndOfPerfomanceTestLatentCommand{levelRecorderComponent})
	
	return true;
}

#endif