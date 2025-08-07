// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/ProceduralPerformanceComponent.h"
#include "Multiplayer/MultiplayerDecisionSyncComponent.h"
#include "CompanionEmoteComponent.generated.h"

/**
 * Companion emote types
 */
UENUM(BlueprintType)
enum class ECompanionEmoteAnimType : uint8
{
    Approval            UMETA(DisplayName = "Approval"),
    Disapproval         UMETA(DisplayName = "Disapproval"),
    Shock               UMETA(DisplayName = "Shock"),
    Amusement           UMETA(DisplayName = "Amusement"),
    Concern             UMETA(DisplayName = "Concern"),
    Relief              UMETA(DisplayName = "Relief"),
    Frustration         UMETA(DisplayName = "Frustration"),
    Pride               UMETA(DisplayName = "Pride"),
    Disappointment      UMETA(DisplayName = "Disappointment"),
    Excitement          UMETA(DisplayName = "Excitement"),
    Nervousness         UMETA(DisplayName = "Nervousness"),
    Confidence          UMETA(DisplayName = "Confidence"),
    Sadness             UMETA(DisplayName = "Sadness"),
    Anger               UMETA(DisplayName = "Anger"),
    Fear                UMETA(DisplayName = "Fear"),
    Love                UMETA(DisplayName = "Love"),
    Jealousy            UMETA(DisplayName = "Jealousy"),
    Curiosity           UMETA(DisplayName = "Curiosity"),
    Boredom             UMETA(DisplayName = "Boredom"),
    Determination       UMETA(DisplayName = "Determination")
};

/**
 * Emote trigger types
 */
UENUM(BlueprintType)
enum class EEmoteTriggerType : uint8
{
    PlayerChoice        UMETA(DisplayName = "Player Choice"),
    DialogueOption      UMETA(DisplayName = "Dialogue Option"),
    QuestDecision       UMETA(DisplayName = "Quest Decision"),
    CombatAction        UMETA(DisplayName = "Combat Action"),
    ItemUse             UMETA(DisplayName = "Item Use"),
    LocationEnter       UMETA(DisplayName = "Location Enter"),
    CharacterMeet       UMETA(DisplayName = "Character Meet"),
    StoryMoment         UMETA(DisplayName = "Story Moment"),
    AlignmentShift      UMETA(DisplayName = "Alignment Shift"),
    CompanionInteract   UMETA(DisplayName = "Companion Interaction"),
    PlayerDeath         UMETA(DisplayName = "Player Death"),
    Victory             UMETA(DisplayName = "Victory"),
    Defeat              UMETA(DisplayName = "Defeat"),
    Discovery           UMETA(DisplayName = "Discovery"),
    Betrayal            UMETA(DisplayName = "Betrayal"),
    Reunion             UMETA(DisplayName = "Reunion")
};

