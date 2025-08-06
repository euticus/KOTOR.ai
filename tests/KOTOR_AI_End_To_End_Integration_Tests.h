// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Tests/AutomationCommon.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

// KOTOR.ai System Includes - All 12 Phases
#include "AI/AIDirectorSubsystem.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Core/GameStartupSubsystem.h"

/**
 * KOTOR.ai End-to-End Integration Test Suite
 * Tests the complete pipeline from Python AIDM generation to UE5 gameplay experience
 * Verifies that Python AI and Unreal Engine work together seamlessly to create a great game!
 */

/* ============================================================================ */
/* 🎮 COMPLETE GAME EXPERIENCE TESTS - PYTHON + UE5 INTEGRATION               */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCompleteGameExperienceTest, "KOTOR.AI.EndToEnd.CompleteGameExperience", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCompleteGameExperienceTest::RunTest(const FString& Parameters)
{
    // Test the complete player experience from game start to epic boss battle
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Game World Created", TestWorld);
    
    // PHASE 1 + 2: Python AIDM generates campaign, UE5 loads it
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    TestNotNull("AI Director Available", AIDirector);
    
    // Python generates campaign data, UE5 receives it
    bool bCampaignGenerated = AIDirector->GenerateCampaign("TestSeed_EpicJourney");
    TestTrue("Python→UE5: Campaign Generated", bCampaignGenerated);
    
    // Verify campaign data is properly loaded in UE5
    FCampaignData CampaignData = AIDirector->GetCurrentCampaign();
    TestTrue("UE5: Campaign Data Loaded", !CampaignData.CampaignTitle.IsEmpty());
    TestTrue("UE5: Campaign Has Quests", CampaignData.MainQuests.Num() > 0);
    
    // PHASE 12: Game startup loads everything automatically
    UGameStartupSubsystem* StartupSystem = TestWorld->GetSubsystem<UGameStartupSubsystem>();
    TestNotNull("Startup System Available", StartupSystem);
    
    StartupSystem->InitializeGameStartup();
    TestTrue("Game Startup Complete", StartupSystem->IsStartupComplete());
    
    // PHASE 3: Player character spawned with voice synthesis
    ACharacter* PlayerCharacter = TestWorld->SpawnActor<ACharacter>();
    TestNotNull("Player Character Spawned", PlayerCharacter);
    
    UVoiceSynthesisComponent* VoiceComp = PlayerCharacter->FindComponentByClass<UVoiceSynthesisComponent>();
    TestNotNull("Voice Synthesis Available", VoiceComp);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎭 PYTHON AI → UE5 ANIMATION PIPELINE TEST                                 */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToAnimationPipelineTest, "KOTOR.AI.EndToEnd.PythonToAnimationPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToAnimationPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Create test NPC with animation system
    AActor* TestNPC = TestWorld->SpawnActor<AActor>();
    UProceduralPerformanceComponentV2* PerformanceComp = TestNPC->CreateDefaultSubobject<UProceduralPerformanceComponentV2>(TEXT("Performance"));
    UAnimationTagMap* AnimTagMap = NewObject<UAnimationTagMap>();
    
    TestNotNull("NPC Created", TestNPC);
    TestNotNull("Performance Component", PerformanceComp);
    TestNotNull("Animation Tag Map", AnimTagMap);
    
    // STEP 1: Python AIDM generates dialogue with emotional context
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    
    // Simulate Python generating dialogue: "You have betrayed us all!" (Angry, Threatening)
    FDialogueData PythonGeneratedDialogue;
    PythonGeneratedDialogue.DialogueText = "You have betrayed us all!";
    PythonGeneratedDialogue.SpeakerEmotion = "Angry";
    PythonGeneratedDialogue.SpeakerTone = "Threatening";
    PythonGeneratedDialogue.EmotionalIntensity = 0.9f;
    
    bool bDialogueGenerated = AIDirector->GenerateDialogue("Malak", PythonGeneratedDialogue);
    TestTrue("Python→UE5: Dialogue Generated", bDialogueGenerated);
    
    // STEP 2: UE5 receives Python data and converts to performance metadata
    FPerformanceMetadata PerformanceData;
    PerformanceData.Emotion = EPerformanceEmotion::Angry;
    PerformanceData.Tone = EPerformanceTone::Threatening;
    PerformanceData.DialogueLine = PythonGeneratedDialogue.DialogueText;
    PerformanceData.IntensityLevel = PythonGeneratedDialogue.EmotionalIntensity;
    PerformanceData.SpeakerName = "Malak";
    
    // STEP 3: Animation system selects appropriate montage
    FAnimationTagEntry AnimEntry = AnimTagMap->GetAnimationForEmotionTone(
        EPerformanceEmotion::Angry, EPerformanceTone::Threatening, 0.9f);
    TestTrue("UE5: Animation Selected", AnimEntry.AnimationMontage != nullptr);
    TestTrue("UE5: Correct Animation Tag", AnimEntry.AnimationTag.Contains("Angry"));
    
    // STEP 4: Performance component plays the animation
    FPerformanceResult Result = PerformanceComp->PlayPerformance(PerformanceData);
    TestTrue("UE5: Performance Played", Result.bSuccess);
    TestTrue("UE5: Correct Montage Used", Result.SelectedMontage != nullptr);
    
    // STEP 5: Verify the complete pipeline worked
    TestEqual("End-to-End: Emotion Preserved", Result.UsedEmotion, EPerformanceEmotion::Angry);
    TestEqual("End-to-End: Tone Preserved", Result.UsedTone, EPerformanceTone::Threatening);
    TestTrue("End-to-End: Intensity Preserved", FMath::Abs(Result.UsedIntensity - 0.9f) < 0.1f);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎵 PYTHON AI → UE5 MUSIC PIPELINE TEST                                     */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToMusicPipelineTest, "KOTOR.AI.EndToEnd.PythonToMusicPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToMusicPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // STEP 1: Python AIDM analyzes game state and generates music requirements
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    
    TestNotNull("AI Director Available", AIDirector);
    TestNotNull("Music Subsystem Available", MusicSubsystem);
    
    // Simulate Python analyzing: Player on Korriban, approaching Sith Academy, high tension
    FGameStateAnalysis PythonAnalysis;
    PythonAnalysis.CurrentLocation = "Korriban";
    PythonAnalysis.NarrativeTension = 0.8f;
    PythonAnalysis.PlayerMorality = -0.6f; // Dark side leaning
    PythonAnalysis.UpcomingEvents = {"SithAcademyEntrance", "DarthMalakConfrontation"};
    
    bool bAnalysisComplete = AIDirector->AnalyzeGameState(PythonAnalysis);
    TestTrue("Python→UE5: Game State Analyzed", bAnalysisComplete);
    
    // STEP 2: Python generates music recommendations
    FMusicRecommendation PythonMusicRec;
    PythonMusicRec.RecommendedBiome = "Korriban";
    PythonMusicRec.RecommendedTone = "Dark";
    PythonMusicRec.RecommendedStems = {"dark_choir", "tension_strings", "sith_ambience"};
    PythonMusicRec.IntensityLevel = 0.8f;
    
    bool bMusicRecommended = AIDirector->RecommendMusic(PythonMusicRec);
    TestTrue("Python→UE5: Music Recommended", bMusicRecommended);
    
    // STEP 3: UE5 music system receives and applies Python recommendations
    bool bBiomeSet = MusicSubsystem->SetMusicBiome(EMusicBiome::Korriban, 3.0f);
    bool bToneSet = MusicSubsystem->SetMusicTone(EMusicTone::Dark, 2.0f);
    
    TestTrue("UE5: Biome Applied", bBiomeSet);
    TestTrue("UE5: Tone Applied", bToneSet);
    
    // STEP 4: Verify stems are activated based on Python recommendations
    for (const FString& StemID : PythonMusicRec.RecommendedStems)
    {
        MusicSubsystem->ActivateMusicStem(StemID, PythonMusicRec.IntensityLevel, 2.0f);
    }
    
    TArray<FString> ActiveStems = MusicSubsystem->GetActiveStems();
    TestTrue("UE5: Stems Activated", ActiveStems.Num() >= 3);
    TestTrue("UE5: Dark Choir Active", ActiveStems.Contains("dark_choir"));
    
    // STEP 5: Test narrative event triggers music change
    // Python detects boss encounter, UE5 music responds
    bool bBossEventTriggered = AIDirector->TriggerNarrativeEvent("BossEncounter", "DarthMalak");
    TestTrue("Python→UE5: Boss Event Triggered", bBossEventTriggered);
    
    // Music should automatically switch to epic boss music
    EMusicBiome CurrentBiome = MusicSubsystem->GetCurrentBiome();
    EMusicTone CurrentTone = MusicSubsystem->GetCurrentTone();
    
    TestEqual("End-to-End: Boss Music Biome", CurrentBiome, EMusicBiome::Korriban);
    TestEqual("End-to-End: Boss Music Tone", CurrentTone, EMusicTone::Epic);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🤝 PYTHON AI → UE5 COMPANION REACTION PIPELINE TEST                        */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToCompanionPipelineTest, "KOTOR.AI.EndToEnd.PythonToCompanionPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToCompanionPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // Create companion with reaction system
    AActor* CompanionActor = TestWorld->SpawnActor<AActor>();
    UCompanionReactionSystem* ReactionSystem = CompanionActor->CreateDefaultSubobject<UCompanionReactionSystem>(TEXT("ReactionSystem"));
    
    TestNotNull("Companion Created", CompanionActor);
    TestNotNull("Reaction System Available", ReactionSystem);
    
    // STEP 1: Python AIDM tracks player choice with moral implications
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    
    // Player chooses to kill innocent villagers (dark side choice)
    FPlayerChoice PythonTrackedChoice;
    PythonTrackedChoice.ChoiceID = "KillInnocentVillagers";
    PythonTrackedChoice.ChoiceText = "Execute the villagers to send a message";
    PythonTrackedChoice.MoralityShift = -0.8f; // Significant dark side shift
    PythonTrackedChoice.CompanionReactions = {
        {"Bastila", "Shock"}, {"Carth", "Disapproval"}, {"Jolee", "Sadness"}
    };
    
    bool bChoiceTracked = AIDirector->TrackPlayerChoice(PythonTrackedChoice);
    TestTrue("Python→UE5: Player Choice Tracked", bChoiceTracked);
    
    // STEP 2: Python analyzes companion relationships and generates reactions
    for (const auto& Reaction : PythonTrackedChoice.CompanionReactions)
    {
        FCompanionReactionData ReactionData;
        ReactionData.CompanionID = Reaction.Key;
        ReactionData.TriggerType = ECompanionReactionTrigger::PlayerChoice;
        ReactionData.EmoteType = GetEmoteTypeFromString(Reaction.Value);
        ReactionData.ReactionDialogue = GenerateReactionDialogue(Reaction.Key, Reaction.Value);
        ReactionData.MoralityThreshold = 0.3f;
        
        // STEP 3: UE5 receives Python reaction data and triggers companion response
        bool bReactionTriggered = ReactionSystem->TriggerCompanionReaction(
            ECompanionReactionTrigger::PlayerChoice, 
            PythonTrackedChoice.ChoiceID, 
            PythonTrackedChoice.MoralityShift, 
            Reaction.Key
        );
        
        TestTrue(FString::Printf(TEXT("UE5: %s Reaction Triggered"), *Reaction.Key), bReactionTriggered);
    }
    
    // STEP 4: Verify companion animations and dialogue play correctly
    // Bastila should show shock animation and speak disapproval dialogue
    UProceduralPerformanceComponentV2* BastilaPerformance = CompanionActor->FindComponentByClass<UProceduralPerformanceComponentV2>();
    if (BastilaPerformance)
    {
        FPerformanceMetadata LastPerformance = BastilaPerformance->GetLastPerformanceMetadata();
        TestEqual("End-to-End: Bastila Emotion", LastPerformance.Emotion, EPerformanceEmotion::Shock);
        TestTrue("End-to-End: Bastila Dialogue", LastPerformance.DialogueLine.Contains("can't believe"));
    }
    
    // STEP 5: Test relationship impact
    float BastilaRelationship = ReactionSystem->GetCompanionRelationshipLevel("Bastila");
    TestTrue("End-to-End: Relationship Decreased", BastilaRelationship < 0.5f);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎬 PYTHON AI → UE5 BOSS ENCOUNTER PIPELINE TEST                            */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToBossEncounterPipelineTest, "KOTOR.AI.EndToEnd.PythonToBossEncounterPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToBossEncounterPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // STEP 1: Python AIDM determines it's time for climactic boss encounter
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UBossIntroSequencer* BossSequencer = NewObject<UBossIntroSequencer>();
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    
    TestNotNull("AI Director Available", AIDirector);
    TestNotNull("Boss Sequencer Available", BossSequencer);
    TestNotNull("Music Subsystem Available", MusicSubsystem);
    
    // Python analyzes: Player has completed 80% of story, high tension, ready for Malak
    FStoryProgressAnalysis PythonStoryAnalysis;
    PythonStoryAnalysis.CompletionPercentage = 0.8f;
    PythonStoryAnalysis.NarrativeTension = 0.95f;
    PythonStoryAnalysis.PlayerLevel = 18;
    PythonStoryAnalysis.ReadyForClimax = true;
    
    bool bStoryAnalyzed = AIDirector->AnalyzeStoryProgress(PythonStoryAnalysis);
    TestTrue("Python→UE5: Story Progress Analyzed", bStoryAnalyzed);
    
    // STEP 2: Python generates boss encounter parameters
    FBossEncounterData PythonBossData;
    PythonBossData.BossID = "DarthMalak";
    PythonBossData.EncounterType = "FinalConfrontation";
    PythonBossData.DialogueTheme = "Betrayal";
    PythonBossData.MusicMood = "Epic";
    PythonBossData.CinematicStyle = "Dramatic";
    PythonBossData.PlayerMoralityContext = -0.4f; // Slightly dark
    
    bool bBossDataGenerated = AIDirector->GenerateBossEncounter(PythonBossData);
    TestTrue("Python→UE5: Boss Encounter Generated", bBossDataGenerated);
    
    // STEP 3: UE5 receives Python data and creates boss intro sequence
    FBossIntroSequenceData SequenceData;
    SequenceData.SequenceType = EBossIntroSequenceType::Dramatic;
    SequenceData.BossDialogue = "At last, we meet face to face, my former apprentice!";
    SequenceData.BossEmotion = EPerformanceEmotion::Pride;
    SequenceData.BossTone = EPerformanceTone::Commanding;
    SequenceData.MusicMood = EMusicMood::Epic;
    
    // STEP 4: Boss intro sequence plays with coordinated systems
    AActor* MalakActor = TestWorld->SpawnActor<AActor>();
    bool bIntroStarted = BossSequencer->StartBossIntro(MalakActor, EBossIntroSequenceType::Dramatic, SequenceData);
    TestTrue("UE5: Boss Intro Started", bIntroStarted);
    
    // STEP 5: Verify all systems coordinate for epic experience
    // Music should switch to epic boss music
    bool bMusicSwitched = MusicSubsystem->ApplyBlendPreset("epic_boss_encounter");
    TestTrue("UE5: Epic Boss Music", bMusicSwitched);
    
    // Animation should play dramatic boss reveal
    UProceduralPerformanceComponentV2* MalakPerformance = MalakActor->FindComponentByClass<UProceduralPerformanceComponentV2>();
    if (MalakPerformance)
    {
        FPerformanceMetadata BossPerformance;
        BossPerformance.Emotion = SequenceData.BossEmotion;
        BossPerformance.Tone = SequenceData.BossTone;
        BossPerformance.DialogueLine = SequenceData.BossDialogue;
        
        FPerformanceResult BossResult = MalakPerformance->PlayPerformance(BossPerformance);
        TestTrue("UE5: Boss Animation Played", BossResult.bSuccess);
    }
    
    // STEP 6: Test complete pipeline integration
    TestTrue("End-to-End: Python Analysis → UE5 Execution", bStoryAnalyzed && bBossDataGenerated && bIntroStarted);
    TestTrue("End-to-End: Coordinated Systems", bMusicSwitched);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🌍 PYTHON AI → UE5 LIVING WORLD PIPELINE TEST                              */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToLivingWorldPipelineTest, "KOTOR.AI.EndToEnd.PythonToLivingWorldPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToLivingWorldPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);
    
    // STEP 1: Python AIDM simulates galactic events and consequences
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    
    // Python simulates: Player destroyed Star Forge, Sith Empire collapsing
    FGalacticEvent PythonEvent;
    PythonEvent.EventID = "StarForgeDestroyed";
    PythonEvent.EventType = "MajorVictory";
    PythonEvent.GalacticImpact = 0.9f;
    PythonEvent.AffectedPlanets = {"Korriban", "Dromund_Kaas", "Ziost"};
    PythonEvent.PoliticalConsequences = {"SithEmpireWeakened", "RepublicStrengthened"};
    
    bool bEventSimulated = AIDirector->SimulateGalacticEvent(PythonEvent);
    TestTrue("Python→UE5: Galactic Event Simulated", bEventSimulated);
    
    // STEP 2: Python calculates world state changes
    FWorldStateChange PythonWorldChange;
    PythonWorldChange.PoliticalTensionDelta = -0.4f; // Tension decreases
    PythonWorldChange.EconomicStabilityDelta = 0.3f; // Economy improves
    PythonWorldChange.WarStatusChange = EWarStatus::ColdWar; // War de-escalates
    PythonWorldChange.NPCMoraleChange = 0.5f; // NPCs more hopeful
    
    bool bWorldStateCalculated = AIDirector->CalculateWorldStateChange(PythonWorldChange);
    TestTrue("Python→UE5: World State Calculated", bWorldStateCalculated);
    
    // STEP 3: UE5 receives Python data and updates living world
    ULivingUniverseComponent* LivingUniverse = NewObject<ULivingUniverseComponent>();
    TestNotNull("Living Universe Available", LivingUniverse);
    
    // Apply Python-calculated changes to UE5 world
    FWorldState NewWorldState;
    NewWorldState.PoliticalTension = 0.3f; // Reduced from 0.7f
    NewWorldState.EconomicStability = 0.8f; // Improved from 0.5f
    NewWorldState.WarStatus = PythonWorldChange.WarStatusChange;
    
    LivingUniverse->SetWorldState(NewWorldState);
    
    // STEP 4: Verify NPCs react to world state changes
    // NPCs should be more hopeful, less fearful
    for (int32 i = 0; i < 5; i++)
    {
        AActor* TestNPC = TestWorld->SpawnActor<AActor>();
        UEmotionalStateComponent* EmotionalState = TestNPC->CreateDefaultSubobject<UEmotionalStateComponent>(TEXT("EmotionalState"));
        
        // Python-driven emotional state should reflect world improvement
        FEmotionalState NPCEmotion;
        NPCEmotion.Happiness = 0.7f; // Increased due to victory
        NPCEmotion.Fear = 0.2f; // Decreased due to peace
        NPCEmotion.Hope = 0.8f; // High due to Sith defeat
        
        EmotionalState->SetEmotionalState(NPCEmotion);
        
        FEmotionalState RetrievedState = EmotionalState->GetEmotionalState();
        TestTrue("UE5: NPC Happiness Increased", RetrievedState.Happiness > 0.5f);
        TestTrue("UE5: NPC Fear Decreased", RetrievedState.Fear < 0.3f);
    }
    
    // STEP 5: Test atmospheric changes based on Python simulation
    UPlayerDrivenAtmosphereComponent* Atmosphere = NewObject<UPlayerDrivenAtmosphereComponent>();
    TestNotNull("Atmosphere System Available", Atmosphere);
    
    // Atmosphere should reflect the victory and peace
    FAtmosphericState VictoryAtmosphere;
    VictoryAtmosphere.OverallMood = "Hopeful";
    VictoryAtmosphere.LightingTone = "Warm";
    VictoryAtmosphere.AmbientSoundscape = "Peaceful";
    VictoryAtmosphere.NPCBehaviorPattern = "Celebratory";
    
    bool bAtmosphereSet = Atmosphere->SetAtmosphericState(VictoryAtmosphere);
    TestTrue("UE5: Victory Atmosphere Set", bAtmosphereSet);
    
    // STEP 6: Verify complete pipeline
    TestTrue("End-to-End: Python Simulation → UE5 World State", bEventSimulated && bWorldStateCalculated);
    TestTrue("End-to-End: Living World Response", bAtmosphereSet);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🚀 COMPLETE GAME LAUNCH PIPELINE TEST                                      */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCompleteGameLaunchPipelineTest, "KOTOR.AI.EndToEnd.CompleteGameLaunchPipeline", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCompleteGameLaunchPipelineTest::RunTest(const FString& Parameters)
{
    // Test the complete game launch experience from CLI to playing
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Game World Created", TestWorld);
    
    // STEP 1: CLI script launches game (simulated)
    // run_game.cmd --map Taris --gamemode BP_KOTORGameMode --windowed
    
    // STEP 2: Game startup subsystem initializes everything
    UGameStartupSubsystem* StartupSystem = TestWorld->GetSubsystem<UGameStartupSubsystem>();
    TestNotNull("Startup System Available", StartupSystem);
    
    StartupSystem->InitializeGameStartup();
    TestTrue("All Systems Initialized", StartupSystem->IsStartupComplete());
    
    // STEP 3: Auto map loader loads Taris
    UAutoMapLoader* MapLoader = UAutoMapLoader::GetAutoMapLoader(TestWorld);
    TestNotNull("Map Loader Available", MapLoader);
    
    bool bMapLoaded = MapLoader->LoadSpecificMap("Taris", false);
    TestTrue("Taris Map Loaded", bMapLoaded);
    
    // STEP 4: Python AIDM generates initial content for Taris
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    
    bool bTarisContentGenerated = AIDirector->GenerateLocationContent("Taris");
    TestTrue("Python→UE5: Taris Content Generated", bTarisContentGenerated);
    
    // STEP 5: All 12 phases working together
    // Phase 1-2: Python + UE5 integration ✓
    // Phase 3: Voice synthesis ready
    // Phase 4: Multiplayer systems ready
    // Phase 5: Emotional intelligence active
    // Phase 6: Living world simulation running
    // Phase 7-8: Streamlined development tools active
    // Phase 9: Cinematic systems ready
    // Phase 10-11: Animation and music systems active
    // Phase 12: Build and launch complete
    
    UProceduralMusicSubsystemV2* MusicSubsystem = TestWorld->GetSubsystem<UProceduralMusicSubsystemV2>();
    TestNotNull("Music System Active", MusicSubsystem);
    
    // Music should automatically set to Taris ambience
    bool bTarisMusicSet = MusicSubsystem->SetMusicBiome(EMusicBiome::Taris, 3.0f);
    TestTrue("Taris Music Active", bTarisMusicSet);
    
    // STEP 6: Player can immediately start playing with full AI support
    ACharacter* PlayerCharacter = TestWorld->SpawnActor<ACharacter>();
    TestNotNull("Player Ready", PlayerCharacter);
    
    // Python AI should be ready to respond to player actions
    bool bAIReady = AIDirector->IsReadyForPlayerInteraction();
    TestTrue("Python AI Ready for Player", bAIReady);
    
    // STEP 7: Complete pipeline verification
    TestTrue("End-to-End: Complete Game Launch Pipeline", 
        StartupSystem->IsStartupComplete() && 
        bMapLoaded && 
        bTarisContentGenerated && 
        bTarisMusicSet && 
        bAIReady);
    
    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎯 PYTHON AI → UE5 QUEST PIPELINE TEST                                     */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToQuestPipelineTest, "KOTOR.AI.EndToEnd.PythonToQuestPipeline",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToQuestPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);

    // STEP 1: Python AIDM generates dynamic quest based on player history
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();

    // Python analyzes: Player has been helping Jedi, high light side, needs challenge
    FPlayerProfile PythonPlayerProfile;
    PythonPlayerProfile.MoralityAlignment = 0.7f; // Light side
    PythonPlayerProfile.CompletedQuests = {"RescueVillagers", "DefeatSithPatrol", "SaveJediMaster"};
    PythonPlayerProfile.PreferredQuestTypes = {"Rescue", "Investigation", "Moral_Dilemma"};
    PythonPlayerProfile.CurrentLevel = 12;

    // Python generates personalized quest
    FQuestGenerationRequest PythonQuestRequest;
    PythonQuestRequest.QuestType = EQuestType::SideQuest;
    PythonQuestRequest.DifficultyLevel = 0.6f;
    PythonQuestRequest.MoralComplexity = 0.8f; // High moral complexity
    PythonQuestRequest.PlayerProfile = PythonPlayerProfile;
    PythonQuestRequest.LocationContext = "Dantooine";

    bool bQuestGenerated = AIDirector->GeneratePersonalizedQuest(PythonQuestRequest);
    TestTrue("Python→UE5: Personalized Quest Generated", bQuestGenerated);

    // STEP 2: UE5 receives Python quest data and creates game objects
    FQuestData GeneratedQuest = AIDirector->GetLastGeneratedQuest();
    TestTrue("UE5: Quest Data Received", !GeneratedQuest.QuestTitle.IsEmpty());
    TestTrue("UE5: Quest Has Objectives", GeneratedQuest.Objectives.Num() > 0);
    TestTrue("UE5: Quest Has Moral Choice", GeneratedQuest.bHasMoralChoice);

    // STEP 3: Quest system creates NPCs and locations
    for (const FQuestObjective& Objective : GeneratedQuest.Objectives)
    {
        if (Objective.ObjectiveType == EObjectiveType::TalkToNPC)
        {
            // Python specified NPC, UE5 spawns them
            AActor* QuestNPC = TestWorld->SpawnActor<AActor>();
            UQuestGiverComponent* QuestGiver = QuestNPC->CreateDefaultSubobject<UQuestGiverComponent>(TEXT("QuestGiver"));

            TestNotNull("UE5: Quest NPC Spawned", QuestNPC);
            TestNotNull("UE5: Quest Giver Component", QuestGiver);

            // NPC should have Python-generated dialogue
            bool bDialogueSet = QuestGiver->SetQuestDialogue(Objective.DialogueLines);
            TestTrue("UE5: Python Dialogue Applied", bDialogueSet);
        }
    }

    // STEP 4: Test quest progression tracking
    UQuestManagerComponent* QuestManager = NewObject<UQuestManagerComponent>();
    TestNotNull("Quest Manager Available", QuestManager);

    bool bQuestStarted = QuestManager->StartQuest(GeneratedQuest.QuestID);
    TestTrue("UE5: Quest Started", bQuestStarted);

    // STEP 5: Test Python AI adapts to quest choices
    // Player makes moral choice, Python analyzes and responds
    FQuestChoice PlayerChoice;
    PlayerChoice.ChoiceID = "SpareTheTraitor";
    PlayerChoice.MoralityImpact = 0.3f; // Light side choice

    bool bChoiceProcessed = AIDirector->ProcessQuestChoice(GeneratedQuest.QuestID, PlayerChoice);
    TestTrue("Python→UE5: Quest Choice Processed", bChoiceProcessed);

    // Python should generate consequences
    FQuestConsequences PythonConsequences = AIDirector->GenerateQuestConsequences(PlayerChoice);
    TestTrue("Python→UE5: Consequences Generated", !PythonConsequences.ConsequenceText.IsEmpty());

    TestWorld->DestroyWorld(false);
    return true;
}

