// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GameFramework/Actor.h"
#include "QA/Perfomance/DP_PerfomanceRecorder_ACC.h"
#include "QA/Perfomance/PerfomanceTestUtils.h"
#include "DP_PerfomanceFunctionalTest_Actor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomanceFunctionalTest_Actor : public AFunctionalTest
{
	GENERATED_BODY()

public:
	ADP_PerfomanceFunctionalTest_Actor();
protected:
	UPROPERTY()
	TArray<TObjectPtr<ADP_PerfomancePoint_Actor>> LevelPerfomancePoints;
	UPROPERTY()
	TObjectPtr<UDP_PerfomanceRecorder_ACC> PerfomanceRecorder;
	
	virtual void PrepareTest() override;
	virtual void StartTest() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void FinishTest(EFunctionalTestResult TestResult, const FString& Message) override;
};
