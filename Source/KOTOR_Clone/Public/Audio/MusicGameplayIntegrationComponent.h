// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Audio/ProceduralMusicSubsystem.h"
#include "Audio/MusicBlendingComponent.h"
#include "Multiplayer/MultiplayerDecisionSyncComponent.h"
#include "MusicGameplayIntegrationComponent.generated.h"

/**
 * Music trigger types
 */
UENUM(BlueprintType)
enum class EMusicTriggerType : uint8
{
    AlignmentShift      UMETA(DisplayName = "Alignment Shift"),
    CombatStart         UMETA(DisplayName = "Combat Start"),
    CombatEnd           UMETA(DisplayName = "Combat End"),
    BossEncounter       UMETA(DisplayName = "Boss Encounter"),
    BossPhaseChange     UMETA(DisplayName = "Boss Phase Change"),
    PlayerDeath         UMETA(DisplayName = "Player Death"),
    Victory             UMETA(DisplayName = "Victory"),
    Defeat              UMETA(DisplayName = "Defeat"),
    QuestComplete       UMETA(DisplayName = "Quest Complete"),
    QuestFail           UMETA(DisplayName = "Quest Fail"),
    ItemAcquired        UMETA(DisplayName = "Item Acquired"),
    LocationEnter       UMETA(DisplayName = "Location Enter"),
    LocationExit        UMETA(DisplayName = "Location Exit"),
    DialogueStart       UMETA(DisplayName = "Dialogue Start"),
    DialogueEnd         UMETA(DisplayName = "Dialogue End"),
    DecisionMade        UMETA(DisplayName = "Decision Made"),
    Betrayal            UMETA(DisplayName = "Betrayal"),
    Romance             UMETA(DisplayName = "Romance"),
    Sacrifice           UMETA(DisplayName = "Sacrifice"),
    Discovery           UMETA(DisplayName = "Discovery"),
    Stealth             UMETA(DisplayName = "Stealth"),
    Tension             UMETA(DisplayName = "Tension"),
    Relief              UMETA(DisplayName = "Relief")
};

