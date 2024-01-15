#pragma once

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"
#include "EngineUtils.h"
#include "Misc/AutomationTest.h"
#include "Kismet/GameplayStatics.h"

namespace Displacement
{
	namespace Test
	{
		UWorld* GetTestWorld();

		FORCEINLINE UDataTable* LoadTableFromPath(const FString& _path)
		{
			return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *_path));
		}

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

		template <typename T>
		void GetAllActorsOfClass(UWorld* _world, TArray<T*>& _outArray)
		{
			for (TActorIterator<T> It(_world, T::StaticClass()); It; ++It)
			{
				_outArray.Add(*It);
			}
		}
	}
}

#endif