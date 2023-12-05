// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerfomanceTestTypes.generated.h"

USTRUCT()
struct FPerfomanceTestRegionData
{
	GENERATED_BODY()

	UPROPERTY()
	FString RegionName{};
	UPROPERTY()
	float AverageFPS{};
	UPROPERTY()
	float MaxFPSDelta{};
};

USTRUCT()
struct FPerfomanceTestLevelData
{
	GENERATED_BODY()

	UPROPERTY()
	FString LevelName{};
	UPROPERTY()
	float AverageFPS{};
	UPROPERTY()
	float MaxFPSDelta{};
	UPROPERTY()
	TArray<FPerfomanceTestRegionData> RegionDatas{};
};


