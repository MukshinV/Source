// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DP_PerfomancePointInterface.generated.h"

UINTERFACE()
class UDP_PerfomancePointInterface : public UInterface
{
	GENERATED_BODY()
};

class DISPLACEMENT_API IDP_PerfomancePointInterface
{
	GENERATED_BODY()
public:
	virtual bool IsValidForTesting() const = 0;
	virtual bool CanMoveToNextPoint() const = 0;
	virtual bool CanMoveToNextStage() const = 0;
	
	virtual void OnRecorderEntered() = 0;
	virtual void OnStartedStageRecording() = 0;
	virtual void OnRecorderExit() = 0;
protected:
	virtual void OnFinishedStageRecording() = 0;
};
