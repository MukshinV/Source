// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"

namespace Displacement
{
	namespace Test
	{
		UWorld* GetTestWorld();

		class LevelScope
		{
		public:
			LevelScope(const FString& MapName) { AutomationOpenMap(MapName); }
			~LevelScope() { ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand); }
		};

	}
}
