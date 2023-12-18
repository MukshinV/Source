// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerfomanceTestTypes.generated.h"

USTRUCT()
struct FPerfomanceTestRegionMetrics
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
struct FPerfomanceTestLevelMetrics
{
	GENERATED_BODY()

	UPROPERTY()
	FString LevelName{};
	UPROPERTY()
	FString PerfomanceTestDate{};
	UPROPERTY()
	TArray<FPerfomanceTestRegionMetrics> RegionDatas{};
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

class FPSCounter
{
public:
	FPSCounter() = default;
	float GetAverageFPS(float _timePassed) const { return static_cast<float>(TickCounter) / _timePassed; }
	void Reset() { TickCounter = 0u; }
	void AddTick() { ++TickCounter; }
private:
	uint32 TickCounter{};
}; 

class FFPSMetricsCollector
{
public:
	FFPSMetricsCollector() = default;
	float GetMaxFPSDelta() const { return MaxFPSDelta; }
	void StartCollect() { FrameCounter.Reset(); MaxFPSDelta = 0.0f; TimeSinceStart = 0.0f; }
	void Tick(float _deltaTime);
	float GetAverageFPS() const { return FrameCounter.GetAverageFPS(TimeSinceStart); }
private:
	FPSCounter FrameCounter;
	float MaxFPSDelta;
	float TimeSinceStart;
};

template<typename T>
class FLinearInterpolator
{
public:
	FLinearInterpolator() = default;
	bool IsFinished() const { return InterpolationValue >= 1.0f; }
	void SetData(const T& _first, const T& _second) { First = _first; Second = _second; }
	T GetInterpolatedData() const { return FMath::Lerp(First, Second, InterpolationValue); }
	void SetInterpolationValue(float _interpolationValue) { InterpolationValue = FMath::Min(1.0f, _interpolationValue);}
	void Reset() { InterpolationValue = 0.0f; }
private:
	T First;
	T Second;
	float InterpolationValue{ 1.0f };
};

using FPositionInterpolator = FLinearInterpolator<FVector>;
using FRotationInterpolator = FLinearInterpolator<FRotator>;



