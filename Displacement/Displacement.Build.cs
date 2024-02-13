// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;

public class Displacement : ModuleRules
{
	public Displacement(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange
		(
		new string[]
			{
				"Json", 
				"EnhancedInput",
				"JsonUtilities",
				"PerfomanceTest",
				"AutoWalkthrough"
			}
		);

		if (Target.Configuration == UnrealTargetConfiguration.Test)
		{
			PublicDependencyModuleNames.Add("AutomationTest");
		}

		PrivateIncludePaths.AddRange(new string[] { ModuleDirectory });
	}
}
