// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "CompanionBanterEngine.generated.h"

/**
 * Banter types
 */
UENUM(BlueprintType)
enum class EBanterType : uint8
{
    Idle            UMETA(DisplayName = "Idle Chatter"),
    Conflict        UMETA(DisplayName = "Conflict"),
    Bonding         UMETA(DisplayName = "Bonding"),
    Philosophical   UMETA(DisplayName = "Philosophical"),
    Humorous        UMETA(DisplayName = "Humorous"),
    Romantic        UMETA(DisplayName = "Romantic"),
    Reminiscence    UMETA(DisplayName = "Reminiscence"),
    Planning        UMETA(DisplayName = "Planning"),
    Observation     UMETA(DisplayName = "Observation"),
    Concern         UMETA(DisplayName = "Concern")
};

/**
 * Banter trigger conditions
 */
UENUM(BlueprintType)
enum class EBanterTrigger : uint8
{
    TimeInterval    UMETA(DisplayName = "Time Interval"),
    LocationChange  UMETA(DisplayName = "Location Change"),
    QuestEvent      UMETA(DisplayName = "Quest Event"),
    CombatEnd       UMETA(DisplayName = "Combat End"),
    MoralChoice     UMETA(DisplayName = "Moral Choice"),
    LoyaltyChange   UMETA(DisplayName = "Loyalty Change"),
    PlayerAction    UMETA(DisplayName = "Player Action"),
    Environmental   UMETA(DisplayName = "Environmental")
};

