// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "Cinematics/CinematicCameraSplineActor.h"
#include "UI/DialogueBoxWidget.h"
#include "BossIntroNarrationComponent.generated.h"

/**
 * Boss intro types
 */
UENUM(BlueprintType)
enum class EBossIntroType : uint8
{
    Dramatic            UMETA(DisplayName = "Dramatic Reveal"),
    Menacing            UMETA(DisplayName = "Menacing Approach"),
    Mysterious          UMETA(DisplayName = "Mysterious Entrance"),
    Explosive           UMETA(DisplayName = "Explosive Arrival"),
    Regal               UMETA(DisplayName = "Regal Presentation"),
    Corrupted           UMETA(DisplayName = "Corrupted Transformation"),
    Ancient             UMETA(DisplayName = "Ancient Awakening"),
    Betrayal            UMETA(DisplayName = "Betrayal Reveal")
};

/**
 * Boss reaction types
 */
UENUM(BlueprintType)
enum class EBossReactionType : uint8
{
    PlayerApproach      UMETA(DisplayName = "Player Approach"),
    CombatStart         UMETA(DisplayName = "Combat Start"),
    HealthThreshold     UMETA(DisplayName = "Health Threshold"),
    SpecialAttack       UMETA(DisplayName = "Special Attack"),
    PlayerDeath         UMETA(DisplayName = "Player Death"),
    BossDefeat          UMETA(DisplayName = "Boss Defeat"),
    PhaseTransition     UMETA(DisplayName = "Phase Transition"),
    TauntResponse       UMETA(DisplayName = "Taunt Response")
};