/**
 * Music trigger data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicTriggerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    FString TriggerID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    EMusicTriggerType TriggerType;

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    EMusicMood TargetMood;

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    EMusicContext TargetContext;

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float IntensityChange; // -1.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float TensionChange; // -1.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float EnergyChange; // -1.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float BlendTime; // Time to transition

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    TArray<FString> LayersToActivate;

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    TArray<FString> LayersToDeactivate;

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float Priority; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float Cooldown; // Seconds before trigger can fire again

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    bool bOverrideCurrentMusic; // Whether to override current music

    UPROPERTY(BlueprintReadWrite, Category = "Music Trigger")
    float LastTriggeredTime;

    FMusicTriggerData()
    {
        TriggerID = TEXT("");
        TriggerType = EMusicTriggerType::AlignmentShift;
        TargetMood = EMusicMood::Neutral;
        TargetContext = EMusicContext::Exploration;
        IntensityChange = 0.0f;
        TensionChange = 0.0f;
        EnergyChange = 0.0f;
        BlendTime = 3.0f;
        Priority = 0.5f;
        Cooldown = 5.0f;
        bOverrideCurrentMusic = false;
        LastTriggeredTime = 0.0f;
    }
};

/**
 * Alignment music mapping
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAlignmentMusicMapping
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Alignment Music")
    float AlignmentThreshold; // -1.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Alignment Music")
    EMusicMood AssociatedMood;

    UPROPERTY(BlueprintReadWrite, Category = "Alignment Music")
    TArray<FString> LayersToActivate;

    UPROPERTY(BlueprintReadWrite, Category = "Alignment Music")
    TArray<FString> LayersToDeactivate;

    UPROPERTY(BlueprintReadWrite, Category = "Alignment Music")
    float VolumeMultiplier; // 0.0 to 2.0

    FAlignmentMusicMapping()
    {
        AlignmentThreshold = 0.0f;
        AssociatedMood = EMusicMood::Neutral;
        VolumeMultiplier = 1.0f;
    }
};

/**
 * Combat music phase
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCombatMusicPhase
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Combat Music Phase")
    FString PhaseID;

    UPROPERTY(BlueprintReadWrite, Category = "Combat Music Phase")
    float HealthThreshold; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Combat Music Phase")
    EMusicMood PhaseMood;

    UPROPERTY(BlueprintReadWrite, Category = "Combat Music Phase")
    float PhaseIntensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Combat Music Phase")
    TArray<FString> PhaseLayers;

    UPROPERTY(BlueprintReadWrite, Category = "Combat Music Phase")
    float TransitionTime;

    FCombatMusicPhase()
    {
        PhaseID = TEXT("");
        HealthThreshold = 1.0f;
        PhaseMood = EMusicMood::Action;
        PhaseIntensity = 0.8f;
        TransitionTime = 2.0f;
    }
};

/**
 * Music integration events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicTriggerActivated, EMusicTriggerType, TriggerType, const FMusicTriggerData&, TriggerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAlignmentMusicChanged, float, OldAlignment, float, NewAlignment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatMusicPhaseChanged, const FString&, OldPhase, const FString&, NewPhase);

/**
 * Music Gameplay Integration Component - Let moment-to-moment gameplay shift the soundtrack
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMusicGameplayIntegrationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMusicGameplayIntegrationComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize music gameplay integration
     * @param MusicSubsystem Procedural music subsystem
     * @param BlendingComponent Music blending component
     * @param DecisionSync Decision sync component (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void InitializeMusicIntegration(UProceduralMusicSubsystem* MusicSubsystem, 
                                   UMusicBlendingComponent* BlendingComponent,
                                   UMultiplayerDecisionSyncComponent* DecisionSync = nullptr);

    /**
     * Trigger music event
     * @param TriggerType Type of trigger
     * @param Context Additional context
     * @param Intensity Override intensity (optional)
     * @return True if trigger was activated
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    bool TriggerMusicEvent(EMusicTriggerType TriggerType, const FString& Context = TEXT(""), float Intensity = -1.0f);

    /**
     * Update player alignment
     * @param NewAlignment New alignment value (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void UpdatePlayerAlignment(float NewAlignment);

    /**
     * Start combat music
     * @param EnemyCount Number of enemies
     * @param BossLevel Boss level (0 = no boss)
     * @param CombatIntensity Combat intensity (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void StartCombatMusic(int32 EnemyCount, int32 BossLevel = 0, float CombatIntensity = 0.8f);

    /**
     * End combat music
     * @param bVictory Whether combat ended in victory
     * @param TransitionTime Time to transition out
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void EndCombatMusic(bool bVictory = true, float TransitionTime = 3.0f);

    /**
     * Update combat phase
     * @param HealthPercentage Current health percentage (0.0 to 1.0)
     * @param PhaseID Optional specific phase ID
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void UpdateCombatPhase(float HealthPercentage, const FString& PhaseID = TEXT(""));

    /**
     * Add music trigger
     * @param TriggerData Trigger data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void AddMusicTrigger(const FMusicTriggerData& TriggerData);

    /**
     * Remove music trigger
     * @param TriggerID ID of trigger to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void RemoveMusicTrigger(const FString& TriggerID);

    /**
     * Add alignment music mapping
     * @param Mapping Alignment mapping to add
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void AddAlignmentMapping(const FAlignmentMusicMapping& Mapping);

    /**
     * Add combat music phase
     * @param Phase Combat phase to add
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void AddCombatPhase(const FCombatMusicPhase& Phase);

    /**
     * Set music enabled
     * @param bEnabled Whether music integration is enabled
     */
    UFUNCTION(BlueprintCallable, Category = "Music Integration")
    void SetMusicIntegrationEnabled(bool bEnabled);

    /**
     * Get current player alignment
     * @return Current player alignment (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Integration")
    float GetCurrentPlayerAlignment() const { return CurrentPlayerAlignment; }

    /**
     * Check if in combat
     * @return True if currently in combat
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Integration")
    bool IsInCombat() const { return bInCombat; }

    /**
     * Get current combat phase
     * @return Current combat phase ID
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Integration")
    FString GetCurrentCombatPhase() const { return CurrentCombatPhase; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Music Integration Events")
    FOnMusicTriggerActivated OnMusicTriggerActivated;

    UPROPERTY(BlueprintAssignable, Category = "Music Integration Events")
    FOnAlignmentMusicChanged OnAlignmentMusicChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Integration Events")
    FOnCombatMusicPhaseChanged OnCombatMusicPhaseChanged;

protected:
    // Music triggers
    UPROPERTY(BlueprintReadOnly, Category = "Music Triggers")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<EMusicTriggerType, TArray<FMusicTriggerData>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by EMusicTriggerType when needed.
     */
    TArray<FMusicTriggerData>MusicTriggers;

    // Alignment mappings
    UPROPERTY(BlueprintReadOnly, Category = "Alignment Music")
    TArray<FAlignmentMusicMapping> AlignmentMappings;

    // Combat phases
    UPROPERTY(BlueprintReadOnly, Category = "Combat Music")
    TArray<FCombatMusicPhase> CombatPhases;

    // Component references
    UPROPERTY()
    UProceduralMusicSubsystem* ProceduralMusicSubsystem;

    UPROPERTY()
    UMusicBlendingComponent* MusicBlendingComponent;

    UPROPERTY()
    UMultiplayerDecisionSyncComponent* DecisionSyncComponent;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "Current State")
    float CurrentPlayerAlignment;

    UPROPERTY(BlueprintReadOnly, Category = "Current State")
    bool bInCombat;

    UPROPERTY(BlueprintReadOnly, Category = "Current State")
    FString CurrentCombatPhase;

    UPROPERTY(BlueprintReadOnly, Category = "Current State")
    float CombatStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Current State")
    int32 CurrentEnemyCount;

    UPROPERTY(BlueprintReadOnly, Category = "Current State")
    int32 CurrentBossLevel;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    bool bMusicIntegrationEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    float AlignmentSensitivity; // How sensitive to alignment changes

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    float DefaultBlendTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    bool bAutoManageCombatMusic;