/**
 * Companion relationship data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionRelationship
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    FString CompanionA;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    FString CompanionB;

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    float RelationshipScore; // -1.0 (hate) to 1.0 (love)

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    FString RelationshipType; // "rivalry", "friendship", "romance", "neutral"

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    TArray<FString> SharedHistory; // Events that shaped their relationship

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    TMap<FString, int32> ConflictTopics; // Topic -> tension level

    UPROPERTY(BlueprintReadWrite, Category = "Companion Relationship")
    TMap<FString, int32> BondingTopics; // Topic -> bonding level

    FCompanionRelationship()
    {
        CompanionA = TEXT("");
        CompanionB = TEXT("");
        RelationshipScore = 0.0f;
        RelationshipType = TEXT("neutral");
    }
};

/**
 * Banter conversation data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBanterConversation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    FString ConversationID;

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    EBanterType BanterType;

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    TArray<FString> Participants; // Companion names

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    TArray<FString> DialogueLines; // Generated dialogue

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    TArray<FString> Speakers; // Who speaks each line

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    FString TriggerContext; // What triggered this banter

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    float Duration; // Expected duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    bool bIsPlaying;

    UPROPERTY(BlueprintReadWrite, Category = "Banter Conversation")
    int32 CurrentLineIndex;

    FBanterConversation()
    {
        ConversationID = TEXT("");
        BanterType = EBanterType::Idle;
        TriggerContext = TEXT("");
        Duration = 30.0f;
        bIsPlaying = false;
        CurrentLineIndex = 0;
    }
};

/**
 * Banter template for generation
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBanterTemplate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    FString TemplateID;

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    EBanterType BanterType;

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    TArray<FString> RequiredCompanions; // Specific companions needed

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    TArray<FString> ConflictingCompanions; // Companions that can't be together

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    FString PromptTemplate; // LLM prompt template

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    TMap<FString, FString> ContextVariables; // Variables for prompt

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    float TriggerWeight; // Probability of this template being selected

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    int32 MinLoyalty; // Minimum loyalty required

    UPROPERTY(BlueprintReadWrite, Category = "Banter Template")
    int32 MaxLoyalty; // Maximum loyalty allowed

    FBanterTemplate()
    {
        TemplateID = TEXT("");
        BanterType = EBanterType::Idle;
        PromptTemplate = TEXT("");
        TriggerWeight = 1.0f;
        MinLoyalty = 0;
        MaxLoyalty = 100;
    }
};

/**
 * Banter events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBanterStarted, const FBanterConversation&, Conversation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBanterCompleted, const FBanterConversation&, Conversation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBanterLineSpoken, const FString&, Speaker, const FString&, Line);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRelationshipChanged, const FString&, CompanionA, const FString&, CompanionB);

/**
 * Companion Banter Engine - Generates dynamic companion conversations
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCompanionBanterEngine : public UActorComponent
{
    GENERATED_BODY()

public:
    UCompanionBanterEngine();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize banter engine
     * @param CompanionManager Companion manager for party data
     * @param NarrativeMemory Narrative memory for context
     * @param VoiceSynthesis Voice synthesis for spoken dialogue
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void InitializeBanterEngine(UCompanionManagerComponent* CompanionManager,
                               UNarrativeMemoryComponent* NarrativeMemory,
                               UVoiceSynthesisComponent* VoiceSynthesis);

    /**
     * Trigger banter conversation
     * @param TriggerType What triggered the banter
     * @param Context Additional context for the banter
     * @return Generated conversation ID
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    FString TriggerBanter(EBanterTrigger TriggerType, const FString& Context = TEXT(""));

    /**
     * Generate banter conversation
     * @param BanterType Type of banter to generate
     * @param Participants Companions involved in the conversation
     * @param Context Contextual information
     * @return Generated banter conversation
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    FBanterConversation GenerateBanterConversation(EBanterType BanterType, 
                                                  const TArray<FString>& Participants, 
                                                  const FString& Context);

    /**
     * Start playing banter conversation
     * @param ConversationID ID of conversation to play
     * @return True if conversation started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    bool StartBanterConversation(const FString& ConversationID);

    /**
     * Stop current banter conversation
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void StopBanterConversation();

    /**
     * Update companion relationship
     * @param CompanionA First companion
     * @param CompanionB Second companion
     * @param RelationshipChange Change in relationship (-1.0 to 1.0)
     * @param Reason Reason for the change
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void UpdateCompanionRelationship(const FString& CompanionA, const FString& CompanionB, 
                                    float RelationshipChange, const FString& Reason);

    /**
     * Get companion relationship
     * @param CompanionA First companion
     * @param CompanionB Second companion
     * @return Relationship data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Banter")
    FCompanionRelationship GetCompanionRelationship(const FString& CompanionA, const FString& CompanionB) const;

    /**
     * Add banter template
     * @param Template Banter template to add
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void AddBanterTemplate(const FBanterTemplate& Template);

    /**
     * Remove banter template
     * @param TemplateID ID of template to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void RemoveBanterTemplate(const FString& TemplateID);

    /**
     * Set banter frequency
     * @param Frequency How often banter occurs (seconds between attempts)
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void SetBanterFrequency(float Frequency);

    /**
     * Enable/disable automatic banter
     * @param bEnabled Whether to enable automatic banter
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Banter")
    void SetAutomaticBanterEnabled(bool bEnabled);

    /**
     * Get current banter conversation
     * @return Current active conversation
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Banter")
    FBanterConversation GetCurrentBanterConversation() const { return CurrentConversation; }

    /**
     * Check if banter is playing
     * @return True if banter conversation is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Banter")
    bool IsBanterPlaying() const { return CurrentConversation.bIsPlaying; }

    /**
     * Get all companion relationships
     * @return Array of all companion relationships
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Banter")
    TArray<FCompanionRelationship> GetAllCompanionRelationships() const { return CompanionRelationships; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Banter Events")
    FOnBanterStarted OnBanterStarted;

    UPROPERTY(BlueprintAssignable, Category = "Banter Events")
    FOnBanterCompleted OnBanterCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Banter Events")
    FOnBanterLineSpoken OnBanterLineSpoken;

    UPROPERTY(BlueprintAssignable, Category = "Banter Events")
    FOnRelationshipChanged OnRelationshipChanged;

protected:
    // Banter data
    UPROPERTY(BlueprintReadOnly, Category = "Companion Banter")
    TArray<FBanterTemplate> BanterTemplates;

    UPROPERTY(BlueprintReadOnly, Category = "Companion Banter")
    TArray<FCompanionRelationship> CompanionRelationships;

    UPROPERTY(BlueprintReadOnly, Category = "Companion Banter")
    FBanterConversation CurrentConversation;

    // Component references
    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisRef;

    // Banter settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Banter Settings")
    bool bAutomaticBanterEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Banter Settings")
    float BanterFrequency; // Seconds between banter attempts

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Banter Settings")
    float BanterChance; // 0.0 to 1.0 chance of banter triggering

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Banter Settings")
    float LinePacing; // Seconds between dialogue lines

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Banter Settings")
    int32 MaxBanterLength; // Maximum lines per conversation

    // Tracking data
    UPROPERTY()
    float LastBanterTime;

    UPROPERTY()
    TMap<FString, float> LastBanterByType; // BanterType -> LastTime

    // Timer handles
    FTimerHandle BanterTimer;
    FTimerHandle LineTimer;

private:
    // Helper methods
    void LoadDefaultBanterTemplates();
    void InitializeCompanionRelationships();
    FBanterTemplate SelectBanterTemplate(EBanterType BanterType, const TArray<FString>& Participants);
    TArray<FString> GetAvailableParticipants();
    FString GenerateBanterDialogue(const FBanterTemplate& Template, const TArray<FString>& Participants, const FString& Context);
    void PlayNextBanterLine();
    void CompleteBanterConversation();
    FCompanionRelationship* FindRelationship(const FString& CompanionA, const FString& CompanionB);
    FString GenerateConversationID();
    bool ShouldTriggerBanter(EBanterTrigger TriggerType);

    // Event handlers
    UFUNCTION()
    void OnCompanionLoyaltyChanged(const FActiveCompanion& Companion);

    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

public:
    /**
     * Blueprint implementable events for custom banter logic
     */

    /**
     * Called when banter starts (for custom effects)
     * @param Conversation The starting conversation
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Banter Events")
    void OnBanterStartedEvent(const FBanterConversation& Conversation);

    /**
     * Called when banter completes (for custom effects)
     * @param Conversation The completed conversation
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Banter Events")
    void OnBanterCompletedEvent(const FBanterConversation& Conversation);

    /**
     * Called to generate custom banter dialogue (override in Blueprint)
     * @param Template The banter template
     * @param Participants Companions in the conversation
     * @param Context Contextual information
     * @return Custom generated dialogue
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Banter Events")
    TArray<FString> GenerateCustomBanterDialogue(const FBanterTemplate& Template, 
                                                const TArray<FString>& Participants, 
                                                const FString& Context);

    /**
     * Called when relationship changes (for custom effects)
     * @param CompanionA First companion
     * @param CompanionB Second companion
     * @param OldScore Previous relationship score
     * @param NewScore New relationship score
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Banter Events")
    void OnRelationshipChangedEvent(const FString& CompanionA, const FString& CompanionB, 
                                   float OldScore, float NewScore);
};
