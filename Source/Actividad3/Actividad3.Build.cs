// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Actividad3 : ModuleRules
{
	public Actividad3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
