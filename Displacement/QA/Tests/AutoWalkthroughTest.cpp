
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "TestUtils.h"
#include "InputAction.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputComponent.h"
#include "AutoWalkthrough/Utils/AutoWalkthroughUtils.h"
#include "Perfomance/PerfomanceTestUtils.h"
#include "AutoWalkthrough/Core/AW_InputRecorder_ACC.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(AutoWalkthroughTest, "Displacement.Displacement.QA.Tests.AutoWalkthroughTest",
                                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

namespace
{
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
		const FBindingsRecord* PeekCurrent() const;
		const FBindingsRecord* GetNext();

	private:
		const FInputRecord* GetCurrentRecord() const { return RecordIndex >= InputRecords.Num() ? nullptr : &InputRecords[RecordIndex]; }
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

	const FBindingsRecord* InputRecordIterator::GetNext()
	{
		const FInputRecord* currentRecord = GetCurrentRecord();
		if(!currentRecord) return nullptr;
		
		if(++BindingIndex >= currentRecord->Bindings.Num())
		{
			++RecordIndex;
			BindingIndex = 0;
			return PeekCurrent();
		}

		++BindingIndex;
		return PeekCurrent();
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
			WorldStartTime(0.0f),
			bIsInitialized(false)
		{}
		
	private:
		InputRecordIterator RecordIterator;
		TMap<FString, const UInputAction*> InputActionMap;
		UWorld* World;
		APawn* PlayerPawn;
		UEnhancedPlayerInput* PlayerInput;
		float WorldStartTime;
		bool bIsInitialized;
		
		void PrepareTest();
		virtual bool Update() override;
		const UInputAction* GetActionByName(const FString& _actionName) const;
	};

	void FAutoWalkthroughLatentCommand::PrepareTest()
	{
		World = Displacement::Test::GetTestWorld();
		const APlayerController* playerController = World->GetFirstPlayerController();
		PlayerInput = Cast<UEnhancedPlayerInput>(playerController->PlayerInput);
		PlayerPawn = playerController->GetPawn();
		WorldStartTime = World->TimeSeconds;

		const UEnhancedInputComponent* inputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent);
		const TArray<TUniquePtr<FEnhancedInputActionEventBinding>>& actionBindings = inputComponent->GetActionEventBindings();

		for (int32 i = 0; i < actionBindings.Num(); ++i)
		{
			const TUniquePtr<FEnhancedInputActionEventBinding>& actionBinding = actionBindings[i];
			const UInputAction* inputAction = actionBinding->GetAction();
			const FString& actionName = inputAction->GetName();\
			
			if(!InputActionMap.Contains(actionName))
			{
				InputActionMap.Add(actionName, inputAction);	
			}
		}
	}

	bool FAutoWalkthroughLatentCommand::Update()
	{
		if(!bIsInitialized)
		{
			PrepareTest();
			bIsInitialized = true;
		}

		const FBindingsRecord* bindingsRecord = RecordIterator.PeekCurrent();
		if(!bindingsRecord) return true;

		if(World->TimeSeconds - WorldStartTime < bindingsRecord->WorldTime) return false;
		
		for(int32 i = 0; i < bindingsRecord->ActionValues.Num(); ++i)
		{
			const UInputAction* inputAction = GetActionByName(bindingsRecord->ActionValues[i].ActionName); 
			
			const FActionRecord& actionRecord = bindingsRecord->ActionValues[i];
			const FInputActionValue actionValue{actionRecord.ActionValue};
			
			PlayerInput->InjectInputForAction(inputAction, actionValue);
		}

		for(int32 i = 0; i < bindingsRecord->AxisValues.Num(); ++i)
		{
			const UInputAction* inputAction = GetActionByName(bindingsRecord->AxisValues[i].AxisName); 
			
			const FAxisRecord& axisRecord = bindingsRecord->AxisValues[i];
			const FInputActionValue actionValue{axisRecord.AxisValue};
			
			PlayerInput->InjectInputForAction(inputAction, actionValue);
		}

		return RecordIterator.GetNext() == nullptr;
	}

	const UInputAction* FAutoWalkthroughLatentCommand::GetActionByName(const FString& _actionName) const
	{
		return *InputActionMap.Find(_actionName);
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
