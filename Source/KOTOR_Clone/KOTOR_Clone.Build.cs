// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KOTOR_Clone : ModuleRules
{
	public KOTOR_Clone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
		});

		// AIDM specific includes
		PublicIncludePaths.AddRange(new string[] {
			"KOTOR_Clone/Public",
			"KOTOR_Clone/Public/AIDM",
			"KOTOR_Clone/Public/UI",
			"KOTOR_Clone/Public/Debug",
			"KOTOR_Clone/Public/Player"
		});

		PrivateIncludePaths.AddRange(new string[] {
			"KOTOR_Clone/Private",
			"KOTOR_Clone/Private/AIDM",
			"KOTOR_Clone/Private/UI",
			"KOTOR_Clone/Private/Debug",
			"KOTOR_Clone/Private/Player"
		});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
