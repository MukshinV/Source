// Fill out your copyright notice in the Description page of Project Settings.


#include "QA/Tests/TestUtils.h"


namespace Displacement
{
	namespace Test
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