/**
 * Companion emote data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionEmoteData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    FString EmoteID;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    FString CompanionID;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    ECompanionEmoteType EmoteType;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    EEmoteTriggerType TriggerType;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    FString TriggerContext; // Specific context for the trigger

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    UAnimMontage* EmoteAnimation;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    FString EmoteDialogue; // Optional dialogue during emote

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    float EmoteDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    float Priority; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    float Cooldown; // Seconds before emote can trigger again

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    TArray<FString> Prerequisites; // Conditions that must be met

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    bool bInterruptsCurrentAction; // Whether emote interrupts current action

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    bool bRequiresLineOfSight; // Whether companion needs to see player

    UPROPERTY(BlueprintReadWrite, Category = "Companion Emote")
    float LastTriggeredTime;

    FCompanionEmoteData()
    {
        EmoteID = TEXT("");
        CompanionID = TEXT("");
        EmoteType = ECompanionEmoteType::Approval;
        TriggerType = EEmoteTriggerType::PlayerChoice;
        TriggerContext = TEXT("");
        EmoteAnimation = nullptr;
        EmoteDialogue = TEXT("");
        EmoteDuration = 2.0f;
        Priority = 0.5f;
        Cooldown = 10.0f;
        bInterruptsCurrentAction = false;
        bRequiresLineOfSight = true;
        LastTriggeredTime = 0.0f;
    }
};

/**
 * Companion group emote data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionGroupEmote
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Group Emote")
    FString GroupEmoteID;

    UPROPERTY(BlueprintReadWrite, Category = "Group Emote")
    TArray<FString> ParticipantIDs; // Companions involved

    UPROPERTY(BlueprintReadWrite, Category = "Group Emote")
    TArray<FCompanionEmoteData> EmoteSequence; // Sequence of emotes

    UPROPERTY(BlueprintReadWrite, Category = "Group Emote")
    float SequenceDelay; // Delay between emotes

    UPROPERTY(BlueprintReadWrite, Category = "Group Emote")
    bool bSynchronized; // Whether emotes play simultaneously

    FCompanionGroupEmote()
    {
        GroupEmoteID = TEXT("");
        SequenceDelay = 0.5f;
        bSynchronized = false;
    }
};

/**
 * Companion emote events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionEmoteTriggered, const FString&, CompanionID, const FCompanionEmoteData&, EmoteData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroupEmoteStarted, const FCompanionGroupEmote&, GroupEmote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEmoteCompleted, const FString&, CompanionID, const FString&, EmoteID);

/**
 * Companion Emote Component - Let party members physically react to player choices
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCompanionEmoteComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCompanionEmoteComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize companion emote system
     * @param PerformanceComponent Performance component for animations
     * @param DecisionSync Decision sync component for multiplayer
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    void InitializeCompanionEmotes(UProceduralPerformanceComponent* PerformanceComponent, 
                                  UMultiplayerDecisionSyncComponent* DecisionSync);

    /**
     * Trigger companion emote
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @param CompanionID Specific companion (empty = all companions)
     * @return True if any emote was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    bool TriggerCompanionEmote(EEmoteTriggerType TriggerType, const FString& TriggerContext, const FString& CompanionID = TEXT(""));

    /**
     * Add companion emote
     * @param EmoteData Emote data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    void AddCompanionEmote(const FCompanionEmoteData& EmoteData);

    /**
     * Remove companion emote
     * @param EmoteID ID of emote to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    void RemoveCompanionEmote(const FString& EmoteID);

    /**
     * Add group emote
     * @param GroupEmote Group emote data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    void AddGroupEmote(const FCompanionGroupEmote& GroupEmote);

    /**
     * Trigger group emote
     * @param GroupEmoteID ID of group emote to trigger
     * @return True if group emote was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    bool TriggerGroupEmote(const FString& GroupEmoteID);

    /**
     * Set companion active
     * @param CompanionID ID of companion
     * @param bActive Whether companion is active
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    void SetCompanionActive(const FString& CompanionID, bool bActive);

    /**
     * Check if companion is active
     * @param CompanionID ID of companion to check
     * @return True if companion is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Emote")
    bool IsCompanionActive(const FString& CompanionID) const;

    /**
     * Get active companions
     * @return Array of active companion IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Emote")
    TArray<FString> GetActiveCompanions() const;

    /**
     * Set emote enabled for companion
     * @param CompanionID ID of companion
     * @param bEnabled Whether emotes are enabled
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    void SetCompanionEmotesEnabled(const FString& CompanionID, bool bEnabled);

    /**
     * Get emotes for trigger
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @return Array of matching emotes
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Emote")
    TArray<FCompanionEmoteData> GetEmotesForTrigger(EEmoteTriggerType TriggerType, const FString& TriggerContext) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Companion Emote Events")
    FOnCompanionEmoteTriggered OnCompanionEmoteTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Companion Emote Events")
    FOnGroupEmoteStarted OnGroupEmoteStarted;

    UPROPERTY(BlueprintAssignable, Category = "Companion Emote Events")
    FOnEmoteCompleted OnEmoteCompleted;

protected:
    // Companion emotes
    UPROPERTY(BlueprintReadOnly, Category = "Companion Emotes")
    TArray<FCompanionEmoteData> CompanionEmotes;

    // Group emotes
    UPROPERTY(BlueprintReadOnly, Category = "Group Emotes")
    TArray<FCompanionGroupEmote> GroupEmotes;

    // Active companions
    UPROPERTY(BlueprintReadOnly, Category = "Active Companions")
    TArray<FString> ActiveCompanions;

    // Companion emote settings
    UPROPERTY(BlueprintReadOnly, Category = "Companion Settings")
    TMap<FString, bool> CompanionEmoteEnabled;

    // Component references
    UPROPERTY()
    UProceduralPerformanceComponent* PerformanceComponent;

    UPROPERTY()
    UMultiplayerDecisionSyncComponent* DecisionSyncComponent;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emote Settings")
    bool bEmotesEnabled; // Global emote enable/disable

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emote Settings")
    float MaxEmoteDistance; // Max distance for emotes to trigger

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emote Settings")
    bool bRequireLineOfSight; // Global line of sight requirement

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emote Settings")
    float EmoteVolume; // Volume for emote dialogue

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Emote State")
    TMap<FString, float> CompanionLastEmoteTime;

    UPROPERTY(BlueprintReadOnly, Category = "Emote State")
    bool bGroupEmoteInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Emote State")
    FString CurrentGroupEmoteID;

    // Timer handles
    FTimerHandle GroupEmoteTimer;

private:
    // Helper methods
    void LoadDefaultEmotes();
    void LoadDefaultGroupEmotes();
    bool CheckEmotePrerequisites(const FCompanionEmoteData& EmoteData) const;
    bool CanTriggerEmote(const FCompanionEmoteData& EmoteData) const;
    FCompanionEmoteData* FindBestEmote(EEmoteTriggerType TriggerType, const FString& TriggerContext, const FString& CompanionID);
    void PlayCompanionEmote(const FCompanionEmoteData& EmoteData);
    void PlayGroupEmoteSequence(const FCompanionGroupEmote& GroupEmote);
    bool HasLineOfSight(const FString& CompanionID) const;
    float GetDistanceToCompanion(const FString& CompanionID) const;

    // Group emote management
    void StartGroupEmoteSequence(const FCompanionGroupEmote& GroupEmote);
    void PlayNextGroupEmote();
    void CompleteGroupEmoteSequence();

    // Current group emote state
    int32 CurrentGroupEmoteIndex;
    TArray<FCompanionEmoteData> CurrentGroupEmoteSequence;

    // Timer callbacks
    UFUNCTION()
    void OnGroupEmoteTimer();

    // Decision sync events
    UFUNCTION()
    void OnPlayerVoteCast(const FString& PlayerID, const FPlayerVote& Vote);

    UFUNCTION()
    void OnDecisionReached(const FDecisionSession& Session, int32 FinalChoice);

public:
    /**
     * Blueprint implementable events for custom companion emote logic
     */

    /**
     * Called when companion emote is triggered (for custom effects)
     * @param CompanionID ID of emoting companion
     * @param EmoteData The emote data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Emote Events")
    void OnCompanionEmoteTriggeredEvent(const FString& CompanionID, const FCompanionEmoteData& EmoteData);

    /**
     * Called when group emote starts (for custom setup)
     * @param GroupEmote The group emote data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Emote Events")
    void OnGroupEmoteStartedEvent(const FCompanionGroupEmote& GroupEmote);

    /**
     * Called to generate custom emote (override in Blueprint)
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @param CompanionID ID of companion
     * @return Custom emote data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Emote Events")
    FCompanionEmoteData GenerateCustomEmote(EEmoteTriggerType TriggerType, const FString& TriggerContext, const FString& CompanionID);

    /**
     * Called to check custom prerequisites (override in Blueprint)
     * @param EmoteData Emote to check
     * @return True if custom prerequisites are met
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Emote Events")
    bool CheckCustomEmotePrerequisites(const FCompanionEmoteData& EmoteData);

    /**
     * Called to customize emote based on relationship (override in Blueprint)
     * @param BaseEmoteData Base emote data
     * @param CompanionID ID of companion
     * @param RelationshipLevel Current relationship level
     * @return Modified emote data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Emote Events")
    FCompanionEmoteData CustomizeEmoteForRelationship(const FCompanionEmoteData& BaseEmoteData, const FString& CompanionID, float RelationshipLevel);
};
