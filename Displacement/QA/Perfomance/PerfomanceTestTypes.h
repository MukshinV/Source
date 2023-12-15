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

	void Reset()
	{
		RegionName.Reset();
		AverageFPS = 0.0f;
		MaxFPSDelta = 0.0f;
	}
};

USTRUCT()
struct FPerfomanceTestLevelData
{
	GENERATED_BODY()

	UPROPERTY()
	FString LevelName{};
	UPROPERTY()
	FString PerfomanceTestDate{};
	UPROPERTY()
	TArray<FPerfomanceTestRegionData> RegionDatas{};
};

USTRUCT()
struct FPerfomanceTestRequest
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name{};
	UPROPERTY()
	FString Path{};
	UPROPERTY()
	bool bIsEnabled{};
};

USTRUCT()
struct FPerfomanceTestRequestCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPerfomanceTestRequest> Data{};
};




