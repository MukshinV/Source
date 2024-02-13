#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

namespace Displacement
{
	namespace GameTesting
	{
		class FWaitLatentCommand : public IAutomationLatentCommand
		{
		public:
			FWaitLatentCommand(float _duration): Duration(_duration) {}
		private:
			float Duration;
			
			virtual bool Update() override;
		};
		
		UWorld* GetTestWorld();
		bool AutomationOpenMap(const FString& MapName, bool bForceReload = false);
		
		FORCEINLINE UDataTable* LoadTableFromPath(const FString& _path)
		{
			return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *_path));
		}

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

