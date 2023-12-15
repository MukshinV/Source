// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DP_PrefabPerfomancePoint_Actor.h"
#include "GameFramework/Actor.h"
#include "DP_PerfomancePointCollection_Actor.generated.h"

UENUM(BlueprintType)
enum class EPerfomancePointTransitionMode : uint8
{
	Instant = 0,
	Smooth
};

USTRUCT(BlueprintType)
struct FPerfomancePointData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	TObjectPtr<ADP_PerfomancePoint_Actor> Point;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	EPerfomancePointTransitionMode TransitionToNextMode;
};

UCLASS()
class DISPLACEMENT_API ADP_PerfomancePointCollection_Actor : public AActor
{
	GENERATED_BODY()

public:
	ADP_PerfomancePointCollection_Actor();
	const TArray<FPerfomancePointData>& GetPointsCollection() const { return PointsCollection; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	TArray<FPerfomancePointData> PointsCollection;
};
