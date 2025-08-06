// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Animation/VOPerformanceIntegrationComponent.h"
#include "CompanionReactionSystem.generated.h"

/**
 * Companion reaction trigger types
 */
UENUM(BlueprintType)
enum class ECompanionReactionTrigger : uint8
{
    PlayerChoice        UMETA(DisplayName = "Player Choice"),
    DialogueOption      UMETA(DisplayName = "Dialogue Option"),
    MoralityShift       UMETA(DisplayName = "Morality Shift"),
    CombatAction        UMETA(DisplayName = "Combat Action"),
    QuestDecision       UMETA(DisplayName = "Quest Decision"),
    ItemUse             UMETA(DisplayName = "Item Use"),
    LocationEnter       UMETA(DisplayName = "Location Enter"),
    CharacterMeet       UMETA(DisplayName = "Character Meet"),
    StoryMoment         UMETA(DisplayName = "Story Moment"),
    PlayerDeath         UMETA(DisplayName = "Player Death"),
    Victory             UMETA(DisplayName = "Victory"),
    Defeat              UMETA(DisplayName = "Defeat"),
    Discovery           UMETA(DisplayName = "Discovery"),
    Betrayal            UMETA(DisplayName = "Betrayal"),
    Romance             UMETA(DisplayName = "Romance"),
    Sacrifice           UMETA(DisplayName = "Sacrifice")
};

/**
 * Companion emote types
 */
UENUM(BlueprintType)
enum class ECompanionEmoteType : uint8
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
 * Companion reaction data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionReactionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    FString ReactionID;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    FString CompanionID;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    ECompanionReactionTrigger TriggerType;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    ECompanionEmoteType EmoteType;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    FString TriggerContext; // Specific context for the trigger

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    FString ReactionDialogue; // What the companion says

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    EPerformanceEmotion ReactionEmotion; // Animation emotion

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    EPerformanceTone ReactionTone; // Animation tone

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    float MoralityThreshold; // Morality threshold for trigger (-1.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    float RelationshipThreshold; // Relationship threshold (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    float Priority; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    float Cooldown; // Seconds before reaction can trigger again

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    TArray<FString> Prerequisites; // Conditions that must be met

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    bool bInterruptsCurrentAction; // Whether reaction interrupts current action

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    bool bRequiresLineOfSight; // Whether companion needs to see player

    UPROPERTY(BlueprintReadWrite, Category = "Companion Reaction")
    float LastTriggeredTime;

    FCompanionReactionData()
    {
        ReactionID = TEXT("");
        CompanionID = TEXT("");
        TriggerType = ECompanionReactionTrigger::PlayerChoice;
        EmoteType = ECompanionEmoteType::Approval;
        TriggerContext = TEXT("");
        ReactionDialogue = TEXT("");
        ReactionEmotion = EPerformanceEmotion::Neutral;
        ReactionTone = EPerformanceTone::Normal;
        MoralityThreshold = 0.0f;
        RelationshipThreshold = 0.0f;
        Priority = 0.5f;
        Cooldown = 10.0f;
        bInterruptsCurrentAction = false;
        bRequiresLineOfSight = true;
        LastTriggeredTime = 0.0f;
    }
};

