

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TestUtils.h"
#include "InputAction.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputComponent.h"
#include "AutoWalkthrough/Utils/AutoWalkthroughUtils.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Perfomance/PerfomanceTestUtils.h"
#include "AutoWalkthrough/Core/AW_InputRecorder_ACC.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(AutoWalkthroughTest, "Displacement.Displacement.QA.Tests.AutoWalkthroughTest",
                                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter);

DEFINE_LOG_CATEGORY_STATIC(LogAutoWalkthrough, All, All);

namespace
{
	const float DISTANCE_TOLERANCE = 200.0f;
	const float TIME_LIMIT_TOLERANCE = 5.0f;
	const float TARGET_FRAME_LIMIT = 15.0f;
	
	struct InputRecordIterator
	{
		TArray<FInputRecord> InputRecords;
		int32 RecordIndex;
		int32 BindingIndex;

		InputRecordIterator(const TArray<FInputRecord>& _recordsArray) :
			InputRecords(_recordsArray),
			RecordIndex(0),
			BindingIndex(0)
		{}

		bool IsAtTheStartOfInputRecord() const { return BindingIndex == 0; }
		const FBindingsRecord* PeekCurrent() const;
		const FInputRecord* GetCurrentRecord() const { return RecordIndex >= InputRecords.Num() ? nullptr : &InputRecords[RecordIndex]; }
		bool IsLastBinding() const;
		FString GetCurrentCheckpointName() const;
		const FBindingsRecord* GetNext();

	private:
		const FBindingsRecord* GetCurrentBinding() const
		{
			const FInputRecord* currentRecord = GetCurrentRecord();
			if(!currentRecord) return nullptr;
			
			return BindingIndex >= currentRecord->Bindings.Num() ? nullptr : &currentRecord->Bindings[BindingIndex];
		}
	};

	const FBindingsRecord* InputRecordIterator::PeekCurrent() const
	{
		return GetCurrentBinding();
	}

	bool InputRecordIterator::IsLastBinding() const
	{
		const FInputRecord* currentRecord = GetCurrentRecord();
		if(!currentRecord) return false;

		return currentRecord->Bindings.Num() - 1 == BindingIndex;
	}

	const FBindingsRecord* InputRecordIterator::GetNext()
	{
		const FInputRecord* currentRecord = GetCurrentRecord();
		if(!currentRecord) return nullptr;
		
		if(++BindingIndex >= currentRecord->Bindings.Num())
		{
			++RecordIndex;
			BindingIndex = 0;
		}

		return PeekCurrent();
	}

	FString InputRecordIterator::GetCurrentCheckpointName() const
	{
		const FInputRecord* currentRecord = GetCurrentRecord();
		if(!currentRecord) return {};

		return currentRecord->CheckpointName;
	}

	struct AutoWalkthroughTimer
	{
		float TimeLimit{0.0f};
		float StartTime{0.0f};
		float TimePassed{0.0f};
		float LastTimeStamp{0.0f};

		bool IsTimeOut() const { return TimePassed >= TimeLimit; }
		float GetTimePassedSinceStamp() const { return TimePassed - LastTimeStamp; }
		void UpdateTimer(const UWorld* _world) { LastTimeStamp = TimePassed; TimePassed = _world->TimeSeconds - StartTime; }
	};

	struct CheckpointsCounter
	{
		TArray<AAW_Checkpoint_QActor*> Checkpoints;
		int32 IndexCounter{0};

		AAW_Checkpoint_QActor* GetCheckpointWithName(const FString& _checkpointName) const;
		void MoveNext();
	};

	AAW_Checkpoint_QActor* CheckpointsCounter::GetCheckpointWithName(const FString& _checkpointName) const
	{
		for (int32 i = 0; i < Checkpoints.Num(); ++i)
		{
			FString currentCheckpoint = Checkpoints[i]->GetCheckpointName();
			if(currentCheckpoint == _checkpointName)
			{
				return Checkpoints[i];
			}
		}
		return nullptr;
	}

	void CheckpointsCounter::MoveNext()
	{
		++IndexCounter;
	}


	class FAutoWalkthroughLatentCommand : public IAutomationLatentCommand
	{
	public:
		FAutoWalkthroughLatentCommand(const TArray<FInputRecord>& inputRecords) :
			RecordIterator(inputRecords),
			InputActionMap(),
			World(),
			PlayerPawn(),
			PlayerInput(),
			WalkthroughTimer(),
			CheckpointsCounter(),
			InitialFrameLimit(),
			bIsInitialized(false)
		{}
		
