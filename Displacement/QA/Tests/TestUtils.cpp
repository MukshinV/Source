#if WITH_AUTOMATION_TESTS

#include "QA/Tests/TestUtils.h"


namespace Displacement
{
	namespace PerfomanceTest
	{
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
	}
}

#endif