/**
 * Boss intro data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossIntroData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FString BossID;

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FString BossName;

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FString BossTitle; // "Dark Lord of the Sith", "Ancient Guardian", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    EBossIntroType IntroType;

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FString IntroNarration; // Narrator text

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FString BossDialogue; // What the boss says

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FVoiceCharacterData NarratorVoice; // Voice for narrator

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FVoiceCharacterData BossVoice; // Voice for boss

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    float IntroDuration; // Total intro duration

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    TArray<FString> CameraSequences; // Camera movements during intro

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    TArray<FString> VisualEffects; // Special effects during intro

    UPROPERTY(BlueprintReadWrite, Category = "Boss Intro")
    FString MusicTrack; // Music for the intro

    FBossIntroData()
    {
        BossID = TEXT("");
        BossName = TEXT("Unknown Boss");
        BossTitle = TEXT("The Adversary");
        IntroType = EBossIntroType::Dramatic;
        IntroNarration = TEXT("A powerful enemy appears...");
        BossDialogue = TEXT("You dare challenge me?");
        IntroDuration = 10.0f;
        MusicTrack = TEXT("boss_intro_theme");
    }
};

/**
 * Boss reaction data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossReactionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    FString ReactionID;

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    EBossReactionType ReactionType;

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    FString ReactionText; // What the boss says

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    FString NarrationText; // Optional narrator commentary

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    float TriggerThreshold; // For health-based reactions (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    float Cooldown; // Seconds before reaction can trigger again

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    bool bInterruptsCombat; // Whether reaction pauses combat

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    TArray<FString> VisualEffects; // Effects during reaction

    UPROPERTY(BlueprintReadWrite, Category = "Boss Reaction")
    float LastTriggeredTime;

    FBossReactionData()
    {
        ReactionID = TEXT("");
        ReactionType = EBossReactionType::CombatStart;
        ReactionText = TEXT("Impressive...");
        NarrationText = TEXT("");
        TriggerThreshold = 0.5f;
        Cooldown = 30.0f;
        bInterruptsCombat = false;
        LastTriggeredTime = 0.0f;
    }
};

/**
 * Boss intro events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossIntroStarted, const FBossIntroData&, IntroData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossIntroCompleted, const FString&, BossID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossReactionTriggered, const FString&, BossID, const FBossReactionData&, ReactionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossNarrationSpoken, const FString&, SpeakerType, const FString&, Text);

/**
 * Boss Intro Narration Component - Unique voiced boss introductions with dynamic reactions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UBossIntroNarrationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBossIntroNarrationComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize boss intro system
     * @param VoiceComponent Voice synthesis component
     * @param CameraSpline Camera spline for cinematics
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    void InitializeBossIntro(UVoiceSynthesisComponent* VoiceComponent, ACinematicCameraSplineActor* CameraSpline);

    /**
     * Start boss introduction
     * @param BossActor The boss actor
     * @param IntroData Introduction data
     * @return True if intro started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    bool StartBossIntroduction(AActor* BossActor, const FBossIntroData& IntroData);

    /**
     * Trigger boss reaction
     * @param ReactionType Type of reaction to trigger
     * @param Context Additional context for the reaction
     * @return True if reaction was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    bool TriggerBossReaction(EBossReactionType ReactionType, const FString& Context = TEXT(""));

    /**
     * Add boss reaction
     * @param ReactionData Reaction data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    void AddBossReaction(const FBossReactionData& ReactionData);

    /**
     * Remove boss reaction
     * @param ReactionID ID of reaction to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    void RemoveBossReaction(const FString& ReactionID);

    /**
     * Set boss health for health-based reactions
     * @param CurrentHealth Current boss health (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    void SetBossHealth(float CurrentHealth);

    /**
     * Skip current intro/reaction
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    void SkipCurrentSequence();

    /**
     * Check if intro is playing
     * @return True if boss intro is currently playing
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Intro")
    bool IsIntroPlaying() const { return bIntroInProgress; }

    /**
     * Get current boss data
     * @return Current boss intro data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Intro")
    FBossIntroData GetCurrentBossData() const { return CurrentBossData; }

    /**
     * Generate dynamic boss intro
     * @param BossActor The boss actor
     * @param PlayerLevel Player's current level
     * @param PlayerAlignment Player's alignment
     * @return Generated boss intro data
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro")
    FBossIntroData GenerateDynamicBossIntro(AActor* BossActor, int32 PlayerLevel, float PlayerAlignment);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossIntroStarted OnBossIntroStarted;

    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossIntroCompleted OnBossIntroCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossReactionTriggered OnBossReactionTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossNarrationSpoken OnBossNarrationSpoken;

protected:
    // Current boss data
    UPROPERTY(BlueprintReadOnly, Category = "Boss Data")
    FBossIntroData CurrentBossData;

    UPROPERTY(BlueprintReadOnly, Category = "Boss Data")
    AActor* CurrentBossActor;

    // Boss reactions
    UPROPERTY(BlueprintReadOnly, Category = "Boss Reactions")
    TArray<FBossReactionData> BossReactions;

    // Component references
    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    UPROPERTY()
    ACinematicCameraSplineActor* CameraSplineActor;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIntroInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bReactionInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    float CurrentBossHealth;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    float IntroStartTime;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Settings")
    bool bAllowSkipping; // Whether intros can be skipped

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Settings")
    float DefaultIntroDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Settings")
    float NarrationVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Settings")
    bool bPauseCombatDuringIntro;

    // Timer handles
    FTimerHandle IntroSequenceTimer;
    FTimerHandle ReactionTimer;

private:
    // Helper methods
    void LoadDefaultBossIntros();
    void LoadDefaultBossReactions();
    void PlayIntroSequence();
    void PlayNarration(const FString& Text, const FVoiceCharacterData& VoiceData, const FString& SpeakerType);
    void PlayBossDialogue(const FString& Text, const FVoiceCharacterData& VoiceData);
    void StartCameraSequence(const TArray<FString>& CameraSequences);
    void ApplyVisualEffects(const TArray<FString>& Effects);
    void CompleteIntroSequence();

    // Reaction system
    FBossReactionData* FindReactionForType(EBossReactionType ReactionType);
    bool CanTriggerReaction(const FBossReactionData& Reaction) const;
    void PlayReactionSequence(const FBossReactionData& Reaction);

    // Dynamic generation
    FString GenerateBossNarration(AActor* BossActor, int32 PlayerLevel, float PlayerAlignment);
    FString GenerateBossDialogue(AActor* BossActor, int32 PlayerLevel, float PlayerAlignment);
    EBossIntroType DetermineBossIntroType(AActor* BossActor);

    // Timer callbacks
    UFUNCTION()
    void OnIntroSequenceTimer();

    UFUNCTION()
    void OnReactionTimer();

public:
    /**
     * Blueprint implementable events for custom boss intro logic
     */

    /**
     * Called when boss intro starts (for custom setup)
     * @param IntroData The boss intro data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    void OnBossIntroStartedEvent(const FBossIntroData& IntroData);

    /**
     * Called when boss reaction is triggered (for custom effects)
     * @param BossID ID of the boss
     * @param ReactionData The reaction data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    void OnBossReactionTriggeredEvent(const FString& BossID, const FBossReactionData& ReactionData);

    /**
     * Called to customize boss intro (override in Blueprint)
     * @param BossActor The boss actor
     * @param BaseIntroData Base intro data
     * @return Customized intro data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    FBossIntroData CustomizeBossIntro(AActor* BossActor, const FBossIntroData& BaseIntroData);

    /**
     * Called to generate custom boss reaction (override in Blueprint)
     * @param ReactionType Type of reaction
     * @param BossActor The boss actor
     * @param Context Reaction context
     * @return Custom reaction data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    FBossReactionData GenerateCustomBossReaction(EBossReactionType ReactionType, AActor* BossActor, const FString& Context);

    /**
     * Called to validate boss reaction trigger (override in Blueprint)
     * @param ReactionData Reaction to validate
     * @param CurrentContext Current game context
     * @return True if reaction should trigger
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    bool ValidateBossReactionTrigger(const FBossReactionData& ReactionData, const FString& CurrentContext);
};
