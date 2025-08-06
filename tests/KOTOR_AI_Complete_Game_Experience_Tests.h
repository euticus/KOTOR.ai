// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

// KOTOR.ai Complete System Includes
#include "AI/AIDirectorSubsystem.h"
#include "Core/GameStartupSubsystem.h"
#include "Core/AutoMapLoader.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Companions/CompanionReactionSystem.h"

/**
 * KOTOR.ai Complete Game Experience Test Suite
 * Tests the full player journey from game launch to epic conclusion
 * Verifies that Python AI and UE5 create an amazing, cohesive game experience
 */

/* ============================================================================ */
/* 🎮 COMPLETE PLAYER JOURNEY TEST - LAUNCH TO EPIC CONCLUSION                */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCompletePlayerJourneyTest, "KOTOR.AI.CompleteExperience.FullPlayerJourney", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCompletePlayerJourneyTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Game World Created", TestWorld);
    
    // PHASE 1: GAME LAUNCH AND INITIALIZATION
    UGameStartupSubsystem* StartupSystem = TestWorld->GetSubsystem<UGameStartupSubsystem>();
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    
    TestNotNull("Startup System Available", StartupSystem);
    TestNotNull("AI Director Available", AIDirector);
    TestNotNull("Music System Available", MusicSubsystem);
    
    // Game launches with CLI script (simulated)
    StartupSystem->InitializeGameStartup();
    TestTrue("Game Launch: All Systems Initialized", StartupSystem->IsStartupComplete());
    
    // Python AIDM generates initial campaign
    bool bCampaignGenerated = AIDirector->GenerateCampaign("PlayerJourney_Epic");
    TestTrue("Game Launch: Python Campaign Generated", bCampaignGenerated);
    
    // PHASE 2: PLAYER STARTS ON ENDAR SPIRE
    UAutoMapLoader* MapLoader = UAutoMapLoader::GetAutoMapLoader(TestWorld);
    bool bEndarSpireLoaded = MapLoader->LoadSpecificMap("Endar_Spire", false);
    TestTrue("Journey Start: Endar Spire Loaded", bEndarSpireLoaded);
    
    // Python sets initial atmosphere and music
    bool bInitialMusicSet = MusicSubsystem->SetMusicBiome(EMusicBiome::Endar_Spire, 3.0f);
    TestTrue("Journey Start: Endar Spire Music Set", bInitialMusicSet);
    
    // Player character spawned with companions
    ACharacter* PlayerCharacter = TestWorld->SpawnActor<ACharacter>();
    AActor* Carth = TestWorld->SpawnActor<AActor>();
    UCompanionReactionSystem* CompanionSystem = Carth->CreateDefaultSubobject<UCompanionReactionSystem>(TEXT("CompanionSystem"));
    
    TestNotNull("Journey Start: Player Character", PlayerCharacter);
    TestNotNull("Journey Start: Carth Companion", Carth);
    TestNotNull("Journey Start: Companion System", CompanionSystem);
    
    // PHASE 3: TARIS EXPLORATION AND CHARACTER DEVELOPMENT
    bool bTarisLoaded = MapLoader->LoadSpecificMap("Taris", false);
    TestTrue("Character Development: Taris Loaded", bTarisLoaded);
    
    // Python generates Taris content based on player choices
    FPlayerProfile PlayerProfile;
    PlayerProfile.MoralityAlignment = 0.2f; // Slightly light side
    PlayerProfile.PreferredPlayStyle = "Diplomatic";
    PlayerProfile.CompletedQuests = {"Endar_Spire_Escape"};
    
    bool bTarisContentGenerated = AIDirector->GenerateLocationContent("Taris", PlayerProfile);
    TestTrue("Character Development: Python Taris Content", bTarisContentGenerated);
    
    // Music adapts to Taris
    bool bTarisMusicSet = MusicSubsystem->SetMusicBiome(EMusicBiome::Taris, 3.0f);
    TestTrue("Character Development: Taris Music", bTarisMusicSet);
    
    // Player makes moral choices, companions react
    bool bMoralChoiceProcessed = AIDirector->ProcessPlayerChoice("Help_Zelka_Forn", 0.3f);
    bool bCompanionReacted = CompanionSystem->TriggerCompanionReaction(
        ECompanionReactionTrigger::PlayerChoice, "Help_Zelka_Forn", 0.3f, "Carth");
    
    TestTrue("Character Development: Moral Choice Processed", bMoralChoiceProcessed);
    TestTrue("Character Development: Companion Reaction", bCompanionReacted);
    
    // PHASE 4: JEDI TRAINING ON DANTOOINE
    bool bDantooineLoaded = MapLoader->LoadSpecificMap("Dantooine", false);
    TestTrue("Jedi Training: Dantooine Loaded", bDantooineLoaded);
    
    // Python generates Jedi training content
    bool bJediTrainingGenerated = AIDirector->GenerateJediTrainingContent(PlayerProfile);
    TestTrue("Jedi Training: Python Training Content", bJediTrainingGenerated);
    
    // Music becomes more mystical and Force-focused
    bool bDantooineMusic = MusicSubsystem->SetMusicBiome(EMusicBiome::Dantooine, 3.0f);
    bool bMysticalTone = MusicSubsystem->SetMusicTone(EMusicTone::Mysterious, 2.0f);
    
    TestTrue("Jedi Training: Dantooine Music", bDantooineMusic);
    TestTrue("Jedi Training: Mystical Tone", bMysticalTone);
    
    // Player learns Force powers, Python adapts story
    PlayerProfile.ForceAlignment = 0.4f; // Light side Force user
    PlayerProfile.ForcePowers = {"Force_Push", "Heal", "Mind_Trick"};
    
    bool bForceTrainingProcessed = AIDirector->ProcessForceTraining(PlayerProfile);
    TestTrue("Jedi Training: Force Training Processed", bForceTrainingProcessed);
    
    // PHASE 5: STAR MAP QUEST ACROSS MULTIPLE WORLDS
    TArray<FString> StarMapWorlds = {"Korriban", "Kashyyyk", "Tatooine", "Manaan"};
    
    for (const FString& World : StarMapWorlds)
    {
        // Load each world
        bool bWorldLoaded = MapLoader->LoadSpecificMap(World, false);
        TestTrue(FString::Printf(TEXT("Star Map Quest: %s Loaded"), *World), bWorldLoaded);
        
        // Python generates world-specific content
        bool bWorldContentGenerated = AIDirector->GenerateLocationContent(World, PlayerProfile);
        TestTrue(FString::Printf(TEXT("Star Map Quest: %s Content Generated"), *World), bWorldContentGenerated);
        
        // Music adapts to each world's unique atmosphere
        EMusicBiome WorldBiome = GetBiomeFromWorldName(World);
        bool bWorldMusicSet = MusicSubsystem->SetMusicBiome(WorldBiome, 3.0f);
        TestTrue(FString::Printf(TEXT("Star Map Quest: %s Music Set"), *World), bWorldMusicSet);
        
        // Player makes choices that affect story progression
        FString WorldChoice = FString::Printf(TEXT("%s_StarMap_Choice"), *World);
        bool bWorldChoiceProcessed = AIDirector->ProcessPlayerChoice(WorldChoice, PlayerProfile.MoralityAlignment);
        TestTrue(FString::Printf(TEXT("Star Map Quest: %s Choice Processed"), *World), bWorldChoiceProcessed);
    }
    
    // PHASE 6: REVELATION AND LEVIATHAN
    bool bLeviathanLoaded = MapLoader->LoadSpecificMap("Leviathan", false);
    TestTrue("Revelation: Leviathan Loaded", bLeviathanLoaded);
    
    // Python orchestrates the big revelation
    FNarrativeRevelation Revelation;
    Revelation.RevelationType = "Player_Identity_Reveal";
    Revelation.EmotionalImpact = 1.0f; // Maximum impact
    Revelation.StoryConsequences = {"Bastila_Conflict", "Companion_Reactions", "Force_Bond_Revealed"};
    
    bool bRevelationOrchestrated = AIDirector->OrchestrateNarrativeRevelation(Revelation);
    TestTrue("Revelation: Python Orchestrated Big Reveal", bRevelationOrchestrated);
    
    // Music becomes intense and dramatic
    bool bLeviathanMusic = MusicSubsystem->SetMusicBiome(EMusicBiome::Leviathan, 2.0f);
    bool bDramaticTone = MusicSubsystem->SetMusicTone(EMusicTone::Dramatic, 1.5f);
    
    TestTrue("Revelation: Leviathan Music", bLeviathanMusic);
    TestTrue("Revelation: Dramatic Tone", bDramaticTone);
    
    // Companions react strongly to revelation
    bool bBastilaReaction = CompanionSystem->TriggerCompanionReaction(
        ECompanionReactionTrigger::StoryMoment, "Identity_Revelation", 1.0f, "Bastila");
    bool bCarthReaction = CompanionSystem->TriggerCompanionReaction(
        ECompanionReactionTrigger::StoryMoment, "Identity_Revelation", 1.0f, "Carth");
    
    TestTrue("Revelation: Bastila Reaction", bBastilaReaction);
    TestTrue("Revelation: Carth Reaction", bCarthReaction);
    
    // PHASE 7: FINAL CONFRONTATION ON STAR FORGE
    bool bStarForgeLoaded = MapLoader->LoadSpecificMap("Star_Forge", false);
    TestTrue("Final Confrontation: Star Forge Loaded", bStarForgeLoaded);
    
    // Python prepares epic finale
    FEpicFinalePreparation FinalePrep;
    FinalePrep.PlayerMorality = PlayerProfile.MoralityAlignment;
    FinalePrep.CompanionLoyalties = {{"Bastila", 0.8f}, {"Carth", 0.9f}, {"Jolee", 0.7f}};
    FinalePrep.StoryChoices = PlayerProfile.CompletedQuests;
    FinalePrep.ForceAlignment = PlayerProfile.ForceAlignment;
    
    bool bFinalePrepped = AIDirector->PrepareEpicFinale(FinalePrep);
    TestTrue("Final Confrontation: Python Finale Prepared", bFinalePrepped);
    
    // Music reaches epic climax
    bool bStarForgeMusic = MusicSubsystem->SetMusicBiome(EMusicBiome::StarForge, 2.0f);
    bool bEpicTone = MusicSubsystem->SetMusicTone(EMusicTone::Epic, 1.0f);
    bool bEpicPresetApplied = MusicSubsystem->ApplyBlendPreset("epic_finale");
    
    TestTrue("Final Confrontation: Star Forge Music", bStarForgeMusic);
    TestTrue("Final Confrontation: Epic Tone", bEpicTone);
    TestTrue("Final Confrontation: Epic Preset Applied", bEpicPresetApplied);
    
    // PHASE 8: BOSS BATTLE WITH DARTH MALAK
    AActor* DarthMalak = TestWorld->SpawnActor<AActor>();
    UBossIntroSequencer* BossSequencer = NewObject<UBossIntroSequencer>();
    
    TestNotNull("Boss Battle: Darth Malak Spawned", DarthMalak);
    TestNotNull("Boss Battle: Boss Sequencer", BossSequencer);
    
    // Python orchestrates epic boss encounter
    FBossIntroSequenceData MalakSequence;
    MalakSequence.SequenceType = EBossIntroSequenceType::Epic;
    MalakSequence.BossDialogue = "Now you will experience the full power of the dark side!";
    MalakSequence.BossEmotion = EPerformanceEmotion::Anger;
    MalakSequence.BossTone = EPerformanceTone::Threatening;
    MalakSequence.MusicMood = EMusicMood::Epic;
    
    bool bBossIntroStarted = BossSequencer->StartBossIntro(DarthMalak, EBossIntroSequenceType::Epic, MalakSequence);
    TestTrue("Boss Battle: Malak Intro Started", bBossIntroStarted);
    
    // Music intensifies for boss battle
    bool bBossMusicTriggered = MusicSubsystem->ApplyBlendPreset("malak_boss_battle");
    TestTrue("Boss Battle: Boss Music Triggered", bBossMusicTriggered);
    
    // PHASE 9: RESOLUTION AND ENDING
    // Python determines ending based on player choices throughout journey
    FGameEnding PlayerEnding = AIDirector->DetermineGameEnding(PlayerProfile, FinalePrep);
    TestTrue("Resolution: Python Determined Ending", !PlayerEnding.EndingType.IsEmpty());
    
    // Music transitions to appropriate ending theme
    FString EndingMusicPreset = FString::Printf(TEXT("ending_%s"), *PlayerEnding.EndingType.ToLower());
    bool bEndingMusicSet = MusicSubsystem->ApplyBlendPreset(EndingMusicPreset);
    TestTrue("Resolution: Ending Music Set", bEndingMusicSet);
    
    // PHASE 10: VERIFY COMPLETE JOURNEY COHERENCE
    // Check that all systems worked together to create coherent experience
    
    // Story progression should be logical and complete
    float StoryCompletionPercentage = AIDirector->GetStoryCompletionPercentage();
    TestTrue("Journey Coherence: Story Complete", StoryCompletionPercentage >= 0.95f);
    
    // Player choices should have meaningful consequences
    int32 MeaningfulChoices = AIDirector->GetMeaningfulChoiceCount();
    TestTrue("Journey Coherence: Meaningful Choices", MeaningfulChoices >= 10);
    
    // Companion relationships should reflect player actions
    float AverageCompanionLoyalty = CompanionSystem->GetAverageCompanionLoyalty();
    TestTrue("Journey Coherence: Companion Relationships", AverageCompanionLoyalty > 0.0f);
    
    // Music should have evolved throughout journey
    int32 MusicTransitions = MusicSubsystem->GetMusicTransitionCount();
    TestTrue("Journey Coherence: Music Evolution", MusicTransitions >= 8);
    
    // FINAL VERIFICATION: COMPLETE PYTHON + UE5 INTEGRATION SUCCESS
    TestTrue("COMPLETE JOURNEY: Game Launch Success", StartupSystem->IsStartupComplete());
    TestTrue("COMPLETE JOURNEY: Python AI Integration", bCampaignGenerated && bFinalePrepped);
    TestTrue("COMPLETE JOURNEY: UE5 Systems Integration", bStarForgeLoaded && bBossIntroStarted);
    TestTrue("COMPLETE JOURNEY: Music System Integration", bEpicPresetApplied && bEndingMusicSet);
    TestTrue("COMPLETE JOURNEY: Companion System Integration", bBastilaReaction && bCarthReaction);
    TestTrue("COMPLETE JOURNEY: Story Coherence", StoryCompletionPercentage >= 0.95f);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎯 PYTHON AI LEARNING AND ADAPTATION TEST                                  */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonAILearningAdaptationTest, "KOTOR.AI.CompleteExperience.PythonAILearningAdaptation", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonAILearningAdaptationTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    TestNotNull("AI Director Available", AIDirector);
    
    // STEP 1: Python AI starts with baseline knowledge
    FPlayerBehaviorBaseline InitialBaseline;
    InitialBaseline.AverageSessionLength = 120.0f; // 2 hours
    InitialBaseline.PreferredQuestTypes = {"Combat", "Exploration"};
    InitialBaseline.MoralityTendency = 0.0f; // Neutral
    InitialBaseline.CompanionPreferences = {"Carth", "Bastila"};
    
    bool bBaselineSet = AIDirector->SetPlayerBehaviorBaseline(InitialBaseline);
    TestTrue("AI Learning: Baseline Set", bBaselineSet);
    
    // STEP 2: Python observes player behavior and learns
    FPlayerBehaviorObservation Observation1;
    Observation1.SessionLength = 180.0f; // Player plays longer
    Observation1.ChosenQuests = {"Rescue_Mission", "Diplomatic_Solution", "Help_Civilians"};
    Observation1.MoralChoices = {0.6f, 0.8f, 0.4f}; // Light side tendency
    Observation1.CompanionInteractions = {{"Bastila", 15}, {"Jolee", 10}, {"Carth", 5}};
    
    bool bObservation1Processed = AIDirector->ProcessPlayerBehaviorObservation(Observation1);
    TestTrue("AI Learning: First Observation Processed", bObservation1Processed);
    
    // STEP 3: Python adapts content based on learning
    FContentAdaptation Adaptation1 = AIDirector->GenerateContentAdaptation();
    TestTrue("AI Learning: Content Adapted", !Adaptation1.AdaptedElements.IsEmpty());
    TestTrue("AI Learning: Light Side Content", Adaptation1.AdaptedElements.Contains("More_Diplomatic_Options"));
    
    // STEP 4: Python continues learning from more sessions
    FPlayerBehaviorObservation Observation2;
    Observation2.SessionLength = 240.0f; // Even longer sessions
    Observation2.ChosenQuests = {"Jedi_Training", "Force_Meditation", "Peaceful_Resolution"};
    Observation2.MoralChoices = {0.9f, 0.7f, 0.8f}; // Strong light side
    Observation2.CompanionInteractions = {{"Bastila", 25}, {"Jolee", 20}, {"Mission", 15}};
    
    bool bObservation2Processed = AIDirector->ProcessPlayerBehaviorObservation(Observation2);
    TestTrue("AI Learning: Second Observation Processed", bObservation2Processed);
    
    // STEP 5: Python's learning should show clear adaptation
    FPlayerProfile LearnedProfile = AIDirector->GetLearnedPlayerProfile();
    TestTrue("AI Learning: Profile Updated", LearnedProfile.MoralityAlignment > 0.5f);
    TestTrue("AI Learning: Session Length Adapted", LearnedProfile.PreferredSessionLength > 180.0f);
    TestTrue("AI Learning: Quest Preferences Learned", LearnedProfile.PreferredQuestTypes.Contains("Diplomatic"));
    
    // STEP 6: Python predicts future behavior
    FPlayerBehaviorPrediction Prediction = AIDirector->PredictPlayerBehavior(LearnedProfile);
    TestTrue("AI Learning: Behavior Predicted", !Prediction.PredictedActions.IsEmpty());
    TestTrue("AI Learning: Light Side Predicted", Prediction.PredictedMorality > 0.5f);
    
    // STEP 7: Python generates personalized content
    FPersonalizedContent PersonalizedContent = AIDirector->GeneratePersonalizedContent(LearnedProfile);
    TestTrue("AI Learning: Personalized Content Generated", !PersonalizedContent.CustomQuests.IsEmpty());
    TestTrue("AI Learning: Jedi Content Emphasized", PersonalizedContent.ContentThemes.Contains("Jedi_Path"));
    
    // STEP 8: Verify learning effectiveness
    float LearningAccuracy = AIDirector->GetLearningAccuracy();
    TestTrue("AI Learning: High Accuracy", LearningAccuracy > 0.8f);
    
    int32 AdaptationCount = AIDirector->GetAdaptationCount();
    TestTrue("AI Learning: Multiple Adaptations", AdaptationCount >= 2);
    
    TestWorld->DestroyWorld(false);
    return true;
}

// Helper function for world name to biome conversion
EMusicBiome GetBiomeFromWorldName(const FString& WorldName)
{
    if (WorldName == "Korriban") return EMusicBiome::Korriban;
    if (WorldName == "Kashyyyk") return EMusicBiome::Kashyyyk;
    if (WorldName == "Tatooine") return EMusicBiome::Tatooine;
    if (WorldName == "Manaan") return EMusicBiome::Manaan;
    return EMusicBiome::Generic;
}
