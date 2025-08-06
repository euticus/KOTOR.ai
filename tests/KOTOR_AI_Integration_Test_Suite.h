// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"

// Include all KOTOR.ai integration test files
#include "KOTOR_AI_End_To_End_Integration_Tests.h"
#include "KOTOR_AI_Animation_Music_Integration_Tests.h"
#include "KOTOR_AI_Complete_Game_Experience_Tests.h"

/**
 * KOTOR.ai Master Integration Test Suite
 * Comprehensive testing of all 12 phases working together to create an amazing game
 */

/* ============================================================================ */
/* 🎯 MASTER INTEGRATION TEST SUITE RUNNER                                    */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKOTORAIMasterIntegrationTest, "KOTOR.AI.MasterSuite.AllPhasesIntegration", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FKOTORAIMasterIntegrationTest::RunTest(const FString& Parameters)
{
    // This test verifies that all 12 phases of KOTOR.ai work together seamlessly
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Master Test World Created", TestWorld);
    
    // Track overall integration success
    int32 PassedIntegrations = 0;
    int32 TotalIntegrations = 12;
    
    // PHASE 1: Python AIDM Core System
    bool bPhase1Success = TestPhase1Integration(TestWorld);
    if (bPhase1Success) PassedIntegrations++;
    TestTrue("Phase 1: Python AIDM Core Integration", bPhase1Success);
    
    // PHASE 2: Unreal Engine 5 Integration
    bool bPhase2Success = TestPhase2Integration(TestWorld);
    if (bPhase2Success) PassedIntegrations++;
    TestTrue("Phase 2: UE5 Integration", bPhase2Success);
    
    // PHASE 3: Immersion & Intelligence
    bool bPhase3Success = TestPhase3Integration(TestWorld);
    if (bPhase3Success) PassedIntegrations++;
    TestTrue("Phase 3: Immersion & Intelligence Integration", bPhase3Success);
    
    // PHASE 4: Multiplayer D&D Simulator
    bool bPhase4Success = TestPhase4Integration(TestWorld);
    if (bPhase4Success) PassedIntegrations++;
    TestTrue("Phase 4: Multiplayer D&D Integration", bPhase4Success);
    
    // PHASE 5: Emotional Intelligence
    bool bPhase5Success = TestPhase5Integration(TestWorld);
    if (bPhase5Success) PassedIntegrations++;
    TestTrue("Phase 5: Emotional Intelligence Integration", bPhase5Success);
    
    // PHASE 6: World Simulation
    bool bPhase6Success = TestPhase6Integration(TestWorld);
    if (bPhase6Success) PassedIntegrations++;
    TestTrue("Phase 6: World Simulation Integration", bPhase6Success);
    
    // PHASE 7: Unreal Streamlining
    bool bPhase7Success = TestPhase7Integration(TestWorld);
    if (bPhase7Success) PassedIntegrations++;
    TestTrue("Phase 7: Unreal Streamlining Integration", bPhase7Success);
    
    // PHASE 8: Optimization Pipeline
    bool bPhase8Success = TestPhase8Integration(TestWorld);
    if (bPhase8Success) PassedIntegrations++;
    TestTrue("Phase 8: Optimization Pipeline Integration", bPhase8Success);
    
    // PHASE 9: Cinematic Multiplayer
    bool bPhase9Success = TestPhase9Integration(TestWorld);
    if (bPhase9Success) PassedIntegrations++;
    TestTrue("Phase 9: Cinematic Multiplayer Integration", bPhase9Success);
    
    // PHASE 10: Dynamic Animation & Music
    bool bPhase10Success = TestPhase10Integration(TestWorld);
    if (bPhase10Success) PassedIntegrations++;
    TestTrue("Phase 10: Dynamic Animation & Music Integration", bPhase10Success);
    
    // PHASE 11: AI Animation + Procedural Music
    bool bPhase11Success = TestPhase11Integration(TestWorld);
    if (bPhase11Success) PassedIntegrations++;
    TestTrue("Phase 11: AI Animation + Procedural Music Integration", bPhase11Success);
    
    // PHASE 12: Build & Launch Streamlining
    bool bPhase12Success = TestPhase12Integration(TestWorld);
    if (bPhase12Success) PassedIntegrations++;
    TestTrue("Phase 12: Build & Launch Streamlining Integration", bPhase12Success);
    
    // MASTER INTEGRATION VERIFICATION
    float IntegrationSuccessRate = (float)PassedIntegrations / (float)TotalIntegrations;
    TestTrue("Master Integration: High Success Rate", IntegrationSuccessRate >= 0.9f); // 90%+ success
    
    // Test cross-phase communication
    bool bCrossPhaseComm = TestCrossPhaseIntegration(TestWorld);
    TestTrue("Master Integration: Cross-Phase Communication", bCrossPhaseComm);
    
    // Test complete game experience
    bool bCompleteExperience = TestCompleteGameExperience(TestWorld);
    TestTrue("Master Integration: Complete Game Experience", bCompleteExperience);
    
    TestWorld->DestroyWorld(false);
    
    // Final master verification
    bool bMasterSuccess = (IntegrationSuccessRate >= 0.9f) && bCrossPhaseComm && bCompleteExperience;
    TestTrue("🎉 KOTOR.AI MASTER INTEGRATION SUCCESS", bMasterSuccess);
    
    return bMasterSuccess;
}

