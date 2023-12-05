// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GameFramework/Actor.h"
#include "QA/Perfomance/DP_LevelPerfomanceRecorder_ACC.h"
#include "QA/Perfomance/PerfomanceTestUtils.h"
#include "DP_PerfomanceFunctionalTest_QActor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomanceFunctionalTest_QActor : public AFunctionalTest
{
	GENERATED_BODY()

	using PerfPointsArray_T = TArray<TObjectPtr<ADP_PerfomancePoint_Actor>>;
public:
	ADP_PerfomanceFunctionalTest_QActor();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	TSubclassOf<APawn> TestPawnSubclass;
	UPROPERTY()
	TObjectPtr<UDP_LevelPerfomanceRecorder_ACC> PerfomanceRecorder;
	
	virtual void PrepareTest() override;

	void OnTestFinished(FPerfomanceTestLevelData _result);
private:
	void CollectPerfomanceTestPoints(PerfPointsArray_T& _perfomancePoints) const;
	void SetupActors(PerfPointsArray_T& _perfomancePoints);
};
