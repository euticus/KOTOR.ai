// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Timeline/CampaignTimelineComponent.h"
#include "Cinematics/ProceduralCutsceneGenerator.h"
#include "AlternateRealitySimulator.generated.h"

/**
 * What-if scenario types
 */
UENUM(BlueprintType)
enum class EWhatIfScenarioType : uint8
{
    AlternateChoice     UMETA(DisplayName = "Alternate Choice"),
    DifferentPath       UMETA(DisplayName = "Different Path"),
    OppositeAlignment   UMETA(DisplayName = "Opposite Alignment"),
    CompanionSurvival   UMETA(DisplayName = "Companion Survival"),
    FactionAlliance     UMETA(DisplayName = "Faction Alliance"),
    EarlyDeath          UMETA(DisplayName = "Early Death"),
    PowerfulArtifact    UMETA(DisplayName = "Powerful Artifact"),
    TimelineShift       UMETA(DisplayName = "Timeline Shift"),
    RoleReversal        UMETA(DisplayName = "Role Reversal"),
    CosmicIntervention  UMETA(DisplayName = "Cosmic Intervention")
};

/**
 * Vision shrine types
 */
UENUM(BlueprintType)
enum class EVisionShrineType : uint8
{
    ForceShrine         UMETA(DisplayName = "Force Shrine"),
    AncientObelisk      UMETA(DisplayName = "Ancient Obelisk"),
    CrystalFormation    UMETA(DisplayName = "Crystal Formation"),
    HolocronPedestal    UMETA(DisplayName = "Holocron Pedestal"),
    SithAltar           UMETA(DisplayName = "Sith Altar"),
    JediMeditation      UMETA(DisplayName = "Jedi Meditation"),
    TimeRift            UMETA(DisplayName = "Time Rift"),
    CosmicNexus         UMETA(DisplayName = "Cosmic Nexus")
};

/**
 * What-if scenario data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FWhatIfScenario
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    FString ScenarioID;

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    FString ScenarioName;

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    EWhatIfScenarioType ScenarioType;

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    FString Description; // LLM-generated scenario description

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    FString AlternateChoice; // The choice that would have been made

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    FString OriginalChoice; // The choice that was actually made

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    TArray<FString> ConsequenceChain; // Chain of consequences

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    TMap<FString, FString> AlternateOutcomes; // Key events -> alternate outcomes

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    TArray<FString> VisionSequence; // Sequence of vision scenes

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    float VisionDuration; // Total duration of vision

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    TArray<FString> UnlockedInsights; // Insights gained from vision

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    TArray<FString> UnlockedDialogue; // New dialogue options unlocked

    UPROPERTY(BlueprintReadWrite, Category = "What If Scenario")
    TArray<FString> UnlockedQuests; // Hidden quests revealed

    FWhatIfScenario()
    {
        ScenarioID = TEXT("");
        ScenarioName = TEXT("What If Scenario");
        ScenarioType = EWhatIfScenarioType::AlternateChoice;
        Description = TEXT("");
        AlternateChoice = TEXT("");
        OriginalChoice = TEXT("");
        VisionDuration = 60.0f;
    }
};

/**
 * Alternate reality state
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAlternateRealityState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Alternate Reality")
    FString StateID;

    UPROPERTY(BlueprintReadWrite, Category = "Alternate Reality")
    TArray<FNarrativeMemory> ModifiedMemories; // Modified campaign memories

    UPROPERTY(BlueprintReadWrite, Category = "Alternate Reality")
    TArray<FTimelineEvent> AlternateTimeline; // Alternate timeline events

    UPROPERTY(BlueprintReadWrite, Category = "Alternate Reality")
    TMap<FString, FString> CharacterFates; // Character -> fate in this reality

    UPROPERTY(BlueprintReadWrite, Category = "Alternate Reality")
    TMap<FString, FString> WorldStates; // Planet -> state in this reality

    UPROPERTY(BlueprintReadWrite, Category = "Alternate Reality")
    float DivergencePoint; // When this reality diverged from main timeline

    FAlternateRealityState()
    {
        StateID = TEXT("");
        DivergencePoint = 0.0f;
    }
};

/**
 * Vision events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWhatIfVisionStarted, const FWhatIfScenario&, Scenario);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWhatIfVisionCompleted, const FWhatIfScenario&, Scenario);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInsightUnlocked, const FString&, InsightType, const FString&, InsightContent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHiddenQuestRevealed, const FString&, QuestID);

/**
 * Vision Shrine Actor - Interactable shrine for What-If visions
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AVisionShrine : public AActor
{
    GENERATED_BODY()

public:
    AVisionShrine();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Activate vision shrine
     * @param PlayerCharacter Player activating the shrine
     */
    UFUNCTION(BlueprintCallable, Category = "Vision Shrine")
    void ActivateVisionShrine(APawn* PlayerCharacter);

    /**
     * Get available scenarios
     * @return Array of available What-If scenarios
     */
    UFUNCTION(BlueprintCallable, Category = "Vision Shrine")
    TArray<FWhatIfScenario> GetAvailableScenarios() const;

    /**
     * Set shrine type
     * @param NewShrineType Type of shrine
     */
    UFUNCTION(BlueprintCallable, Category = "Vision Shrine")
    void SetShrineType(EVisionShrineType NewShrineType);