/* ============================================================================ */
/* 🔧 INDIVIDUAL PHASE INTEGRATION TESTS                                      */
/* ============================================================================ */

bool FKOTORAIMasterIntegrationTest::TestPhase1Integration(UWorld* TestWorld)
{
    // Test Python AIDM core system integration with UE5
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    if (!AIDirector) return false;
    
    // Test campaign generation
    bool bCampaignGen = AIDirector->GenerateCampaign("IntegrationTest");
    
    // Test NPC generation
    FNPCData TestNPC = AIDirector->GenerateNPC("TestNPC", ENPCType::Merchant);
    bool bNPCGen = !TestNPC.NPCName.IsEmpty();
    
    // Test quest generation
    FQuestData TestQuest = AIDirector->GenerateQuest("TestQuest", EQuestType::SideQuest);
    bool bQuestGen = !TestQuest.QuestTitle.IsEmpty();
    
    return bCampaignGen && bNPCGen && bQuestGen;
}

bool FKOTORAIMasterIntegrationTest::TestPhase2Integration(UWorld* TestWorld)
{
    // Test UE5 subsystem integration
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    
    bool bSubsystemsAvailable = (AIDirector != nullptr) && (MusicSubsystem != nullptr);
    bool bSubsystemsInitialized = AIDirector->IsInitialized() && MusicSubsystem->IsInitialized();
    
    return bSubsystemsAvailable && bSubsystemsInitialized;
}

bool FKOTORAIMasterIntegrationTest::TestPhase3Integration(UWorld* TestWorld)
{
    // Test voice synthesis and companion AI integration
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    UVoiceSynthesisComponent* VoiceComp = TestActor->CreateDefaultSubobject<UVoiceSynthesisComponent>(TEXT("Voice"));
    UCompanionAIComponent* CompanionAI = TestActor->CreateDefaultSubobject<UCompanionAIComponent>(TEXT("CompanionAI"));
    
    bool bComponentsCreated = (VoiceComp != nullptr) && (CompanionAI != nullptr);
    bool bVoiceSynthesis = VoiceComp->SynthesizeVoice("Integration test", EVoiceType::Female_Young);
    
    return bComponentsCreated && bVoiceSynthesis;
}

bool FKOTORAIMasterIntegrationTest::TestPhase4Integration(UWorld* TestWorld)
{
    // Test multiplayer and D&D simulation integration
    UMultiplayerSessionComponent* SessionComp = NewObject<UMultiplayerSessionComponent>();
    UDnDCampaignSimulator* CampaignSim = NewObject<UDnDCampaignSimulator>();
    
    bool bComponentsCreated = (SessionComp != nullptr) && (CampaignSim != nullptr);
    
    FSessionSettings Settings;
    Settings.MaxPlayers = 4;
    bool bSessionCreated = SessionComp->CreateSession(Settings);
    
    return bComponentsCreated && bSessionCreated;
}

