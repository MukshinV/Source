
#include "QA/Tests/TestUtils.h"
#include "GameFramework/GameStateBase.h"
#if WITH_EDITOR
#include "Tests/AutomationCommon.h"
#endif

namespace Displacement
{
	namespace GameTesting
	{
		namespace
		{
			class FWaitForMapToLoadCommand : public IAutomationLatentCommand
			{
			public:
				FWaitForMapToLoadCommand() = default;
			private:
				virtual bool Update() override;
			};
			
			bool FWaitForMapToLoadCommand::Update()
			{
				UWorld* TestWorld = Displacement::GameTesting::GetTestWorld();

				if ( TestWorld && TestWorld->AreActorsInitialized() )
				{
					AGameStateBase* GameState = TestWorld->GetGameState();
					if (GameState && GameState->HasMatchStarted() )
					{
						return true;
					}
				}

				return false;
			}

		}


		bool FWaitLatentCommand::Update()
		{
			const double NewTime = FPlatformTime::Seconds();
			if (NewTime - StartTime >= Duration)
			{
				return true;
			}
			return false;
		}

		UWorld* GetTestWorld()
		{
			const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
			for (const FWorldContext& Context : WorldContexts)
			{
				if ((Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game) && Context.World())
				{
					return Context.World();
				}
			}

			return nullptr;
		}

		bool AutomationOpenMap(const FString& MapName, bool bForceReload)
		{
#if WITH_EDITOR
		return ::AutomationOpenMap(MapName, bForceReload);
#endif
			
			bool bCanProceed = true;
			FString OutString = TEXT("");
			{
				UWorld* TestWorld = GetTestWorld();

				// Convert both to short names and strip PIE prefix
				FString ShortMapName = FPackageName::GetShortName(MapName);
				FString ShortWorldMapName = FPackageName::GetShortName(TestWorld->GetMapName());

				if (TestWorld->GetOutermost()->GetPIEInstanceID() != INDEX_NONE)
				{
					FString PIEPrefix = FString::Printf(PLAYWORLD_PACKAGE_PREFIX TEXT("_%d_"), TestWorld->GetOutermost()->GetPIEInstanceID());
					ShortWorldMapName.ReplaceInline(*PIEPrefix, TEXT(""));
				}
				if (ShortMapName != ShortWorldMapName || bForceReload)
				{
					FString OpenCommand = FString::Printf(TEXT("Open %s"), *MapName);
					GEngine->Exec(TestWorld, *OpenCommand);
				}

				ADD_LATENT_AUTOMATION_COMMAND(FWaitForMapToLoadCommand());
			}

			return (OutString.IsEmpty());
		}
	}
}

