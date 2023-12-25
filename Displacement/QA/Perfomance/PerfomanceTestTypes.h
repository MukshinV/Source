﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerfomanceTestTypes.generated.h"

class ADP_PerfomancePoint_Actor;

USTRUCT()
struct FPerfomanceTestRegionMetrics
{
	GENERATED_BODY()

	UPROPERTY()
	FString RegionName{};
	UPROPERTY()
	FString FrameMilliseconds{};
	UPROPERTY()
	FString MaxFPSDelta{};
	UPROPERTY()
	FString TicksPerSecond{};

	void SetFrameMilliseconds(float _frameTime);
	void SetFPSDelta(float _fpsDelta);
	void SetTicksPerSecond(float _tickPerSecond);
	
	void Reset()
	{
		RegionName.Reset();
		MaxFPSDelta.Reset();
		TicksPerSecond.Reset();
		FrameMilliseconds.Reset();
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
	FString MapPath{};
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

UENUM(BlueprintType)
enum class EPerfomancePointTransitionMode : uint8
{
	Instant = 0,
	Smooth
};

USTRUCT(BlueprintType)
struct FPerfomancePointTransitionData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	EPerfomancePointTransitionMode TransitionToPointMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	float TransitionDuration{ 2.0f };
};

USTRUCT()
struct FPerfomanceCollectResult
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FName, ADP_PerfomancePoint_Actor*> PointsCollection;
	UPROPERTY()
	TObjectPtr<const UDataTable> PathTable;
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
	float GetFrameMilliseconds(float _passedSeconds) const;
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
	bool IsFinished() const { return FMath::IsNearlyEqual(InterpolationValue, 1.0f, 0.001f) ; }
	void SetData(const T& _first, const T& _second) { First = _first; Second = _second; }
	T GetInterpolatedData() const { return FMath::Lerp(First, Second, InterpolationValue); }
	void SetInterpolationValue(float _interpolationValue) { InterpolationValue = FMath::Clamp(_interpolationValue, 0.0f, 1.0f );}
	void ResetInterpolatorValue() { InterpolationValue = 0.0f; }
private:
	T First;
	T Second;
	float InterpolationValue{ 1.0f };
};

using FPositionInterpolator = FLinearInterpolator<FVector>;
using FRotationInterpolator = FLinearInterpolator<FRotator>;



