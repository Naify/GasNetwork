// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GasNetwork : ModuleRules
{
	public GasNetwork(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		
		PublicIncludePaths.Add("GasNetwork/");
		
		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "EnhancedInput" });
	}
}
