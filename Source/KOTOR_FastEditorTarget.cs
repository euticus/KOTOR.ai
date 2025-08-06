// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class KOTOR_FastEditorTarget : TargetRules
{
	public KOTOR_FastEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.AddRange(new string[] { "KOTOR_Clone" });

		// Fast compilation settings
		bUseUnityBuild = true;
		bUsePCHFiles = true;
		bUseSharedPCHs = true;
		bBuildEditor = true;
		
		// Disable heavy systems for faster builds
		bCompileChaos = false;
		bCompileSimplygon = false;
		bCompileICU = false;
		bUseChaos = false;
		bCompilePhysX = false;
		bCompileAPEX = false;
		bCompileNvCloth = false;
		bCompileRecast = false;
		bCompileSpeedTree = false;
		bCompileForSize = false;
		
		// Optimization settings
		bUseLoggingInShipping = true;
		bUseChecksInShipping = false;
		bCompileWithStatsWithoutEngine = false;
		bCompileWithPluginSupport = true;
		bIncludePluginsForTargetPlatforms = true;
		bCompileWithAccessibilitySupport = false;
		bWithPerfCounters = false;
		
		// Memory and performance optimizations
		bUseMallocProfiler = false;
		bUseSharedBuildEnvironment = true;
		bUseAdaptiveUnityBuild = true;
		bAdaptiveUnityDisablesOptimizations = true;
		bAdaptiveUnityDisablesPCH = false;
		bAdaptiveUnityCreatesDedicatedPCH = true;
		bAdaptiveUnityEnablesEditAndContinue = true;
		
		// Disable unnecessary features for development
		bBuildDeveloperTools = true;
		bBuildWithEditorOnlyData = true;
		bCompileAgainstEngine = true;
		bCompileAgainstCoreUObject = true;
		bCompileAgainstApplicationCore = true;
		bIncludeADO = false;
		bCompileWithStatsWithoutEngine = false;
		
		// Platform-specific optimizations
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			bUseIncrementalLinking = true;
			bUseFastMonoCalls = true;
			bUseFastSemanticsRenderContexts = true;
		}
		
		// Linker optimizations
		bUseFastPDBLinking = true;
		bCreateMapFile = false;
		bAllowLTCG = false; // Disable Link Time Code Generation for faster linking
		bPGOOptimize = false; // Disable Profile Guided Optimization
		bPGOProfile = false;
		
		// Debug settings for development
		bDebugBuildsActuallyUseDebugCRT = false;
		bUseDebugCRT = false;
		
		// Disable heavy rendering features
		GlobalDefinitions.Add("WITH_EDITOR=1");
		GlobalDefinitions.Add("UE_BUILD_DEVELOPMENT=1");
		GlobalDefinitions.Add("KOTOR_FAST_BUILD=1");
		GlobalDefinitions.Add("WITH_CHAOS=0");
		GlobalDefinitions.Add("WITH_PHYSX=0");
		GlobalDefinitions.Add("COMPILE_WITHOUT_UNREAL_SUPPORT=0");
		
		// AIDM-specific optimizations
		GlobalDefinitions.Add("AIDM_FAST_COMPILE=1");
		GlobalDefinitions.Add("KOTOR_DEV_BUILD=1");
		GlobalDefinitions.Add("WITH_LIVE_CODING=1");
		
		// Exclude unnecessary modules
		ExcludeModules.AddRange(new string[]
		{
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"Niagara",
			"NiagaraCore",
			"NiagaraShader",
			"ChaosVehicles",
			"ChaosSolverEngine",
			"GeometryCollectionEngine",
			"FieldSystemEngine",
			"MRMesh",
			"LiveCoding",
			"HotReload",
			"Simplygon",
			"SimplygonMeshReduction",
			"SimplygonSwarm",
			"SpeedTreeImporter",
			"DatasmithContent",
			"VariantManagerContent",
			"Takes",
			"LevelSequence",
			"MovieScene",
			"MovieSceneTracks",
			"Sequencer",
			"TimeManagement",
			"MediaAssets",
			"MediaUtils",
			"WmfMedia",
			"WindowsMoviePlayer",
			"AvfMedia",
			"ImgMedia",
			"MediaPlayerEditor",
			"WebMMoviePlayer",
			"AndroidMedia",
			"AppleImageUtils",
			"AppleMoviePlayer",
			"ArchVisCharacter",
			"CableComponent",
			"CustomMeshComponent",
			"ProceduralMeshComponent",
			"RuntimeMeshComponent",
			"Paper2D",
			"UMG",
			"Slate",
			"SlateCore",
			"ToolMenus",
			"EditorStyle",
			"EditorWidgets",
			"GraphEditor",
			"KismetCompiler",
			"ToolkitSteamworks",
			"SteamAudio",
			"ResonanceAudio",
			"Synthesis",
			"AudioMixer",
			"SignalProcessing",
			"AudioExtensions",
			"SoundFields",
			"AudioLinkCore",
			"MetasoundEngine",
			"MetasoundFrontend",
			"MetasoundGenerator",
			"MetasoundGraphCore",
			"MetasoundStandardNodes",
			"WaveTable",
			"AudioWidgets",
			"AudioSynesthesia",
			"ConstantQNRT",
			"LoudnessNRT",
			"OnsetNRT",
			"PerceptualLoudnessNRT",
			"SpectralAnalysisNRT"
		});
		
		// Include only essential modules for KOTOR development
		IncludeModules.AddRange(new string[]
		{
			"KOTOR_Clone",
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"EditorSubsystem",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"NavigationSystem",
			"Json",
			"JsonUtilities",
			"HTTP",
			"Networking",
			"Sockets",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"Slate",
			"SlateCore",
			"RenderCore",
			"RHI",
			"ApplicationCore"
		});
		
		// Fast build warning suppressions
		bEnableExceptions = false;
		bUseRTTI = false;
		bEnforceIWYU = false;
		bHasExports = true;
		bBuildInSolutionByDefault = true;
		
		// Development-specific settings
		if (Target.Configuration == UnrealTargetConfiguration.Development ||
		    Target.Configuration == UnrealTargetConfiguration.DebugGame)
		{
			bUseLoggingInShipping = true;
			bUseChecksInShipping = true;
			GlobalDefinitions.Add("KOTOR_DEVELOPMENT_BUILD=1");
		}
		
		// Editor-specific optimizations
		if (Type == TargetType.Editor)
		{
			bBuildDeveloperTools = true;
			bBuildWithEditorOnlyData = true;
			bCompileWithStatsWithoutEngine = false;
			GlobalDefinitions.Add("KOTOR_EDITOR_BUILD=1");
		}
	}
}