/**
 * Companion data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    FString CompanionID;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    FString CompanionName;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    AActor* CompanionActor;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    float RelationshipLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    float MoralityAlignment; // -1.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    bool bIsActive; // Whether companion is in party

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    bool bReactionsEnabled; // Whether reactions are enabled

    UPROPERTY(BlueprintReadWrite, Category = "Companion Data")
    TArray<FString> CompanionTags; // Tags for this companion

    FCompanionData()
    {
        CompanionID = TEXT("");
        CompanionName = TEXT("");
        CompanionActor = nullptr;
        RelationshipLevel = 0.5f;
        MoralityAlignment = 0.0f;
        bIsActive = false;
        bReactionsEnabled = true;
    }
};

/**
 * Companion reaction events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionReactionTriggered, const FString&, CompanionID, const FCompanionReactionData&, ReactionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionRelationshipChanged, const FString&, CompanionID, float, NewRelationshipLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionMoralityChanged, const FString&, CompanionID, float, NewMorality);

/**
 * Companion Reaction System - Companion emote system triggered by events and morality
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCompanionReactionSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UCompanionReactionSystem();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize companion reaction system
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void InitializeCompanionReactionSystem();

    /**
     * Add companion
     * @param CompanionData Companion data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void AddCompanion(const FCompanionData& CompanionData);

    /**
     * Remove companion
     * @param CompanionID ID of companion to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void RemoveCompanion(const FString& CompanionID);

    /**
     * Trigger companion reaction
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @param PlayerMorality Current player morality (-1.0 to 1.0)
     * @param CompanionID Specific companion (empty = all companions)
     * @return True if any reaction was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    bool TriggerCompanionReaction(ECompanionReactionTrigger TriggerType, const FString& TriggerContext, 
                                 float PlayerMorality, const FString& CompanionID = TEXT(""));

    /**
     * Add companion reaction
     * @param ReactionData Reaction data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void AddCompanionReaction(const FCompanionReactionData& ReactionData);

    /**
     * Remove companion reaction
     * @param ReactionID ID of reaction to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void RemoveCompanionReaction(const FString& ReactionID);

    /**
     * Set companion active
     * @param CompanionID ID of companion
     * @param bActive Whether companion is active
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void SetCompanionActive(const FString& CompanionID, bool bActive);

    /**
     * Set companion relationship level
     * @param CompanionID ID of companion
     * @param RelationshipLevel New relationship level (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void SetCompanionRelationshipLevel(const FString& CompanionID, float RelationshipLevel);

    /**
     * Set companion morality alignment
     * @param CompanionID ID of companion
     * @param MoralityAlignment New morality alignment (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void SetCompanionMoralityAlignment(const FString& CompanionID, float MoralityAlignment);

    /**
     * Get companion data
     * @param CompanionID ID of companion
     * @return Companion data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Reaction System")
    FCompanionData GetCompanionData(const FString& CompanionID) const;

    /**
     * Get active companions
     * @return Array of active companion IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Reaction System")
    TArray<FString> GetActiveCompanions() const;

    /**
     * Check if companion is active
     * @param CompanionID ID of companion to check
     * @return True if companion is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Reaction System")
    bool IsCompanionActive(const FString& CompanionID) const;

    /**
     * Set reactions enabled for companion
     * @param CompanionID ID of companion
     * @param bEnabled Whether reactions are enabled
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Reaction System")
    void SetCompanionReactionsEnabled(const FString& CompanionID, bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Companion Reaction Events")
    FOnCompanionReactionTriggered OnCompanionReactionTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Companion Reaction Events")
    FOnCompanionRelationshipChanged OnCompanionRelationshipChanged;

    UPROPERTY(BlueprintAssignable, Category = "Companion Reaction Events")
    FOnCompanionMoralityChanged OnCompanionMoralityChanged;

protected:
    // Companions
    UPROPERTY(BlueprintReadOnly, Category = "Companions")
    TMap<FString, FCompanionData> Companions;

    // Companion reactions
    UPROPERTY(BlueprintReadOnly, Category = "Companion Reactions")
    TArray<FCompanionReactionData> CompanionReactions;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Settings")
    bool bReactionsEnabled; // Global reactions enable/disable

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Settings")
    float MaxReactionDistance; // Max distance for reactions to trigger

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Settings")
    bool bRequireLineOfSight; // Global line of sight requirement

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reaction Settings")
    float ReactionVolume; // Volume for reaction dialogue

private:
    // Helper methods
    void LoadDefaultReactions();
    void LoadDefaultCompanions();
    bool CheckReactionPrerequisites(const FCompanionReactionData& ReactionData, const FString& CompanionID) const;
    bool CanTriggerReaction(const FCompanionReactionData& ReactionData, const FString& CompanionID, float PlayerMorality) const;
    FCompanionReactionData* FindBestReaction(ECompanionReactionTrigger TriggerType, const FString& TriggerContext, 
                                           const FString& CompanionID, float PlayerMorality);
    void PlayCompanionReaction(const FCompanionReactionData& ReactionData, const FString& CompanionID);
    bool HasLineOfSight(const FString& CompanionID) const;
    float GetDistanceToCompanion(const FString& CompanionID) const;
    UProceduralPerformanceComponentV2* GetCompanionPerformanceComponent(const FString& CompanionID) const;
    UVOPerformanceIntegrationComponent* GetCompanionVOComponent(const FString& CompanionID) const;

public:
    /**
     * Blueprint implementable events for custom companion reaction logic
     */

    /**
     * Called when companion reaction is triggered (for custom effects)
     * @param CompanionID ID of reacting companion
     * @param ReactionData The reaction data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Reaction Events")
    void OnCompanionReactionTriggeredEvent(const FString& CompanionID, const FCompanionReactionData& ReactionData);

    /**
     * Called when companion relationship changes (for custom processing)
     * @param CompanionID ID of companion
     * @param OldLevel Previous relationship level
     * @param NewLevel New relationship level
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Reaction Events")
    void OnCompanionRelationshipChangedEvent(const FString& CompanionID, float OldLevel, float NewLevel);

    /**
     * Called to generate custom reaction (override in Blueprint)
     * @param TriggerType Type of trigger
     * @param TriggerContext Specific context
     * @param CompanionID ID of companion
     * @param PlayerMorality Player morality
     * @return Custom reaction data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Reaction Events")
    FCompanionReactionData GenerateCustomReaction(ECompanionReactionTrigger TriggerType, const FString& TriggerContext, 
                                                 const FString& CompanionID, float PlayerMorality);

    /**
     * Called to check custom prerequisites (override in Blueprint)
     * @param ReactionData Reaction to check
     * @param CompanionID ID of companion
     * @return True if custom prerequisites are met
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Reaction Events")
    bool CheckCustomReactionPrerequisites(const FCompanionReactionData& ReactionData, const FString& CompanionID);
};