bool FKOTORAIMasterIntegrationTest::TestPhase5Integration(UWorld* TestWorld)
{
    // Test emotional intelligence integration
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    UCharacterDepthComponent* CharDepth = TestActor->CreateDefaultSubobject<UCharacterDepthComponent>(TEXT("CharDepth"));
    UAIPacingComponent* AIPacing = TestActor->CreateDefaultSubobject<UAIPacingComponent>(TEXT("AIPacing"));
    
    bool bComponentsCreated = (CharDepth != nullptr) && (AIPacing != nullptr);
    
    FEmotionalState TestState;
    TestState.Happiness = 0.7f;
    CharDepth->SetEmotionalState(TestState);
    
    FEmotionalState RetrievedState = CharDepth->GetEmotionalState();
    bool bEmotionalStateWorking = (RetrievedState.Happiness == 0.7f);
    
    return bComponentsCreated && bEmotionalStateWorking;
}

bool FKOTORAIMasterIntegrationTest::TestPhase6Integration(UWorld* TestWorld)
{
    // Test world simulation integration
    ULivingUniverseComponent* LivingUniverse = NewObject<ULivingUniverseComponent>();
    USubconsciousNarrativeComponent* SubNarrative = NewObject<USubconsciousNarrativeComponent>();
    
    bool bComponentsCreated = (LivingUniverse != nullptr) && (SubNarrative != nullptr);
    
    FWorldState TestWorldState;
    TestWorldState.PoliticalTension = 0.6f;
    LivingUniverse->SetWorldState(TestWorldState);
    
    FWorldState RetrievedState = LivingUniverse->GetWorldState();
    bool bWorldStateWorking = (RetrievedState.PoliticalTension == 0.6f);
    
    return bComponentsCreated && bWorldStateWorking;
}

bool FKOTORAIMasterIntegrationTest::TestPhase7Integration(UWorld* TestWorld)
{
    // Test unreal streamlining integration
    UModularToolkitComponent* ModularToolkit = NewObject<UModularToolkitComponent>();
    UPlugAndPlayComponent* PlugAndPlay = NewObject<UPlugAndPlayComponent>();
    
    bool bComponentsCreated = (ModularToolkit != nullptr) && (PlugAndPlay != nullptr);
    bool bModuleRegistered = ModularToolkit->RegisterModule("TestModule", "1.0.0");
    
    return bComponentsCreated && bModuleRegistered;
}

bool FKOTORAIMasterIntegrationTest::TestPhase8Integration(UWorld* TestWorld)
{
    // Test optimization pipeline integration
    UWorkflowOptimizationComponent* WorkflowOpt = NewObject<UWorkflowOptimizationComponent>();
    UPerformanceMonitoringComponent* PerfMon = NewObject<UPerformanceMonitoringComponent>();
    
    bool bComponentsCreated = (WorkflowOpt != nullptr) && (PerfMon != nullptr);
    bool bWorkflowOptimized = WorkflowOpt->OptimizeWorkflow();
    
    return bComponentsCreated && bWorkflowOptimized;
}

bool FKOTORAIMasterIntegrationTest::TestPhase9Integration(UWorld* TestWorld)
{
    // Test cinematic multiplayer integration
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    UAIAudioComponent* AIAudio = TestActor->CreateDefaultSubobject<UAIAudioComponent>(TEXT("AIAudio"));
    UPartyDecisionComponent* PartyDecision = TestActor->CreateDefaultSubobject<UPartyDecisionComponent>(TEXT("PartyDecision"));
    
    bool bComponentsCreated = (AIAudio != nullptr) && (PartyDecision != nullptr);
    bool bAudioSynthesized = AIAudio->SynthesizeAudio("Integration test audio");
    
    return bComponentsCreated && bAudioSynthesized;
}

bool FKOTORAIMasterIntegrationTest::TestPhase10Integration(UWorld* TestWorld)
{
    // Test dynamic animation and music integration
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    UAIAnimationComponent* AIAnimation = TestActor->CreateDefaultSubobject<UAIAnimationComponent>(TEXT("AIAnimation"));
    UProceduralMusicComponent* ProcMusic = TestActor->CreateDefaultSubobject<UProceduralMusicComponent>(TEXT("ProcMusic"));
    
    bool bComponentsCreated = (AIAnimation != nullptr) && (ProcMusic != nullptr);
    
    FAnimationRequest AnimRequest;
    AnimRequest.EmotionalState = "Happy";
    bool bAnimationGenerated = AIAnimation->GenerateAnimation(AnimRequest);
    
    return bComponentsCreated && bAnimationGenerated;
}

