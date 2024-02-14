// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class G3D : ModuleRules
{
	public G3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SimFX", "EditorScriptingUtilities" });

        PublicIncludePaths.AddRange(new string[] { "SimFX/Public" });

    }
}