protected:
    // Shrine data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision Shrine")
    EVisionShrineType ShrineType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision Shrine")
    TArray<EWhatIfScenarioType> SupportedScenarios; // Scenario types this shrine supports

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision Shrine")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision Shrine")
    float CooldownTime; // Time between uses

    UPROPERTY()
    float LastActivationTime;

    // Visual components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ShrineMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* InteractionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UParticleSystemComponent* VisionAura;

    // Interaction events
    UFUNCTION()
    void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                        bool bFromSweep, const FHitResult& SweepResult);

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when vision shrine is activated
     * @param PlayerCharacter Player activating the shrine
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Shrine Events")
    void OnVisionShrineActivated(APawn* PlayerCharacter);

    /**
     * Called to customize shrine appearance based on type
     * @param ShrineType Type of the shrine
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Shrine Events")
    void UpdateShrineAppearance(EVisionShrineType InputShrineType);
};

/**
 * Alternate Reality Simulator - Simulates "What If" scenarios and visions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAlternateRealitySimulator : public UActorComponent
{
    GENERATED_BODY()

public:
    UAlternateRealitySimulator();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize alternate reality simulator
     * @param CampaignLoader Campaign loader for timeline data
     * @param NarrativeMemory Narrative memory for choice tracking
     * @param Timeline Campaign timeline for divergence points
     * @param CutsceneGenerator Cutscene generator for vision sequences
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    void InitializeAlternateRealitySimulator(UCampaignLoaderSubsystem* CampaignLoader,
                                            UNarrativeMemoryComponent* NarrativeMemory,
                                            UCampaignTimelineComponent* Timeline,
                                            UProceduralCutsceneGenerator* CutsceneGenerator);

    /**
     * Generate What-If scenario
     * @param ScenarioType Type of scenario to generate
     * @param FocusEvent Specific event to focus on (optional)
     * @return Generated What-If scenario
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    FWhatIfScenario GenerateWhatIfScenario(EWhatIfScenarioType ScenarioType, const FString& FocusEvent = TEXT(""));

    /**
     * Start What-If vision
     * @param Scenario Scenario to visualize
     * @return True if vision started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    bool StartWhatIfVision(const FWhatIfScenario& Scenario);

    /**
     * Complete What-If vision
     * @param bUnlockInsights Whether to unlock insights from the vision
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    void CompleteWhatIfVision(bool bUnlockInsights = true);

    /**
     * Create alternate reality state
     * @param DivergencePoint When the alternate reality diverged
     * @param AlternateChoice The choice that created the divergence
     * @return Generated alternate reality state
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    FAlternateRealityState CreateAlternateRealityState(float DivergencePoint, const FString& AlternateChoice);

    /**
     * Get available What-If scenarios
     * @param ShrineType Type of shrine (affects available scenarios)
     * @return Array of available scenarios
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    TArray<FWhatIfScenario> GetAvailableWhatIfScenarios(EVisionShrineType InputShrineType) const;

    /**
     * Check if vision is active
     * @return True if a What-If vision is currently playing
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alternate Reality")
    bool IsVisionActive() const { return bVisionActive; }

    /**
     * Get current scenario
     * @return Currently active What-If scenario
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alternate Reality")
    FWhatIfScenario GetCurrentScenario() const { return CurrentScenario; }

    /**
     * Register vision shrine
     * @param VisionShrine Shrine to register
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    void RegisterVisionShrine(AVisionShrine* VisionShrine);

    /**
     * Unlock insight from vision
     * @param InsightType Type of insight
     * @param InsightContent Content of the insight
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    void UnlockInsight(const FString& InsightType, const FString& InsightContent);

    /**
     * Reveal hidden quest
     * @param QuestID ID of the quest to reveal
     */
    UFUNCTION(BlueprintCallable, Category = "Alternate Reality")
    void RevealHiddenQuest(const FString& QuestID);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnWhatIfVisionStarted OnWhatIfVisionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnWhatIfVisionCompleted OnWhatIfVisionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnInsightUnlocked OnInsightUnlocked;

    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnHiddenQuestRevealed OnHiddenQuestRevealed;

