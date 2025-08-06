// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "CompanionVoiceReactionComponent.generated.h"

/**
 * Reaction trigger types
 */
UENUM(BlueprintType)
enum class EReactionTrigger : uint8
{
    PlayerAction        UMETA(DisplayName = "Player Action"),
    DialogueChoice      UMETA(DisplayName = "Dialogue Choice"),
    QuestEvent          UMETA(DisplayName = "Quest Event"),
    CombatEvent         UMETA(DisplayName = "Combat Event"),
    LocationEnter       UMETA(DisplayName = "Location Enter"),
    ItemPickup          UMETA(DisplayName = "Item Pickup"),
    AlignmentShift      UMETA(DisplayName = "Alignment Shift"),
    CompanionInteract   UMETA(DisplayName = "Companion Interaction"),
    AmbientTrigger      UMETA(DisplayName = "Ambient Trigger"),
    StoryMoment         UMETA(DisplayName = "Story Moment")
};

/**
 * Reaction emotion types
 */
UENUM(BlueprintType)
enum class EReactionEmotion : uint8
{
    Approval            UMETA(DisplayName = "Approval"),
    Disapproval         UMETA(DisplayName = "Disapproval"),
    Surprise            UMETA(DisplayName = "Surprise"),
    Concern             UMETA(DisplayName = "Concern"),
    Amusement           UMETA(DisplayName = "Amusement"),
    Sadness             UMETA(DisplayName = "Sadness"),
    Anger               UMETA(DisplayName = "Anger"),
    Fear                UMETA(DisplayName = "Fear"),
    Curiosity           UMETA(DisplayName = "Curiosity"),
    Neutral             UMETA(DisplayName = "Neutral")
};