	private:
		InputRecordIterator RecordIterator;
		TMap<FString, const UInputAction*> InputActionMap;
		UWorld* World;
		APawn* PlayerPawn;
		UEnhancedPlayerInput* PlayerInput;
		AutoWalkthroughTimer WalkthroughTimer;
		CheckpointsCounter CheckpointsCounter;
		float InitialFrameLimit;
		bool bIsInitialized;
		
		void PrepareTest();
		void SetupPlayerPawn(const FInputRecord* _inputRecord) const;
		void FillUpInputMap(const UEnhancedInputComponent* _inputComponent);
		void CalculateTimeLimit();
		void CollectCheckpoints();
		void CheckCurrentCheckpointDistance();
		void FinishTest();
		
		virtual bool Update() override;
		const UInputAction* GetActionByName(const FString& _actionName) const;
		void SimulateActionInput(const FBindingsRecord* _bindingsRecord) const;
		void SimulateAxisInput(const FBindingsRecord* _bindingsRecord) const;
	};
	
	void FAutoWalkthroughLatentCommand::PrepareTest()
	{
		World = Displacement::Test::GetTestWorld();
		const APlayerController* playerController = World->GetFirstPlayerController();
		PlayerInput = Cast<UEnhancedPlayerInput>(playerController->PlayerInput);
		PlayerPawn = playerController->GetPawn();
		WalkthroughTimer.StartTime = World->TimeSeconds;

		if(GEngine)
		{
			UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
			InitialFrameLimit = userSettings->GetFrameRateLimit();
			userSettings->SetFrameRateLimit(TARGET_FRAME_LIMIT);
			userSettings->ApplyNonResolutionSettings();
		}

		const UEnhancedInputComponent* inputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent);
		FillUpInputMap(inputComponent);
		CalculateTimeLimit();
		CollectCheckpoints();
	}

	void FAutoWalkthroughLatentCommand::SetupPlayerPawn(const FInputRecord* _inputRecord) const
	{
		if(!_inputRecord) return;

		UCameraComponent* cameraComponent = PlayerPawn->FindComponentByClass<UCameraComponent>();
		APlayerController* playerController = Cast<APlayerController>(PlayerPawn->GetController());
		
		PlayerPawn->SetActorTransform(_inputRecord->InitialTransform);
		playerController->SetControlRotation(_inputRecord->InitialTransform.Rotator());
		cameraComponent->SetRelativeRotation(_inputRecord->CameraRotation);
	}

	void FAutoWalkthroughLatentCommand::FillUpInputMap(const UEnhancedInputComponent* _inputComponent)
	{
		const TArray<TUniquePtr<FEnhancedInputActionEventBinding>>& actionBindings = _inputComponent->GetActionEventBindings();

		for (int32 i = 0; i < actionBindings.Num(); ++i)
		{
			const TUniquePtr<FEnhancedInputActionEventBinding>& actionBinding = actionBindings[i];
			const UInputAction* inputAction = actionBinding->GetAction();
			const FString& actionName = inputAction->GetName();
			
			if(!InputActionMap.Contains(actionName))
			{
				InputActionMap.Add(actionName, inputAction);	
			}
		}
	}

	void FAutoWalkthroughLatentCommand::CalculateTimeLimit()
	{
		WalkthroughTimer.TimeLimit = 0.0f;

		for (int32 i = 0; i < RecordIterator.InputRecords.Num(); ++i)
		{
			const FInputRecord& inputRecord = RecordIterator.InputRecords[i];
			const float startTime = inputRecord.Bindings[0].WorldTime;
			const float endTime = inputRecord.Bindings[inputRecord.Bindings.Num() - 1].WorldTime;

			WalkthroughTimer.TimeLimit += endTime - startTime;
		}

		WalkthroughTimer.TimeLimit += TIME_LIMIT_TOLERANCE;
	}

	void FAutoWalkthroughLatentCommand::CollectCheckpoints()
	{
		Displacement::Test::GetAllActorsOfClass<AAW_Checkpoint_QActor>(World, CheckpointsCounter.Checkpoints);
	}

	bool FAutoWalkthroughLatentCommand::Update()
	{
		if(!bIsInitialized)
		{
			PrepareTest();
			bIsInitialized = true;
		}

		if(RecordIterator.IsAtTheStartOfInputRecord())
		{
			SetupPlayerPawn(RecordIterator.GetCurrentRecord());
		}
		
		WalkthroughTimer.UpdateTimer(World);

		if(WalkthroughTimer.IsTimeOut())
		{
			UE_LOG(LogAutoWalkthrough, Error, TEXT("Test timeout"));
			FinishTest();
			return true;
		}

		const FBindingsRecord* bindingsRecord = RecordIterator.PeekCurrent();

		while(WalkthroughTimer.TimePassed >= bindingsRecord->WorldTime)
		{
			SimulateActionInput(bindingsRecord);
			SimulateAxisInput(bindingsRecord);

			if(RecordIterator.IsLastBinding())
			{
				CheckCurrentCheckpointDistance();
				CheckpointsCounter.MoveNext();
			}
			
			bindingsRecord = RecordIterator.GetNext();
			if(!bindingsRecord)
			{
				FinishTest();
				return true;
			}
		}

		return false;
	}

	void FAutoWalkthroughLatentCommand::CheckCurrentCheckpointDistance()
	{
		const AAW_Checkpoint_QActor* currentCheckpointActor = CheckpointsCounter.GetCheckpointWithName(RecordIterator.GetCurrentCheckpointName());
		FVector checkpointLocation = currentCheckpointActor->GetActorLocation();
		const FVector pawnLocation = PlayerPawn->GetActorLocation();
		checkpointLocation.Z = pawnLocation.Z;
		const float distance = (checkpointLocation - pawnLocation).Length();

		if(distance > DISTANCE_TOLERANCE)
		{
			UE_LOG(LogAutoWalkthrough, Error, TEXT("Checkpoint %s was not passed. Distance is %f"), *currentCheckpointActor->GetCheckpointName(), distance);
		}
	}

	void FAutoWalkthroughLatentCommand::FinishTest()
	{
		if(GEngine)
		{
			UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
			userSettings->SetFrameRateLimit(InitialFrameLimit);
			userSettings->ApplyNonResolutionSettings();
		}
	}

	const UInputAction* FAutoWalkthroughLatentCommand::GetActionByName(const FString& _actionName) const
	{
		return *InputActionMap.Find(_actionName);
	}

	void FAutoWalkthroughLatentCommand::SimulateActionInput(const FBindingsRecord* _bindingsRecord) const
	{
		for(int32 i = 0; i < _bindingsRecord->ActionValues.Num(); ++i)
		{
			const UInputAction* inputAction = GetActionByName(_bindingsRecord->ActionValues[i].ActionName); 
			
			const FActionRecord& actionRecord = _bindingsRecord->ActionValues[i];
			const FInputActionValue actionValue{actionRecord.ActionValue};
			
			PlayerInput->InjectInputForAction(inputAction, actionValue);
		}
	}

	void FAutoWalkthroughLatentCommand::SimulateAxisInput(const FBindingsRecord* _bindingsRecord) const
	{
		for(int32 i = 0; i < _bindingsRecord->AxisValues.Num(); ++i)
		{
			const UInputAction* inputAction = GetActionByName(_bindingsRecord->AxisValues[i].AxisName); 
			
			const FAxisRecord& axisRecord = _bindingsRecord->AxisValues[i];
			const FInputActionValue axisValue{axisRecord.AxisValue};
			
			PlayerInput->InjectInputForAction(inputAction, axisValue);
		}
	}
}