protected:
    // Vision state
    UPROPERTY(BlueprintReadOnly, Category = "Alternate Reality")
    bool bVisionActive;

    UPROPERTY(BlueprintReadOnly, Category = "Alternate Reality")
    FWhatIfScenario CurrentScenario;

    UPROPERTY(BlueprintReadOnly, Category = "Alternate Reality")
    FAlternateRealityState CurrentAlternateState;

    // Generated scenarios
    UPROPERTY(BlueprintReadOnly, Category = "Alternate Reality")
    TArray<FWhatIfScenario> GeneratedScenarios;

    UPROPERTY(BlueprintReadOnly, Category = "Alternate Reality")
    TArray<FString> UnlockedInsights;

    // Registered shrines
    UPROPERTY(BlueprintReadOnly, Category = "Alternate Reality")
    TArray<AVisionShrine*> RegisteredShrines;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UCampaignTimelineComponent* TimelineRef;

    UPROPERTY()
    UProceduralCutsceneGenerator* CutsceneGeneratorRef;

    // Scenario templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Templates")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<EWhatIfScenarioType, TArray<FString>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by EWhatIfScenarioType when needed.
     */
    TArray<FString>ScenarioPromptTemplates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scenario Templates")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<EVisionShrineType, TArray<EWhatIfScenarioType>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by EVisionShrineType when needed.
     */
    TArray<EWhatIfScenarioType>ShrineScenarioMapping;

private:
    // Helper methods
    FString GenerateScenarioID();
    FString GenerateStateID();
    FString BuildScenarioPrompt(EWhatIfScenarioType ScenarioType, const FString& FocusEvent);
    TArray<FString> FindMajorChoices();
    TArray<FString> GenerateConsequenceChain(const FString& AlternateChoice);
    TArray<FString> CreateVisionSequence(const FWhatIfScenario& Scenario);
    void ModifyTimelineForAlternate(FAlternateRealityState& AlternateState, const FString& AlternateChoice);

public:
    /**
     * Blueprint implementable events for custom vision logic
     */

    /**
     * Called when What-If vision starts (for custom setup)
     * @param Scenario The starting scenario
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void OnWhatIfVisionStartedEvent(const FWhatIfScenario& Scenario);

    /**
     * Called when What-If vision completes (for custom processing)
     * @param Scenario The completed scenario
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void OnWhatIfVisionCompletedEvent(const FWhatIfScenario& Scenario);

    /**
     * Called to generate custom What-If scenario (override in Blueprint)
     * @param ScenarioType Type of scenario to generate
     * @param Context Current campaign context
     * @return Custom generated scenario
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    FWhatIfScenario GenerateCustomWhatIfScenario(EWhatIfScenarioType ScenarioType, const FString& Context);

    /**
     * Called to create custom vision sequence (override in Blueprint)
     * @param Scenario The What-If scenario
     * @return Custom vision sequence
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    TArray<FString> CreateCustomVisionSequence(const FWhatIfScenario& Scenario);
};
