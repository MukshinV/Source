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
                                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::PerfFilter);

namespace
{
	using LevelRecorder_ACC = UDP_LevelPerfomanceRecorder_ACC;

	class FPerfomanceTestLatentCommand : public IAutomationLatentCommand
	{
	public:
		FPerfomanceTestLatentCommand() :
			LevelRecorder(),
			bIsInitialized(false),
			bIsTestFinished(false)
		{}
		
	private:
		LevelRecorder_ACC* LevelRecorder;
		bool bIsInitialized;
		bool bIsTestFinished;

		void PrepareTest();
		void SetupActors(UWorld* _world, APawn* _testPawn);
		FPerfomanceCollectResult GetPerfomanceTestPoints(UWorld* _world);

		virtual bool Update() override;
		
		void OnTestFinished(FPerfomanceTestLevelMetrics _result);
	};

	FPerfomanceCollectResult FPerfomanceTestLatentCommand::GetPerfomanceTestPoints(UWorld* _world)
	{
		FPerfomanceCollectResult collectResult{};
		
		for (TActorIterator<ADP_PerfomancePoint_Actor> It(_world, ADP_PerfomancePoint_Actor::StaticClass()); It; ++It)
		{
			ADP_PerfomancePoint_Actor* foundPoint = *It;
			FName pointName = foundPoint->GetRegionName();
			check(!collectResult.PointsCollection.Contains(pointName))
			collectResult.PointsCollection.Add(pointName, foundPoint);

			if(!collectResult.PathTable)
			{
				FDataTableRowHandle rowHandle = foundPoint->GetRowHandle();
				check(rowHandle.DataTable);

				collectResult.PathTable = rowHandle.DataTable;
			}
		}

		return collectResult;
	}

	void FPerfomanceTestLatentCommand::SetupActors(UWorld* _world, APawn* _testPawn)
	{
		APlayerController* playerController = _world->GetFirstPlayerController();
		check(playerController);

		playerController->DisableInput(playerController);
		playerController->Possess(_testPawn);

		UCharacterMovementComponent* movementComponent = _testPawn->FindComponentByClass<UCharacterMovementComponent>();
		if(movementComponent)
		{
			movementComponent->GravityScale = 0.0f;
		}
	}

	void FPerfomanceTestLatentCommand::PrepareTest()
	{
		UWorld* world = Displacement::Test::GetTestWorld();
		check(world);

		FString levelName = UGameplayStatics::GetCurrentLevelName(world);

		const APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
		check(playerController);

		FPerfomanceCollectResult collectResult = GetPerfomanceTestPoints(world);

		APawn* testPawn = Displacement::Test::CreateBlueprint<APawn>(world, Displacement::Test::DisplacementPerfomancePawn);
		check(testPawn);
	
		SetupActors(world, testPawn);
		
		UActorComponent* foundComponent = testPawn->AddComponentByClass(UDP_LevelPerfomanceRecorder_ACC::StaticClass(), false, FTransform::Identity, false);
		LevelRecorder = CastChecked<LevelRecorder_ACC>(foundComponent);

		LevelRecorder->BeginPerfomanceRecording(collectResult);
		LevelRecorder->OnFinishedRecording().AddRaw(this, &FPerfomanceTestLatentCommand::OnTestFinished);
	}
	
	bool FPerfomanceTestLatentCommand::Update()
	{
		if(!bIsInitialized)
		{
			PrepareTest();
			bIsInitialized = true;
		}
			
		return bIsTestFinished;
	}
	
	void FPerfomanceTestLatentCommand::OnTestFinished(FPerfomanceTestLevelMetrics _result)
	{
		LevelRecorder->OnFinishedRecording().Clear();
			
		const UWorld* world = LevelRecorder->GetWorld();
		check(world)
			
		Displacement::Test::WritePerfomanceTestData(UGameplayStatics::GetCurrentLevelName(world), _result);
			
		bIsTestFinished = true;
	}
	
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
		_outTestCommands.Add(*request.Path);
	}
}

bool DisplacementPerfomanceTest::RunTest(const FString& _parameters)
{
	TArray<FString> parsedParams{};
	_parameters.ParseIntoArray(parsedParams, TEXT(","));

	if (!TestTrue("Map name should exist", parsedParams.Num() == 1)) return false;

	AutomationOpenMap(parsedParams[0]);
	
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand{2.0f});
	ADD_LATENT_AUTOMATION_COMMAND(FPerfomanceTestLatentCommand{});
	
	return true;
}

#endif