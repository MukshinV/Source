// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DP_PerfomancePoint_Actor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_PerfomancePoint_Actor : public AActor
{
	GENERATED_BODY()

public:
	ADP_PerfomancePoint_Actor();
	
	FName GetRegionName() const { return FName{GetActorNameOrLabel()}; }
	float GetWaitAmount() const { return WaitAmountSeconds; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data")
	float WaitAmountSeconds;

};
