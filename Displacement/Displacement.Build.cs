// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Displacement : ModuleRules
{
	public Displacement(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" });

		PrivateIncludePaths.AddRange(new string[] { ModuleDirectory });
		
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "FunctionalTesting" });
		}
	}
}
