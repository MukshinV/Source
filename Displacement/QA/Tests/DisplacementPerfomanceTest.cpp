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

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceTest, All, All);

namespace
{

	class FPerfomanceTestLatentCommand : public IAutomationLatentCommand
	{
		using LevelRecorder_ACC = UDP_LevelPerfomanceRecorder_ACC;
		
	public:
		FPerfomanceTestLatentCommand(const FString& _tablePath) :
			LevelRecorder(),
			TablePath(_tablePath),
			bIsInitialized(false),
			bIsTestFinished(false)
		{}
		
	private:
		LevelRecorder_ACC* LevelRecorder;
		FString TablePath;
		bool bIsInitialized;
		bool bIsTestFinished;

		void PrepareTest();
		void SetupActors(const UWorld* _world, APawn* _testPawn);
		void GetPerfomanceTestPoints(FPerfomanceCollectResult& _collectResult, const UWorld* _world);

		virtual bool Update() override;
		
		void OnTestFinished(FPerfomanceTestLevelMetrics _result);
		bool IsEqualAmountOfRowNamesAndPoints(const FPerfomanceCollectResult& _collectResult, const TArray<FName>& rowNames) const;
		bool IsRowNamesAndPointsNamesAreEqual(const FPerfomanceCollectResult& _collectResult, const TArray<FName>& rowNames) const;
		bool IsAllPointsAreValidForTest(const FPerfomanceCollectResult& _collectResult) const;
	};

	void FPerfomanceTestLatentCommand::GetPerfomanceTestPoints(FPerfomanceCollectResult& _collectResult, const UWorld* _world)
	{
		for (TActorIterator<ADP_PerfomancePoint_Actor> It(_world, ADP_PerfomancePoint_Actor::StaticClass()); It; ++It)
		{
			ADP_PerfomancePoint_Actor* foundPoint = *It;
			FName pointName = foundPoint->GetRegionName();
			_collectResult.PointsCollection.Add(pointName, foundPoint);
		}
	}

	void FPerfomanceTestLatentCommand::SetupActors(const UWorld* _world, APawn* _testPawn)
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

		FPerfomanceCollectResult collectResult{};

		collectResult.PathTable = Displacement::Test::LoadTableFromPath(TablePath);

		if(!collectResult.PathTable)
		{
			UE_LOG(LogPerfomanceTest, Error, TEXT("Can't load test table"));
			bIsTestFinished = true;
			return;
		}
		
		GetPerfomanceTestPoints(collectResult, world);

		const TArray<FName> rowNames { collectResult.PathTable->GetRowNames() };
		
		if(!IsEqualAmountOfRowNamesAndPoints(collectResult, rowNames) ||
			!IsAllPointsAreValidForTest(collectResult) ||
			!IsRowNamesAndPointsNamesAreEqual(collectResult, rowNames))
		{
			bIsTestFinished = true;
			return;
		}

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
		Displacement::Test::WritePerfomanceTestData(UGameplayStatics::GetCurrentLevelName(world), _result);
			
		bIsTestFinished = true;
	}

	bool FPerfomanceTestLatentCommand::IsRowNamesAndPointsNamesAreEqual(const FPerfomanceCollectResult& _collectResult, const TArray<FName>& rowNames) const
	{
		bool bIsAllConsistent = true;
		for (const FName& rowName : rowNames)
		{
			if(!_collectResult.PointsCollection.Contains(rowName))
			{
				UE_LOG(LogPerfomanceTest, Error, TEXT("Perfomance point %s is not valid for tesing"), *rowName.ToString());
				bIsAllConsistent = false;
			}
		}

		return bIsAllConsistent;
	}

	bool FPerfomanceTestLatentCommand::IsEqualAmountOfRowNamesAndPoints(const FPerfomanceCollectResult& _collectResult, const TArray<FName>& rowNames) const
	{
		if(rowNames.Num() != _collectResult.PointsCollection.Num())
		{
			UE_LOG(LogPerfomanceTest, Error, TEXT("Different amount of table rows and found points"));
			return false;
		}

		return true;
	}

	bool FPerfomanceTestLatentCommand::IsAllPointsAreValidForTest(const FPerfomanceCollectResult& _collectResult) const
	{
		for (const TTuple<FName, ADP_PerfomancePoint_Actor*>& pair : _collectResult.PointsCollection)
		{
			if(!pair.Value->IsValidForTesting())
			{
				UE_LOG(LogPerfomanceTest, Error, TEXT("Perfomance point %s is not valid for tesing"), *pair.Value->GetRegionName().ToString());
				return false;
			}
		}

		return true;
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
		_outTestCommands.Add(FString::Printf(TEXT("%s,%s"), *request.MapPath, *request.TestTablePath));
	}
}

bool DisplacementPerfomanceTest::RunTest(const FString& _parameters)
{
	TArray<FString> parsedParams{};
	_parameters.ParseIntoArray(parsedParams, TEXT(","));

	if (!TestTrue("Map name and test table should exist", parsedParams.Num() == 2)) return false;

	AutomationOpenMap(parsedParams[0]);
	
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand{2.0f});
	ADD_LATENT_AUTOMATION_COMMAND(FPerfomanceTestLatentCommand{parsedParams[1]});
	
	return true;
}

#endif