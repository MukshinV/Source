// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DP_PerfomanceTestConfig_Actor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomanceTestConfig_Actor : public AActor
{
	GENERATED_BODY()

public:
	ADP_PerfomanceTestConfig_Actor();
	UDataTable* GetDataTable() const { return ReferenceDataTable; }
	TSubclassOf<APawn> GetTestPawnSubclass() const { return TestPawnSubclass; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	TSubclassOf<APawn> TestPawnSubclass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	TObjectPtr<UDataTable> ReferenceDataTable;
};
