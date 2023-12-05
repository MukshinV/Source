// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GameFramework/Actor.h"
#include "QA/Perfomance/DP_PerfomanceRecorder_ACC.h"
#include "QA/Perfomance/PerfomanceTestUtils.h"
#include "DP_PerfomanceFunctionalTest_QActor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomanceFunctionalTest_QActor : public AFunctionalTest
{
	GENERATED_BODY()

public:
	ADP_PerfomanceFunctionalTest_QActor();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	TSubclassOf<APawn> TestPawnSubclass;
	UPROPERTY()
	TArray<TObjectPtr<ADP_PerfomancePoint_Actor>> LevelPerfomancePoints;
	UPROPERTY()
	TObjectPtr<UDP_PerfomanceRecorder_ACC> PerfomanceRecorder;
	
	virtual void PrepareTest() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void FinishTest(EFunctionalTestResult TestResult, const FString& Message) override;

private:
	void CollectPerfomanceTestPoints();
	void SetupActors();
};
