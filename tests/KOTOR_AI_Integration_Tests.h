// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tests/AutomationCommon.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"

// KOTOR.ai System Includes
#include "AI/AIDirectorSubsystem.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Animation/AnimationTagMap.h"
#include "Animation/VOPerformanceIntegrationComponent.h"
#include "Animation/MetaHumanFacialAnimationComponent.h"
#include "Animation/AnimationTestConsole.h"
#include "Companions/CompanionReactionSystem.h"
#include "Cinematics/BossIntroSequencer.h"
#include "Audio/RuntimeMusicLayeringComponent.h"
#include "Audio/AIDMNarrativeMusicLinker.h"
#include "Audio/MetaSoundMusicController.h"
#include "Core/GameStartupSubsystem.h"
#include "Core/AutoMapLoader.h"
#include "Core/DefaultViewportPrepComponent.h"
#include "UI/EditorPrepUtilityWidget.h"
#include "UI/MusicDebugPanel.h"

/**
 * KOTOR.ai Integration Test Suite
 * Comprehensive tests for all 12 phases of the KOTOR.ai framework
 */

/* ============================================================================ */
/* 🐍 PHASE 1: PYTHON AIDM CORE SYSTEM TESTS                                  */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase1_AIDMCoreSystemTest, "KOTOR.AI.Phase1.AIDMCoreSystem", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase1_AIDMCoreSystemTest::RunTest(const FString& Parameters)
{
    // Test AI Director Subsystem initialization and core functionality
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    TestNotNull("AI Director Subsystem", AIDirector);
    
    // Test campaign generation
    bool bCampaignGenerated = AIDirector->GenerateCampaign("TestSeed123");
    TestTrue("Campaign Generation", bCampaignGenerated);
    
    // Test NPC generation
    FNPCData TestNPC = AIDirector->GenerateNPC("TestNPC", ENPCType::Companion);
    TestTrue("NPC Generation", !TestNPC.NPCName.IsEmpty());
    TestEqual("NPC Type", TestNPC.NPCType, ENPCType::Companion);
    
    // Test quest generation
    FQuestData TestQuest = AIDirector->GenerateQuest("TestQuest", EQuestType::MainStory);
    TestTrue("Quest Generation", !TestQuest.QuestTitle.IsEmpty());
    TestTrue("Quest Objectives", TestQuest.Objectives.Num() > 0);
    
    // Test loot generation
    FLootData TestLoot = AIDirector->GenerateLoot(ELootTier::Legendary, "Lightsaber");
    TestTrue("Loot Generation", !TestLoot.ItemName.IsEmpty());
    TestEqual("Loot Tier", TestLoot.LootTier, ELootTier::Legendary);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎮 PHASE 2: UNREAL ENGINE 5 INTEGRATION TESTS                              */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase2_UE5IntegrationTest, "KOTOR.AI.Phase2.UE5Integration", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase2_UE5IntegrationTest::RunTest(const FString& Parameters)
{
    // Test complete UE5 integration with all subsystems
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Test subsystem registration
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    
    TestNotNull("AI Director Subsystem Registration", AIDirector);
    TestNotNull("Music Subsystem Registration", MusicSubsystem);
    
    // Test subsystem initialization
    TestTrue("AI Director Initialization", AIDirector->IsInitialized());
    TestTrue("Music Subsystem Initialization", MusicSubsystem->IsInitialized());
    
    // Test cross-subsystem communication
    bool bCommunicationTest = AIDirector->SendEventToMusicSubsystem("TestEvent", 1.0f);
    TestTrue("Cross-Subsystem Communication", bCommunicationTest);
    
    // Test Blueprint integration
    UClass* TestBPClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blueprints/BP_TestActor"));
    TestNotNull("Blueprint Class Loading", TestBPClass);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎭 PHASE 3: IMMERSION & INTELLIGENCE TESTS                                 */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase3_ImmersionIntelligenceTest, "KOTOR.AI.Phase3.ImmersionIntelligence", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase3_ImmersionIntelligenceTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Test voice synthesis integration
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    UVoiceSynthesisComponent* VoiceComp = TestActor->CreateDefaultSubobject<UVoiceSynthesisComponent>(TEXT("VoiceComp"));
    TestNotNull("Voice Synthesis Component", VoiceComp);
    
    // Test voice synthesis
    bool bVoiceSynthesized = VoiceComp->SynthesizeVoice("Hello, this is a test.", EVoiceType::Male_Young);
    TestTrue("Voice Synthesis", bVoiceSynthesized);
    
    // Test companion AI
    UCompanionAIComponent* CompanionAI = TestActor->CreateDefaultSubobject<UCompanionAIComponent>(TEXT("CompanionAI"));
    TestNotNull("Companion AI Component", CompanionAI);
    
    // Test memory system
    UMemoryComponent* Memory = TestActor->CreateDefaultSubobject<UMemoryComponent>(TEXT("Memory"));
    TestNotNull("Memory Component", Memory);
    
    FMemoryEntry TestMemory;
    TestMemory.EventDescription = "Player helped villagers";
    TestMemory.EmotionalWeight = 0.8f;
    Memory->AddMemory(TestMemory);
    
    TestEqual("Memory Storage", Memory->GetMemoryCount(), 1);
    
    // Test modding framework
    UModdingFrameworkComponent* ModFramework = TestActor->CreateDefaultSubobject<UModdingFrameworkComponent>(TEXT("ModFramework"));
    TestNotNull("Modding Framework", ModFramework);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🌌 PHASE 4: MULTIPLAYER D&D SIMULATOR TESTS                               */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase4_MultiplayerDnDTest, "KOTOR.AI.Phase4.MultiplayerDnD", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase4_MultiplayerDnDTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Test multiplayer session management
    UMultiplayerSessionComponent* SessionComp = NewObject<UMultiplayerSessionComponent>();
    TestNotNull("Multiplayer Session Component", SessionComp);
    
    // Test D&D campaign simulator
    UDnDCampaignSimulator* CampaignSim = NewObject<UDnDCampaignSimulator>();
    TestNotNull("D&D Campaign Simulator", CampaignSim);
    
    // Test space exploration
    USpaceExplorationComponent* SpaceComp = NewObject<USpaceExplorationComponent>();
    TestNotNull("Space Exploration Component", SpaceComp);
    
    // Test procedural generation
    UProceduralGenerationComponent* ProcGen = NewObject<UProceduralGenerationComponent>();
    TestNotNull("Procedural Generation Component", ProcGen);
    
    // Test cloud integration
    UCloudIntegrationComponent* CloudComp = NewObject<UCloudIntegrationComponent>();
    TestNotNull("Cloud Integration Component", CloudComp);
    
    // Test session creation
    FSessionSettings Settings;
    Settings.MaxPlayers = 4;
    Settings.bIsLAN = false;
    bool bSessionCreated = SessionComp->CreateSession(Settings);
    TestTrue("Session Creation", bSessionCreated);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 💭 PHASE 5: EMOTIONAL INTELLIGENCE TESTS                                   */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase5_EmotionalIntelligenceTest, "KOTOR.AI.Phase5.EmotionalIntelligence", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase5_EmotionalIntelligenceTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    
    // Test character depth system
    UCharacterDepthComponent* CharDepth = TestActor->CreateDefaultSubobject<UCharacterDepthComponent>(TEXT("CharDepth"));
    TestNotNull("Character Depth Component", CharDepth);
    
    // Test AI pacing
    UAIPacingComponent* AIPacing = TestActor->CreateDefaultSubobject<UAIPacingComponent>(TEXT("AIPacing"));
    TestNotNull("AI Pacing Component", AIPacing);
    
    // Test infinite content generation
    UInfiniteContentComponent* InfiniteContent = TestActor->CreateDefaultSubobject<UInfiniteContentComponent>(TEXT("InfiniteContent"));
    TestNotNull("Infinite Content Component", InfiniteContent);
    
    // Test emotional state tracking
    FEmotionalState TestState;
    TestState.Happiness = 0.7f;
    TestState.Anger = 0.2f;
    TestState.Fear = 0.1f;
    CharDepth->SetEmotionalState(TestState);
    
    FEmotionalState RetrievedState = CharDepth->GetEmotionalState();
    TestEqual("Emotional State Storage", RetrievedState.Happiness, 0.7f, 0.01f);
    
    // Test AI pacing adjustment
    AIPacing->SetPacingMode(EAIPacingMode::Dynamic);
    TestEqual("AI Pacing Mode", AIPacing->GetPacingMode(), EAIPacingMode::Dynamic);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🌍 PHASE 6: WORLD SIMULATION TESTS                                         */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase6_WorldSimulationTest, "KOTOR.AI.Phase6.WorldSimulation", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase6_WorldSimulationTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Test living universe simulation
    ULivingUniverseComponent* LivingUniverse = NewObject<ULivingUniverseComponent>();
    TestNotNull("Living Universe Component", LivingUniverse);
    
    // Test subconscious narrative
    USubconsciousNarrativeComponent* SubNarrative = NewObject<USubconsciousNarrativeComponent>();
    TestNotNull("Subconscious Narrative Component", SubNarrative);
    
    // Test player-driven atmosphere
    UPlayerDrivenAtmosphereComponent* Atmosphere = NewObject<UPlayerDrivenAtmosphereComponent>();
    TestNotNull("Player-Driven Atmosphere Component", Atmosphere);
    
    // Test world state simulation
    FWorldState TestWorldState;
    TestWorldState.PoliticalTension = 0.6f;
    TestWorldState.EconomicStability = 0.4f;
    TestWorldState.WarStatus = EWarStatus::ColdWar;
    LivingUniverse->SetWorldState(TestWorldState);
    
    FWorldState RetrievedState = LivingUniverse->GetWorldState();
    TestEqual("World State Storage", RetrievedState.PoliticalTension, 0.6f, 0.01f);
    
    // Test narrative influence
    bool bNarrativeInfluenced = SubNarrative->InfluenceNarrative("PlayerBetrayedCompanion", 0.8f);
    TestTrue("Narrative Influence", bNarrativeInfluenced);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🔧 PHASE 7: UNREAL STREAMLINING TESTS                                      */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase7_UnrealStreamliningTest, "KOTOR.AI.Phase7.UnrealStreamlining", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase7_UnrealStreamliningTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Test modular toolkit
    UModularToolkitComponent* ModularToolkit = NewObject<UModularToolkitComponent>();
    TestNotNull("Modular Toolkit Component", ModularToolkit);
    
    // Test plug-and-play system
    UPlugAndPlayComponent* PlugAndPlay = NewObject<UPlugAndPlayComponent>();
    TestNotNull("Plug-and-Play Component", PlugAndPlay);
    
    // Test developer toolkit
    UDeveloperToolkitComponent* DevToolkit = NewObject<UDeveloperToolkitComponent>();
    TestNotNull("Developer Toolkit Component", DevToolkit);
    
    // Test module registration
    bool bModuleRegistered = ModularToolkit->RegisterModule("TestModule", "1.0.0");
    TestTrue("Module Registration", bModuleRegistered);
    
    // Test plug-and-play functionality
    bool bComponentPlugged = PlugAndPlay->PlugComponent("TestComponent");
    TestTrue("Component Plugging", bComponentPlugged);
    
    // Test toolkit functionality
    TArray<FString> AvailableTools = DevToolkit->GetAvailableTools();
    TestTrue("Available Tools", AvailableTools.Num() > 0);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* ⚡ PHASE 8: OPTIMIZATION PIPELINE TESTS                                    */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase8_OptimizationPipelineTest, "KOTOR.AI.Phase8.OptimizationPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase8_OptimizationPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Test lightning-fast workflow
    UWorkflowOptimizationComponent* WorkflowOpt = NewObject<UWorkflowOptimizationComponent>();
    TestNotNull("Workflow Optimization Component", WorkflowOpt);
    
    // Test performance monitoring
    UPerformanceMonitoringComponent* PerfMon = NewObject<UPerformanceMonitoringComponent>();
    TestNotNull("Performance Monitoring Component", PerfMon);
    
    // Test automated optimization
    UAutomatedOptimizationComponent* AutoOpt = NewObject<UAutomatedOptimizationComponent>();
    TestNotNull("Automated Optimization Component", AutoOpt);
    
    // Test workflow optimization
    float OptimizationStartTime = FPlatformTime::Seconds();
    bool bWorkflowOptimized = WorkflowOpt->OptimizeWorkflow();
    float OptimizationTime = FPlatformTime::Seconds() - OptimizationStartTime;
    
    TestTrue("Workflow Optimization", bWorkflowOptimized);
    TestTrue("Optimization Speed", OptimizationTime < 1.0f); // Should complete in under 1 second
    
    // Test performance metrics
    FPerformanceMetrics Metrics = PerfMon->GetCurrentMetrics();
    TestTrue("Performance Metrics", Metrics.FrameRate > 0.0f);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎬 PHASE 9: CINEMATIC MULTIPLAYER TESTS                                    */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase9_CinematicMultiplayerTest, "KOTOR.AI.Phase9.CinematicMultiplayer", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase9_CinematicMultiplayerTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    
    // Test AI audio system
    UAIAudioComponent* AIAudio = TestActor->CreateDefaultSubobject<UAIAudioComponent>(TEXT("AIAudio"));
    TestNotNull("AI Audio Component", AIAudio);
    
    // Test party decision system
    UPartyDecisionComponent* PartyDecision = TestActor->CreateDefaultSubobject<UPartyDecisionComponent>(TEXT("PartyDecision"));
    TestNotNull("Party Decision Component", PartyDecision);
    
    // Test procedural trailer generation
    UProceduralTrailerComponent* ProcTrailer = TestActor->CreateDefaultSubobject<UProceduralTrailerComponent>(TEXT("ProcTrailer"));
    TestNotNull("Procedural Trailer Component", ProcTrailer);
    
    // Test AI audio synthesis
    bool bAudioSynthesized = AIAudio->SynthesizeAudio("Test dialogue for cinematic sequence");
    TestTrue("AI Audio Synthesis", bAudioSynthesized);
    
    // Test party decision making
    FPartyDecisionData DecisionData;
    DecisionData.DecisionText = "Should we trust the mysterious stranger?";
    DecisionData.Options = {"Trust", "Distrust", "Investigate"};
    bool bDecisionStarted = PartyDecision->StartPartyDecision(DecisionData);
    TestTrue("Party Decision Started", bDecisionStarted);
    
    // Test trailer generation
    bool bTrailerGenerated = ProcTrailer->GenerateTrailer("Epic Boss Battle");
    TestTrue("Procedural Trailer Generation", bTrailerGenerated);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎭 PHASE 10: DYNAMIC ANIMATION & ADAPTIVE SOUNDTRACK TESTS                 */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhase10_DynamicAnimationMusicTest, "KOTOR.AI.Phase10.DynamicAnimationMusic", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPhase10_DynamicAnimationMusicTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    
    // Test AI animation system
    UAIAnimationComponent* AIAnimation = TestActor->CreateDefaultSubobject<UAIAnimationComponent>(TEXT("AIAnimation"));
    TestNotNull("AI Animation Component", AIAnimation);
    
    // Test procedural music
    UProceduralMusicComponent* ProcMusic = TestActor->CreateDefaultSubobject<UProceduralMusicComponent>(TEXT("ProcMusic"));
    TestNotNull("Procedural Music Component", ProcMusic);
    
    // Test adaptive soundtrack
    UAdaptiveSoundtrackComponent* AdaptiveSoundtrack = TestActor->CreateDefaultSubobject<UAdaptiveSoundtrackComponent>(TEXT("AdaptiveSoundtrack"));
    TestNotNull("Adaptive Soundtrack Component", AdaptiveSoundtrack);
    
    // Test AI animation generation
    FAnimationRequest AnimRequest;
    AnimRequest.EmotionalState = "Angry";
    AnimRequest.ActionType = "Combat";
    bool bAnimationGenerated = AIAnimation->GenerateAnimation(AnimRequest);
    TestTrue("AI Animation Generation", bAnimationGenerated);
    
    // Test procedural music generation
    FMusicGenerationParams MusicParams;
    MusicParams.Mood = "Epic";
    MusicParams.Intensity = 0.8f;
    bool bMusicGenerated = ProcMusic->GenerateMusic(MusicParams);
    TestTrue("Procedural Music Generation", bMusicGenerated);
    
    // Test adaptive soundtrack response
    bool bSoundtrackAdapted = AdaptiveSoundtrack->AdaptToGameState("BossEncounter");
    TestTrue("Adaptive Soundtrack", bSoundtrackAdapted);
    
    TestWorld->DestroyWorld(false);
    return true;
}