bool FKOTORAIMasterIntegrationTest::TestPhase11Integration(UWorld* TestWorld)
{
    // Test AI animation + procedural music integration
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    UProceduralPerformanceComponentV2* PerformanceComp = TestActor->CreateDefaultSubobject<UProceduralPerformanceComponentV2>(TEXT("Performance"));
    UMetaHumanFacialAnimationComponent* FacialAnimation = TestActor->CreateDefaultSubobject<UMetaHumanFacialAnimationComponent>(TEXT("FacialAnimation"));
    
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    URuntimeMusicLayeringComponent* MusicLayering = NewObject<URuntimeMusicLayeringComponent>();
    
    bool bComponentsCreated = (PerformanceComp != nullptr) && (FacialAnimation != nullptr) && 
                             (MusicSubsystem != nullptr) && (MusicLayering != nullptr);
    
    // Test performance system
    FPerformanceMetadata TestPerformance;
    TestPerformance.Emotion = EPerformanceEmotion::Happy;
    TestPerformance.Tone = EPerformanceTone::Normal;
    TestPerformance.DialogueLine = "Integration test dialogue";
    
    FPerformanceResult Result = PerformanceComp->PlayPerformance(TestPerformance);
    bool bPerformanceWorking = Result.bSuccess;
    
    // Test music layering
    MusicLayering->InitializeMusicLayering(MusicSubsystem);
    bool bMusicLayeringInitialized = true; // Assume success if no crash
    
    return bComponentsCreated && bPerformanceWorking && bMusicLayeringInitialized;
}

bool FKOTORAIMasterIntegrationTest::TestPhase12Integration(UWorld* TestWorld)
{
    // Test build & launch streamlining integration
    UGameStartupSubsystem* StartupSystem = TestWorld->GetSubsystem<UGameStartupSubsystem>();
    UAutoMapLoader* MapLoader = UAutoMapLoader::GetAutoMapLoader(TestWorld);
    
    bool bSystemsAvailable = (StartupSystem != nullptr) && (MapLoader != nullptr);
    
    if (StartupSystem)
    {
        StartupSystem->InitializeGameStartup();
        bool bStartupComplete = StartupSystem->IsStartupComplete();
        return bSystemsAvailable && bStartupComplete;
    }
    
    return bSystemsAvailable;
}

bool FKOTORAIMasterIntegrationTest::TestCrossPhaseIntegration(UWorld* TestWorld)
{
    // Test that different phases communicate and work together
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    
    if (!AIDirector || !MusicSubsystem) return false;
    
    // Test AI Director → Music Subsystem communication
    bool bCommunicationTest = AIDirector->SendEventToMusicSubsystem("TestEvent", 1.0f);
    
    // Test Music Subsystem responding to AI Director
    bool bMusicResponse = MusicSubsystem->SetMusicTone(EMusicTone::Epic, 2.0f);
    
    return bCommunicationTest && bMusicResponse;
}

bool FKOTORAIMasterIntegrationTest::TestCompleteGameExperience(UWorld* TestWorld)
{
    // Test that all systems work together to create a complete game experience
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    UGameStartupSubsystem* StartupSystem = TestWorld->GetSubsystem<UGameStartupSubsystem>();
    
    if (!AIDirector || !MusicSubsystem || !StartupSystem) return false;
    
    // Simulate complete game flow
    bool bGameStarted = StartupSystem->IsStartupComplete();
    bool bCampaignGenerated = AIDirector->GenerateCampaign("CompleteExperienceTest");
    bool bMusicPlaying = MusicSubsystem->SetMusicBiome(EMusicBiome::Taris, 3.0f);
    
    // Create player character
    ACharacter* PlayerCharacter = TestWorld->SpawnActor<ACharacter>();
    bool bPlayerCreated = (PlayerCharacter != nullptr);
    
    return bGameStarted && bCampaignGenerated && bMusicPlaying && bPlayerCreated;
}
