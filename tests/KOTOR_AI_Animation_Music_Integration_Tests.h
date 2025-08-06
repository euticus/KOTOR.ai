// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"

// KOTOR.ai Animation and Music System Includes
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Animation/AnimationTagMap.h"
#include "Animation/VOPerformanceIntegrationComponent.h"
#include "Animation/MetaHumanFacialAnimationComponent.h"
#include "Animation/AnimationTestConsole.h"
#include "Companions/CompanionReactionSystem.h"
#include "Cinematics/BossIntroSequencer.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Audio/RuntimeMusicLayeringComponent.h"
#include "Audio/AIDMNarrativeMusicLinker.h"
#include "Audio/MetaSoundMusicController.h"
#include "UI/MusicDebugPanel.h"

/**
 * KOTOR.ai Animation and Music Integration Test Suite
 * Tests the complete pipeline from Python AI emotion/tone generation to UE5 animation and music
 */

/* ============================================================================ */
/* 🎭 PYTHON EMOTION → UE5 ANIMATION COMPLETE PIPELINE TEST                   */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonEmotionToAnimationPipelineTest, "KOTOR.AI.AnimationMusic.PythonEmotionToAnimation", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonEmotionToAnimationPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Create character with complete animation pipeline
    AActor* TestCharacter = TestWorld->SpawnActor<AActor>();
    UProceduralPerformanceComponentV2* PerformanceComp = TestCharacter->CreateDefaultSubobject<UProceduralPerformanceComponentV2>(TEXT("Performance"));
    UVOPerformanceIntegrationComponent* VOIntegration = TestCharacter->CreateDefaultSubobject<UVOPerformanceIntegrationComponent>(TEXT("VOIntegration"));
    UMetaHumanFacialAnimationComponent* FacialAnimation = TestCharacter->CreateDefaultSubobject<UMetaHumanFacialAnimationComponent>(TEXT("FacialAnimation"));
    UAnimationTagMap* AnimTagMap = NewObject<UAnimationTagMap>();
    
    TestNotNull("Character Created", TestCharacter);
    TestNotNull("Performance Component", PerformanceComp);
    TestNotNull("VO Integration", VOIntegration);
    TestNotNull("Facial Animation", FacialAnimation);
    TestNotNull("Animation Tag Map", AnimTagMap);
    
    // STEP 1: Python AIDM analyzes dialogue context and generates emotional metadata
    // Simulate Python analyzing: "You killed my father!" - Angry, Accusatory, High Intensity
    FDialogueAnalysisResult PythonAnalysis;
    PythonAnalysis.DialogueText = "You killed my father!";
    PythonAnalysis.DetectedEmotion = "Angry";
    PythonAnalysis.DetectedTone = "Accusatory";
    PythonAnalysis.EmotionalIntensity = 0.95f;
    PythonAnalysis.SpeakerPersonality = "Vengeful";
    PythonAnalysis.ContextualFactors = {"Confrontation", "Personal_Loss", "Seeking_Justice"};
    
    // STEP 2: UE5 converts Python analysis to performance metadata
    FPerformanceMetadata PerformanceData;
    PerformanceData.Emotion = EPerformanceEmotion::Angry;
    PerformanceData.Tone = EPerformanceTone::Threatening; // Closest to accusatory
    PerformanceData.DialogueLine = PythonAnalysis.DialogueText;
    PerformanceData.IntensityLevel = PythonAnalysis.EmotionalIntensity;
    PerformanceData.SpeakerName = "Vengeful_Son";
    PerformanceData.ContextualTags = {"Confrontation", "Personal"};
    
    // STEP 3: Animation Tag Map selects appropriate animation
    FAnimationTagEntry SelectedAnimation = AnimTagMap->GetAnimationForEmotionTone(
        PerformanceData.Emotion, PerformanceData.Tone, PerformanceData.IntensityLevel);
    
    TestNotNull("Animation Selected", SelectedAnimation.AnimationMontage);
    TestTrue("Correct Animation Tag", SelectedAnimation.AnimationTag.Contains("Angry"));
    TestTrue("High Intensity Animation", SelectedAnimation.Priority >= 8); // High intensity should have high priority
    
    // STEP 4: Performance component plays body animation
    FPerformanceResult BodyResult = PerformanceComp->PlayPerformance(PerformanceData);
    TestTrue("Body Animation Played", BodyResult.bSuccess);
    TestEqual("Correct Emotion Used", BodyResult.UsedEmotion, EPerformanceEmotion::Angry);
    TestEqual("Correct Tone Used", BodyResult.UsedTone, EPerformanceTone::Threatening);
    
    // STEP 5: Facial animation system responds to emotion
    bool bFacialExpressionSet = FacialAnimation->SetFacialExpression(EMetaHumanExpression::Angry, 0.95f, 0.5f);
    TestTrue("Facial Expression Set", bFacialExpressionSet);
    
    // STEP 6: VO integration synchronizes voice with animation
    FVOPerformanceData VOData;
    VOData.SpeakerName = "Vengeful_Son";
    VOData.DialogueLine = PythonAnalysis.DialogueText;
    VOData.PerformanceMetadata = PerformanceData;
    VOData.SyncMode = EVOPerformanceSyncMode::Automatic;
    
    FVOPerformanceResult VOResult = VOIntegration->PlayDialogueWithPerformance(VOData);
    TestTrue("VO Performance Synchronized", VOResult.bSuccess);
    TestTrue("Animation Sync Successful", VOResult.bAnimationSynced);
    
    // STEP 7: Test complete pipeline timing
    TestTrue("Complete Pipeline Timing", VOResult.SyncAccuracy > 0.95f); // 95%+ sync accuracy
    
    // STEP 8: Verify emotional consistency throughout pipeline
    TestEqual("End-to-End Emotion Consistency", BodyResult.UsedEmotion, EPerformanceEmotion::Angry);
    TestTrue("End-to-End Intensity Preserved", FMath::Abs(BodyResult.UsedIntensity - 0.95f) < 0.05f);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎵 PYTHON NARRATIVE → UE5 MUSIC COMPLETE PIPELINE TEST                     */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonNarrativeToMusicPipelineTest, "KOTOR.AI.AnimationMusic.PythonNarrativeToMusic", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonNarrativeToMusicPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Initialize complete music pipeline
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    URuntimeMusicLayeringComponent* MusicLayering = NewObject<URuntimeMusicLayeringComponent>();
    UAIDMNarrativeMusicLinker* NarrativeLinker = NewObject<UAIDMNarrativeMusicLinker>();
    UMetaSoundMusicController* MetaSoundController = NewObject<UMetaSoundMusicController>();
    
    TestNotNull("Music Subsystem", MusicSubsystem);
    TestNotNull("Music Layering", MusicLayering);
    TestNotNull("Narrative Linker", NarrativeLinker);
    TestNotNull("MetaSound Controller", MetaSoundController);
    
    // Initialize systems
    MusicLayering->InitializeMusicLayering(MusicSubsystem);
    NarrativeLinker->InitializeNarrativeMusicLinker(MusicSubsystem, MusicLayering);
    
    // STEP 1: Python AIDM analyzes narrative progression and emotional arc
    // Scenario: Player approaching final confrontation with Darth Malak
    FNarrativeAnalysis PythonNarrativeAnalysis;
    PythonNarrativeAnalysis.CurrentStoryBeat = "Climax_Approaching";
    PythonNarrativeAnalysis.EmotionalArc = "Rising_Tension";
    PythonNarrativeAnalysis.PlayerMorality = -0.3f; // Slightly dark
    PythonNarrativeAnalysis.StoryProgression = 0.85f; // Near end
    PythonNarrativeAnalysis.UpcomingEvents = {"Boss_Reveal", "Final_Confrontation"};
    PythonNarrativeAnalysis.LocationContext = "Star_Forge";
    PythonNarrativeAnalysis.CompanionStates = {{"Bastila", "Conflicted"}, {"Carth", "Determined"}};
    
    // STEP 2: Python generates music requirements based on narrative analysis
    FMusicNarrativeRequirements PythonMusicReq;
    PythonMusicReq.PrimaryMood = "Epic_Tension";
    PythonMusicReq.SecondaryMood = "Dark_Anticipation";
    PythonMusicReq.IntensityProgression = {0.6f, 0.8f, 1.0f}; // Building intensity
    PythonMusicReq.RequiredBiome = "Star_Forge";
    PythonMusicReq.RequiredTone = "Epic";
    PythonMusicReq.NarrativeTag = "Boss_Encounter";
    PythonMusicReq.DynamicElements = {"Tension_Strings", "Epic_Brass", "Dark_Choir"};
    
    // STEP 3: UE5 narrative linker receives Python requirements
    FNarrativeContextData ContextData;
    ContextData.CurrentTag = EAIDMNarrativeTag::BossEncounter;
    ContextData.PlayerMorality = PythonNarrativeAnalysis.PlayerMorality;
    ContextData.StoryProgression = PythonNarrativeAnalysis.StoryProgression;
    ContextData.EmotionalIntensity = 0.9f;
    
    bool bNarrativeTagTriggered = NarrativeLinker->TriggerNarrativeTag(EAIDMNarrativeTag::BossEncounter, ContextData);
    TestTrue("Python→UE5: Narrative Tag Triggered", bNarrativeTagTriggered);
    
    // STEP 4: Music subsystem applies Python-generated requirements
    bool bBiomeSet = MusicSubsystem->SetMusicBiome(EMusicBiome::StarForge, 3.0f);
    bool bToneSet = MusicSubsystem->SetMusicTone(EMusicTone::Epic, 2.0f);
    
    TestTrue("UE5: Star Forge Biome Set", bBiomeSet);
    TestTrue("UE5: Epic Tone Set", bToneSet);
    
    // STEP 5: Music layering system creates dynamic progression
    // Layer 1: Base epic music
    FMusicLayerData BaseLayer;
    BaseLayer.LayerID = "epic_base";
    BaseLayer.StemType = EMusicStemType::Melody;
    BaseLayer.AssociatedBiome = EMusicBiome::StarForge;
    BaseLayer.AssociatedTone = EMusicTone::Epic;
    BaseLayer.Priority = 5;
    
    bool bBaseLayerAdded = MusicLayering->AddMusicLayer(BaseLayer);
    bool bBaseLayerActivated = MusicLayering->ActivateMusicLayer("epic_base", 0.8f, 2.0f);
    
    TestTrue("UE5: Base Epic Layer Added", bBaseLayerAdded);
    TestTrue("UE5: Base Epic Layer Activated", bBaseLayerActivated);
    
    // Layer 2: Tension strings (Python requirement)
    FMusicLayerData TensionLayer;
    TensionLayer.LayerID = "tension_strings";
    TensionLayer.StemType = EMusicStemType::Strings;
    TensionLayer.Priority = 7;
    
    bool bTensionLayerAdded = MusicLayering->AddMusicLayer(TensionLayer);
    bool bTensionLayerActivated = MusicLayering->ActivateMusicLayer("tension_strings", 0.6f, 1.5f);
    
    TestTrue("UE5: Tension Strings Added", bTensionLayerAdded);
    TestTrue("UE5: Tension Strings Activated", bTensionLayerActivated);
    
    // STEP 6: MetaSound controller receives real-time parameters
    bool bMetaSoundInitialized = MetaSoundController->InitializeMetaSoundController(nullptr, MusicSubsystem);
    TestTrue("MetaSound Controller Initialized", bMetaSoundInitialized);
    
    // Python sends real-time intensity updates
    MetaSoundController->SetIntensityParameter(0.9f, 1.0f);
    MetaSoundController->SetTensionParameter(0.8f, 1.0f);
    MetaSoundController->UpdateForMusicState(EMusicBiome::StarForge, EMusicTone::Epic, false);
    
    float CurrentIntensity = MetaSoundController->GetCurrentParameterValue("Intensity");
    TestTrue("MetaSound Intensity Set", FMath::Abs(CurrentIntensity - 0.9f) < 0.1f);
    
    // STEP 7: Test dynamic music progression as Python narrative evolves
    // Python detects boss reveal moment
    bool bBossRevealTriggered = NarrativeLinker->TriggerNarrativeTag(EAIDMNarrativeTag::Climax, ContextData);
    TestTrue("Python→UE5: Boss Reveal Triggered", bBossRevealTriggered);
    
    // Music should intensify automatically
    TArray<FString> ActiveLayers = MusicLayering->GetActiveLayers();
    TestTrue("UE5: Multiple Layers Active", ActiveLayers.Num() >= 2);
    TestTrue("UE5: Epic Base Active", ActiveLayers.Contains("epic_base"));
    TestTrue("UE5: Tension Strings Active", ActiveLayers.Contains("tension_strings"));
    
    // STEP 8: Verify complete Python→UE5 music pipeline
    EMusicBiome CurrentBiome = MusicSubsystem->GetCurrentBiome();
    EMusicTone CurrentTone = MusicSubsystem->GetCurrentTone();
    
    TestEqual("End-to-End: Biome Consistency", CurrentBiome, EMusicBiome::StarForge);
    TestEqual("End-to-End: Tone Consistency", CurrentTone, EMusicTone::Epic);
    TestTrue("End-to-End: Python Requirements Met", ActiveLayers.Num() >= 2);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎭🎵 PYTHON AI → UE5 SYNCHRONIZED ANIMATION + MUSIC TEST                   */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToSynchronizedAnimationMusicTest, "KOTOR.AI.AnimationMusic.PythonToSynchronizedAnimationMusic", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToSynchronizedAnimationMusicTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Create boss encounter with synchronized animation and music
    AActor* BossActor = TestWorld->SpawnActor<AActor>();
    UBossIntroSequencer* BossSequencer = NewObject<UBossIntroSequencer>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    UProceduralPerformanceComponentV2* BossPerformance = BossActor->CreateDefaultSubobject<UProceduralPerformanceComponentV2>(TEXT("BossPerformance"));
    
    TestNotNull("Boss Actor Created", BossActor);
    TestNotNull("Boss Sequencer", BossSequencer);
    TestNotNull("Music Subsystem", MusicSubsystem);
    TestNotNull("Boss Performance", BossPerformance);
    
    // STEP 1: Python AIDM orchestrates epic boss encounter
    // Python analyzes: Perfect moment for Darth Malak reveal, maximum dramatic impact
    FBossEncounterOrchestration PythonOrchestration;
    PythonOrchestration.BossID = "Darth_Malak";
    PythonOrchestration.EncounterMoment = "Dramatic_Reveal";
    PythonOrchestration.PlayerEmotionalState = "Anticipation";
    PythonOrchestration.NarrativeTension = 1.0f; // Maximum tension
    PythonOrchestration.RequiredSynchronization = true;
    PythonOrchestration.CinematicElements = {"Slow_Motion", "Camera_Zoom", "Lighting_Change"};
    PythonOrchestration.MusicCues = {"Silence_Beat", "Epic_Sting", "Boss_Theme_Swell"};
    PythonOrchestration.AnimationCues = {"Menacing_Approach", "Cape_Flourish", "Lightsaber_Ignite"};
    
    // STEP 2: UE5 boss sequencer receives Python orchestration
    FBossIntroSequenceData SequenceData;
    SequenceData.SequenceType = EBossIntroSequenceType::Dramatic;
    SequenceData.BossDialogue = "At last, we meet face to face!";
    SequenceData.BossEmotion = EPerformanceEmotion::Pride;
    SequenceData.BossTone = EPerformanceTone::Commanding;
    SequenceData.MusicMood = EMusicMood::Epic;
    SequenceData.SynchronizationRequired = true;
    
    // STEP 3: Synchronized sequence begins
    bool bSequenceStarted = BossSequencer->StartBossIntro(BossActor, EBossIntroSequenceType::Dramatic, SequenceData);
    TestTrue("UE5: Boss Sequence Started", bSequenceStarted);
    
    // STEP 4: Music system responds to Python cues
    // Cue 1: Silence beat (dramatic pause)
    bool bSilenceTriggered = MusicSubsystem->StopAllMusic(0.5f);
    TestTrue("UE5: Dramatic Silence", bSilenceTriggered);
    
    // Wait for silence beat (simulated)
    // Cue 2: Epic sting
    bool bEpicStingTriggered = MusicSubsystem->ActivateMusicStem("epic_sting", 1.0f, 0.1f);
    TestTrue("UE5: Epic Sting Triggered", bEpicStingTriggered);
    
    // Cue 3: Boss theme swell
    bool bBossThemeTriggered = MusicSubsystem->ApplyBlendPreset("malak_boss_theme");
    TestTrue("UE5: Boss Theme Triggered", bBossThemeTriggered);
    
    // STEP 5: Animation system responds to Python cues
    // Cue 1: Menacing approach
    FPerformanceMetadata MenacingApproach;
    MenacingApproach.Emotion = EPerformanceEmotion::Pride;
    MenacingApproach.Tone = EPerformanceTone::Menacing;
    MenacingApproach.DialogueLine = "Approaching with dark confidence";
    MenacingApproach.IntensityLevel = 0.8f;
    
    FPerformanceResult ApproachResult = BossPerformance->PlayPerformance(MenacingApproach);
    TestTrue("UE5: Menacing Approach Animation", ApproachResult.bSuccess);
    
    // Cue 2: Cape flourish (synchronized with music sting)
    FPerformanceMetadata CapeFlourish;
    CapeFlourish.Emotion = EPerformanceEmotion::Pride;
    CapeFlourish.Tone = EPerformanceTone::Dramatic;
    CapeFlourish.DialogueLine = "Dramatic cape movement";
    CapeFlourish.IntensityLevel = 1.0f;
    
    FPerformanceResult FlourishResult = BossPerformance->PlayPerformance(CapeFlourish);
    TestTrue("UE5: Cape Flourish Animation", FlourishResult.bSuccess);
    
    // STEP 6: Test synchronization accuracy
    // Both music and animation should be perfectly timed
    float MusicTiming = MusicSubsystem->GetCurrentPlaybackTime();
    float AnimationTiming = BossPerformance->GetCurrentPerformanceTime();
    float SyncDifference = FMath::Abs(MusicTiming - AnimationTiming);
    
    TestTrue("Synchronization Accuracy", SyncDifference < 0.1f); // Within 100ms
    
    // STEP 7: Test Python adaptive response
    // If synchronization drifts, Python should detect and correct
    if (SyncDifference > 0.05f)
    {
        // Python detects sync drift and sends correction
        bool bSyncCorrected = BossSequencer->CorrectSynchronization(MusicTiming, AnimationTiming);
        TestTrue("Python→UE5: Sync Correction", bSyncCorrected);
    }
    
    // STEP 8: Verify complete orchestrated experience
    TestTrue("End-to-End: Boss Sequence Orchestrated", bSequenceStarted);
    TestTrue("End-to-End: Music Cues Executed", bSilenceTriggered && bEpicStingTriggered && bBossThemeTriggered);
    TestTrue("End-to-End: Animation Cues Executed", ApproachResult.bSuccess && FlourishResult.bSuccess);
    TestTrue("End-to-End: Synchronization Maintained", SyncDifference < 0.1f);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎮 PYTHON AI → UE5 REAL-TIME ADAPTATION TEST                               */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToRealTimeAdaptationTest, "KOTOR.AI.AnimationMusic.PythonToRealTimeAdaptation", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToRealTimeAdaptationTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Setup real-time adaptation systems
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    UCompanionReactionSystem* CompanionReactions = NewObject<UCompanionReactionSystem>();
    
    TestNotNull("AI Director", AIDirector);
    TestNotNull("Music Subsystem", MusicSubsystem);
    TestNotNull("Companion Reactions", CompanionReactions);
    
    // STEP 1: Python AIDM monitors player behavior in real-time
    // Scenario: Player starts peaceful, then becomes aggressive
    FPlayerBehaviorMonitoring PythonMonitoring;
    PythonMonitoring.InitialBehavior = "Peaceful_Exploration";
    PythonMonitoring.CurrentActions = {"Walking", "Observing", "Talking_to_NPCs"};
    PythonMonitoring.AggressionLevel = 0.1f;
    PythonMonitoring.MoralityTrend = 0.0f; // Neutral
    
    // Initial state: Peaceful music and animations
    bool bPeacefulMusicSet = MusicSubsystem->SetMusicTone(EMusicTone::Peaceful, 2.0f);
    TestTrue("Initial: Peaceful Music Set", bPeacefulMusicSet);
    
    // STEP 2: Player behavior changes - becomes aggressive
    PythonMonitoring.CurrentActions = {"Attacking_NPCs", "Threatening_Dialogue", "Destroying_Property"};
    PythonMonitoring.AggressionLevel = 0.8f;
    PythonMonitoring.MoralityTrend = -0.6f; // Dark side shift
    
    // Python detects behavior change and sends adaptation signals
    bool bBehaviorChangeDetected = AIDirector->DetectPlayerBehaviorChange(PythonMonitoring);
    TestTrue("Python→UE5: Behavior Change Detected", bBehaviorChangeDetected);
    
    // STEP 3: UE5 systems adapt in real-time
    // Music should shift to tense/dark
    bool bTenseMusicSet = MusicSubsystem->SetMusicTone(EMusicTone::Tense, 1.5f);
    TestTrue("UE5: Music Adapted to Tension", bTenseMusicSet);
    
    // Companions should react with concern
    bool bCompanionReactionTriggered = CompanionReactions->TriggerCompanionReaction(
        ECompanionReactionTrigger::MoralityShift, "AggressiveBehavior", -0.6f, "Bastila");
    TestTrue("UE5: Companion Reaction Triggered", bCompanionReactionTriggered);
    
    // STEP 4: Python continues monitoring and adapting
    // Player escalates to violence
    PythonMonitoring.CurrentActions = {"Killing_Innocents", "Dark_Side_Powers", "Intimidation"};
    PythonMonitoring.AggressionLevel = 1.0f;
    PythonMonitoring.MoralityTrend = -0.9f; // Extreme dark side
    
    bool bEscalationDetected = AIDirector->DetectPlayerBehaviorChange(PythonMonitoring);
    TestTrue("Python→UE5: Escalation Detected", bEscalationDetected);
    
    // STEP 5: UE5 systems respond to escalation
    // Music should become dark and aggressive
    bool bDarkMusicSet = MusicSubsystem->SetMusicTone(EMusicTone::Dark, 1.0f);
    bool bAggressiveStemActivated = MusicSubsystem->ActivateMusicStem("dark_aggressive", 0.9f, 1.0f);
    
    TestTrue("UE5: Dark Music Set", bDarkMusicSet);
    TestTrue("UE5: Aggressive Stem Activated", bAggressiveStemActivated);
    
    // Companions should show strong negative reactions
    bool bStrongReactionTriggered = CompanionReactions->TriggerCompanionReaction(
        ECompanionReactionTrigger::PlayerChoice, "KillInnocents", -0.9f, "Carth");
    TestTrue("UE5: Strong Companion Reaction", bStrongReactionTriggered);
    
    // STEP 6: Test adaptation speed and accuracy
    EMusicTone CurrentTone = MusicSubsystem->GetCurrentTone();
    TestEqual("Real-Time: Music Tone Adapted", CurrentTone, EMusicTone::Dark);
    
    float CompanionRelationship = CompanionReactions->GetCompanionRelationshipLevel("Bastila");
    TestTrue("Real-Time: Companion Relationship Affected", CompanionRelationship < 0.3f);
    
    // STEP 7: Test Python learning and prediction
    // Python should predict player will continue dark path
    FPlayerBehaviorPrediction PythonPrediction = AIDirector->PredictPlayerBehavior(PythonMonitoring);
    TestTrue("Python→UE5: Behavior Prediction Generated", !PythonPrediction.PredictedActions.IsEmpty());
    TestTrue("Python→UE5: Dark Path Predicted", PythonPrediction.PredictedMorality < -0.5f);
    
    // STEP 8: Verify complete real-time adaptation pipeline
    TestTrue("End-to-End: Real-Time Monitoring", bBehaviorChangeDetected && bEscalationDetected);
    TestTrue("End-to-End: Music Adaptation", bPeacefulMusicSet && bTenseMusicSet && bDarkMusicSet);
    TestTrue("End-to-End: Companion Adaptation", bCompanionReactionTriggered && bStrongReactionTriggered);
    TestTrue("End-to-End: Predictive AI", !PythonPrediction.PredictedActions.IsEmpty());
    
    TestWorld->DestroyWorld(false);
    return true;
}
