#pragma once

#if WITH_AUTOMATION_TESTS

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
			LevelScope(const FString& _mapName) { AutomationOpenMap(_mapName); }
			~LevelScope() { ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand); }
		};

		template <typename T>
		T* CreateBlueprint(UWorld* _world, const FString& _name, const FTransform& _transform = FTransform::Identity)
		{
			const UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *_name);
			return (_world && Blueprint) ? _world->SpawnActor<T>(Blueprint->GeneratedClass, _transform) : nullptr;
		}
	}
}

#endif