/**
 * Voice reaction data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionVoiceReaction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    FString ReactionID;

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    FString CompanionID;

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    EReactionTrigger TriggerType;

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    FString TriggerContext; // Specific context for the trigger

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    EReactionEmotion Emotion;

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    FString ReactionText;

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    float Priority; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    float Cooldown; // Seconds before this reaction can trigger again

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    TArray<FString> Prerequisites; // Conditions that must be met

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    bool bIsAmbient; // Whether this is ambient banter

    UPROPERTY(BlueprintReadWrite, Category = "Voice Reaction")
    float LastTriggeredTime;

    FCompanionVoiceReaction()
    {
        ReactionID = TEXT("");
        CompanionID = TEXT("");
        TriggerType = EReactionTrigger::PlayerAction;
        TriggerContext = TEXT("");
        Emotion = EReactionEmotion::Neutral;
        ReactionText = TEXT("");
        Priority = 0.5f;
        Cooldown = 30.0f;
        bIsAmbient = false;
        LastTriggeredTime = 0.0f;
    }
};

/**
 * Companion banter data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionBanter
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    FString BanterID;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    TArray<FString> ParticipantIDs; // Companions involved in banter

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    TArray<FString> BanterLines; // Lines of dialogue

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    TArray<FString> SpeakerOrder; // Order of speakers

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    FString TriggerLocation; // Location where banter can trigger

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    TArray<FString> Prerequisites; // Story/quest prerequisites

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    float TriggerChance; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Companion Banter")
    bool bHasTriggered; // Whether this banter has already played

    FCompanionBanter()
    {
        BanterID = TEXT("");
        TriggerLocation = TEXT("");
        TriggerChance = 0.3f;
        bHasTriggered = false;
    }
};

/**
 * Voice reaction events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionReaction, const FString&, CompanionID, const FCompanionVoiceReaction&, Reaction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompanionBanterStarted, const FCompanionBanter&, Banter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompanionBanterCompleted, const FString&, BanterID);

/**
 * Companion Voice Reaction Component - Handles ambient and triggered companion banter
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCompanionVoiceReactionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCompanionVoiceReactionComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize companion voice reactions
     * @param VoiceComponent Voice synthesis component to use
     * @param QuestManager Quest manager for context
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    void InitializeVoiceReactions(UVoiceSynthesisComponent* VoiceComponent, UQuestManagerComponent* QuestManager);

    /**
     * Trigger companion reaction
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @param CompanionID Specific companion (empty = all companions)
     * @return True if any reaction was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    bool TriggerCompanionReaction(EReactionTrigger TriggerType, const FString& TriggerContext, const FString& CompanionID = TEXT(""));

    /**
     * Add companion reaction
     * @param Reaction Reaction data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    void AddCompanionReaction(const FCompanionVoiceReaction& Reaction);

    /**
     * Remove companion reaction
     * @param ReactionID ID of reaction to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    void RemoveCompanionReaction(const FString& ReactionID);

    /**
     * Add companion banter
     * @param Banter Banter data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    void AddCompanionBanter(const FCompanionBanter& Banter);

    /**
     * Trigger companion banter
     * @param BanterID Specific banter to trigger (empty = random eligible)
     * @param bForcePlay Force play even if prerequisites not met
     * @return True if banter was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    bool TriggerCompanionBanter(const FString& BanterID = TEXT(""), bool bForcePlay = false);

    /**
     * Set companion active
     * @param CompanionID ID of companion
     * @param bActive Whether companion is active
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    void SetCompanionActive(const FString& CompanionID, bool bActive);

    /**
     * Check if companion is active
     * @param CompanionID ID of companion to check
     * @return True if companion is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Voice")
    bool IsCompanionActive(const FString& CompanionID) const;

    /**
     * Get active companions
     * @return Array of active companion IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Voice")
    TArray<FString> GetActiveCompanions() const;

    /**
     * Set ambient banter enabled
     * @param bEnabled Whether ambient banter should be enabled
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    void SetAmbientBanterEnabled(bool bEnabled);

    /**
     * Get companion reactions for trigger
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @return Array of matching reactions
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Voice")
    TArray<FCompanionVoiceReaction> GetReactionsForTrigger(EReactionTrigger TriggerType, const FString& TriggerContext) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Companion Voice Events")
    FOnCompanionReaction OnCompanionReaction;

    UPROPERTY(BlueprintAssignable, Category = "Companion Voice Events")
    FOnCompanionBanterStarted OnCompanionBanterStarted;

    UPROPERTY(BlueprintAssignable, Category = "Companion Voice Events")
    FOnCompanionBanterCompleted OnCompanionBanterCompleted;

protected:
    // Voice reactions
    UPROPERTY(BlueprintReadOnly, Category = "Voice Reactions")
    TArray<FCompanionVoiceReaction> CompanionReactions;

    // Companion banter
    UPROPERTY(BlueprintReadOnly, Category = "Companion Banter")
    TArray<FCompanionBanter> CompanionBanters;

    // Active companions
    UPROPERTY(BlueprintReadOnly, Category = "Active Companions")
    TArray<FString> ActiveCompanions;

    // Component references
    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerComponent;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Settings")
    bool bAmbientBanterEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Settings")
    float AmbientBanterInterval; // Seconds between ambient banter checks

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Settings")
    float AmbientBanterChance; // Base chance for ambient banter

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Settings")
    float ReactionVolume; // Volume for companion reactions

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Settings")
    float MaxReactionDistance; // Max distance for reactions to trigger

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "State")
    float LastAmbientBanterTime;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bBanterInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    FString CurrentBanterID;

    // Timer handles
    FTimerHandle AmbientBanterTimer;
    FTimerHandle BanterSequenceTimer;

private:
    // Helper methods
    void LoadDefaultReactions();
    void LoadDefaultBanters();
    bool CheckReactionPrerequisites(const FCompanionVoiceReaction& Reaction) const;
    bool CheckBanterPrerequisites(const FCompanionBanter& Banter) const;
    FCompanionVoiceReaction* FindBestReaction(EReactionTrigger TriggerType, const FString& TriggerContext, const FString& CompanionID);
    void PlayCompanionReaction(const FCompanionVoiceReaction& Reaction);
    void PlayCompanionBanter(const FCompanionBanter& Banter);

    // Ambient banter
    void CheckAmbientBanter();
    FCompanionBanter* FindEligibleBanter();

    // Banter sequence management
    void StartBanterSequence(const FCompanionBanter& Banter);
    void PlayNextBanterLine();
    void CompleteBanterSequence();

    // Current banter state
    int32 CurrentBanterLineIndex;
    TArray<FString> CurrentBanterLines;
    TArray<FString> CurrentBanterSpeakers;

    // Timer callbacks
    UFUNCTION()
    void OnAmbientBanterTimer();

    UFUNCTION()
    void OnBanterSequenceTimer();

public:
    /**
     * Blueprint implementable events for custom companion logic
     */

    /**
     * Called when companion reaction is triggered (for custom effects)
     * @param CompanionID ID of reacting companion
     * @param Reaction The reaction data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Voice Events")
    void OnCompanionReactionEvent(const FString& CompanionID, const FCompanionVoiceReaction& Reaction);

    /**
     * Called when companion banter starts (for custom setup)
     * @param Banter The banter data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Voice Events")
    void OnCompanionBanterStartedEvent(const FCompanionBanter& Banter);

    /**
     * Called to generate custom reaction (override in Blueprint)
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @param CompanionID ID of companion
     * @return Custom reaction data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Voice Events")
    FCompanionVoiceReaction GenerateCustomReaction(EReactionTrigger TriggerType, const FString& TriggerContext, const FString& CompanionID);

    /**
     * Called to check custom prerequisites (override in Blueprint)
     * @param Reaction Reaction to check
     * @return True if custom prerequisites are met
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Voice Events")
    bool CheckCustomReactionPrerequisites(const FCompanionVoiceReaction& Reaction);

    /**
     * Called to customize reaction emotion (override in Blueprint)
     * @param BaseReaction Base reaction data
     * @param CurrentContext Current game context
     * @return Modified reaction with updated emotion
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Voice Events")
    FCompanionVoiceReaction CustomizeReactionEmotion(const FCompanionVoiceReaction& BaseReaction, const FString& CurrentContext);
};