void AutoWalkthroughTest::GetTests(TArray<FString>& _outBeautifiedNames, TArray<FString>& _outTestCommands) const
{
	FAutoWalkthroughList walkthroughList{};
	AutoWalkthrough::ReadWalkthroughList(walkthroughList);
	TArray<uint32> enabledTestsIndices{};
	
	for (int32 i = 0; i < walkthroughList.Data.Num(); ++i)
	{
		if(walkthroughList.Data[i].bIsEnabled)
		{
			enabledTestsIndices.Add(i);
		}
	}
	
	for (int32 i = 0; i < enabledTestsIndices.Num(); ++i)
	{
		FAutoWalkthroughParameters& testParameters = walkthroughList.Data[enabledTestsIndices[i]];
		_outBeautifiedNames.Add(testParameters.Name);
		_outTestCommands.Add(testParameters.CombineInternalDataToString());
	}
}

bool AutoWalkthroughTest::RunTest(const FString& _parameters)
{
	TArray<FString> parsedParams{};
	_parameters.ParseIntoArray(parsedParams, TEXT(";"));

	AutomationOpenMap(parsedParams[1]);

	TArray<FInputRecord> recordsArray{};
	AutoWalkthrough::ReadWalkthroughRecords(parsedParams[0], recordsArray);
	recordsArray.Sort([](const FInputRecord& _lRecord, const FInputRecord& _rRecord){ return _lRecord.CheckpointIndex < _rRecord.CheckpointIndex; });
	ADD_LATENT_AUTOMATION_COMMAND(FAutoWalkthroughLatentCommand{recordsArray});
	return true;
}




#endif
