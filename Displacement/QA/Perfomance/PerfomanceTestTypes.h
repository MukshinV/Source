// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MathUtil.h"
#include "PerfomanceTestTypes.generated.h"

USTRUCT()
struct FPerfomanceTestRegionMetrics
{
	GENERATED_BODY()

	UPROPERTY()
	FString RegionName{};
	UPROPERTY()
	float TicksPerSecond{};
	UPROPERTY()
	float MaxFPSDelta{};

	void Reset()
	{
		RegionName.Reset();
		MaxFPSDelta = 0.0f;
		TicksPerSecond = 0.0f;
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

struct FPerfomanceTestTimer
{
	float TimePassed;
	float WaitDurationSeconds;

	void AddDeltaTime(float _deltaTime) { TimePassed += _deltaTime;}
	bool IsRunning() const { return TimePassed < WaitDurationSeconds; }
	void Reset() { TimePassed = 0.0f; WaitDurationSeconds = 0.0f; }
	void SetWaitDuration(float _waitDuration) { WaitDurationSeconds = _waitDuration; }
	float GetRatio() const
	{
		check(WaitDurationSeconds != 0.0f)
		return TimePassed / WaitDurationSeconds;
	}
};

class FTickCounter
{
public:
	FTickCounter() = default;
	uint32 GetTicksAmount() const { return TickCounter; }
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
	void StartCollect() { TickCounter.Reset(); MaxFPSDelta = 0.0f; }
	void Tick(float _deltaTime);
	uint32 GetTickAmount() const { return TickCounter.GetTicksAmount(); }
private:
	FTickCounter TickCounter;
	float MaxFPSDelta;
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