private:
    // Helper methods
    void LoadDefaultTriggers();
    void LoadDefaultAlignmentMappings();
    void LoadDefaultCombatPhases();
    FMusicTriggerData* FindBestTrigger(EMusicTriggerType TriggerType, const FString& Context);
    bool CanTrigger(const FMusicTriggerData& TriggerData);
    void ExecuteTrigger(const FMusicTriggerData& TriggerData);
    void UpdateAlignmentMusic();
    FCombatMusicPhase* FindCombatPhase(float HealthPercentage);
    void TransitionToCombatPhase(const FCombatMusicPhase& Phase);

    // Decision sync events
    UFUNCTION()
    void OnDecisionReached(const FDecisionSession& Session, int32 FinalChoice);

    UFUNCTION()
    void OnPlayerVoteCast(const FString& PlayerID, const FPlayerVote& Vote);

public:
    /**
     * Blueprint implementable events for custom music integration logic
     */

    /**
     * Called when music trigger is activated (for custom processing)
     * @param TriggerType Type of trigger
     * @param TriggerData The trigger data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Integration Events")
    void OnMusicTriggerActivatedEvent(EMusicTriggerType TriggerType, const FMusicTriggerData& TriggerData);

    /**
     * Called when alignment music changes (for custom effects)
     * @param OldAlignment Previous alignment
     * @param NewAlignment New alignment
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Integration Events")
    void OnAlignmentMusicChangedEvent(float OldAlignment, float NewAlignment);

    /**
     * Called to customize trigger response (override in Blueprint)
     * @param TriggerType Type of trigger
     * @param Context Trigger context
     * @param BaseTriggerData Base trigger data
     * @return Customized trigger data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Integration Events")
    FMusicTriggerData CustomizeMusicTrigger(EMusicTriggerType TriggerType, const FString& Context, const FMusicTriggerData& BaseTriggerData);

    /**
     * Called to determine combat music intensity (override in Blueprint)
     * @param EnemyCount Number of enemies
     * @param BossLevel Boss level
     * @param PlayerHealth Player health percentage
     * @return Custom combat intensity
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Integration Events")
    float DetermineCombatMusicIntensity(int32 EnemyCount, int32 BossLevel, float PlayerHealth);
};