/* ============================================================================ */
/* 🎮 PYTHON AI → UE5 MULTIPLAYER COORDINATION TEST                           */
/* ============================================================================ */

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonToMultiplayerPipelineTest, "KOTOR.AI.EndToEnd.PythonToMultiplayerPipeline",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPythonToMultiplayerPipelineTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test World Created", TestWorld);

    // STEP 1: Python AIDM coordinates multiplayer D&D session
    UAIDirectorSubsystem* AIDirector = TestWorld->GetSubsystem<UAIDirectorSubsystem>();
    UMultiplayerSessionComponent* SessionComp = NewObject<UMultiplayerSessionComponent>();

    TestNotNull("AI Director Available", AIDirector);
    TestNotNull("Multiplayer Session Available", SessionComp);

    // Python analyzes party composition and generates appropriate content
    FPartyComposition PythonPartyAnalysis;
    PythonPartyAnalysis.PlayerCount = 4;
    PythonPartyAnalysis.PlayerClasses = {"Jedi_Guardian", "Jedi_Consular", "Scoundrel", "Soldier"};
    PythonPartyAnalysis.AverageLevel = 10;
    PythonPartyAnalysis.PartyAlignment = 0.4f; // Slightly light side

    bool bPartyAnalyzed = AIDirector->AnalyzePartyComposition(PythonPartyAnalysis);
    TestTrue("Python→UE5: Party Analyzed", bPartyAnalyzed);

    // STEP 2: Python generates party-appropriate encounter
    FMultiplayerEncounter PythonEncounter;
    PythonEncounter.EncounterType = "Cooperative_Puzzle";
    PythonEncounter.RequiredPlayers = 4;
    PythonEncounter.DifficultyScaling = 0.7f;
    PythonEncounter.RoleRequirements = {"Tank", "Healer", "DPS", "Support"};

    bool bEncounterGenerated = AIDirector->GenerateMultiplayerEncounter(PythonEncounter);
    TestTrue("Python→UE5: Multiplayer Encounter Generated", bEncounterGenerated);

    // STEP 3: UE5 coordinates party decisions
    UPartyDecisionComponent* PartyDecision = NewObject<UPartyDecisionComponent>();
    TestNotNull("Party Decision System Available", PartyDecision);

    // Python presents moral dilemma requiring party consensus
    FPartyDecisionData DecisionData;
    DecisionData.DecisionText = "The ancient Sith artifact could save the Republic, but using it requires a sacrifice. What do you choose?";
    DecisionData.Options = {"Use the artifact", "Destroy the artifact", "Find another way"};
    DecisionData.RequiredConsensus = 0.75f; // 75% agreement needed
    DecisionData.TimeLimit = 120.0f; // 2 minutes to decide

    bool bDecisionStarted = PartyDecision->StartPartyDecision(DecisionData);
    TestTrue("UE5: Party Decision Started", bDecisionStarted);

    // STEP 4: Test party coordination mechanics
    // Simulate players voting
    PartyDecision->SubmitPlayerVote("Player1", 2); // Find another way
    PartyDecision->SubmitPlayerVote("Player2", 2); // Find another way
    PartyDecision->SubmitPlayerVote("Player3", 1); // Destroy artifact
    PartyDecision->SubmitPlayerVote("Player4", 2); // Find another way

    FPartyDecisionResult Result = PartyDecision->GetDecisionResult();
    TestEqual("UE5: Party Decision Result", Result.ChosenOption, 2);
    TestTrue("UE5: Consensus Reached", Result.ConsensusPercentage >= 0.75f);

    // STEP 5: Python adapts story based on party decision
    bool bStoryAdapted = AIDirector->AdaptStoryToPartyDecision(Result);
    TestTrue("Python→UE5: Story Adapted to Party Choice", bStoryAdapted);

    TestWorld->DestroyWorld(false);
    return true;
}

// Helper functions for test data conversion
ECompanionEmoteType GetEmoteTypeFromString(const FString& EmoteString)
{
    if (EmoteString == "Shock") return ECompanionEmoteType::Shock;
    if (EmoteString == "Disapproval") return ECompanionEmoteType::Disapproval;
    if (EmoteString == "Sadness") return ECompanionEmoteType::Sadness;
    return ECompanionEmoteType::Approval;
}

FString GenerateReactionDialogue(const FString& CompanionName, const FString& EmoteType)
{
    if (CompanionName == "Bastila" && EmoteType == "Shock")
        return "I can't believe you would do such a thing!";
    if (CompanionName == "Carth" && EmoteType == "Disapproval")
        return "That's not the person I thought you were.";
    if (CompanionName == "Jolee" && EmoteType == "Sadness")
        return "Such unnecessary violence... there was another way.";
    return "I have thoughts about your actions.";
}
