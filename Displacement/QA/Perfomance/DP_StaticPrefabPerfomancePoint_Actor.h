// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DP_PrefabPerfomancePoint_Actor.h"
#include "GameFramework/Actor.h"
#include "DP_StaticPrefabPerfomancePoint_Actor.generated.h"

UCLASS()
class DISPLACEMENT_API ADP_StaticPrefabPerfomancePoint_Actor : public ADP_PrefabPerfomancePoint_Actor
{
	GENERATED_BODY()

public:
	virtual AActor* GetSpawnPoint() override { return PrefabSpawnPoint; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Point Data", DisplayName="@PrefabSpawnPoint")
	TObjectPtr<AActor> PrefabSpawnPoint;
};
