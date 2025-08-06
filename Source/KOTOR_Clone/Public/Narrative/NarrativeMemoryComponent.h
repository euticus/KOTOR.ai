// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/QuestManagerComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "NarrativeMemoryComponent.generated.h"

/**
 * Memory event types
 */
UENUM(BlueprintType)
enum class EMemoryEventType : uint8
{
    MoralChoice     UMETA(DisplayName = "Moral Choice"),
    QuestDecision   UMETA(DisplayName = "Quest Decision"),
    Combat          UMETA(DisplayName = "Combat"),
    Dialogue        UMETA(DisplayName = "Dialogue"),
    Exploration     UMETA(DisplayName = "Exploration"),
    Companion       UMETA(DisplayName = "Companion Interaction"),
    Story           UMETA(DisplayName = "Story Event"),
    Custom          UMETA(DisplayName = "Custom Event")
};

/**
 * Memory importance levels
 */
UENUM(BlueprintType)
enum class EMemoryImportance : uint8
{
    Trivial         UMETA(DisplayName = "Trivial"),
    Minor           UMETA(DisplayName = "Minor"),
    Moderate        UMETA(DisplayName = "Moderate"),
    Important       UMETA(DisplayName = "Important"),
    Critical        UMETA(DisplayName = "Critical"),
    Legendary       UMETA(DisplayName = "Legendary")
};

/**
 * Narrative memory entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNarrativeMemory
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    FString MemoryID;

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    EMemoryEventType EventType;

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    EMemoryImportance Importance;

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    FString Title;

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    FString Location; // Planet/Layout where event occurred

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    TArray<FString> ParticipantNPCs; // NPCs involved in the event

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    TArray<FString> Tags; // Searchable tags

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    TMap<FString, FString> ContextData; // Additional context information

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    float AlignmentImpact; // -1.0 (dark) to 1.0 (light)

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    float EmotionalWeight; // How emotionally significant this was

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    float Timestamp; // When the event occurred

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    bool bIsPublic; // Whether other NPCs know about this event

    UPROPERTY(BlueprintReadWrite, Category = "Memory")
    TArray<FString> Consequences; // What happened as a result

    FNarrativeMemory()
    {
        MemoryID = TEXT("");
        EventType = EMemoryEventType::Custom;
        Importance = EMemoryImportance::Minor;
        Title = TEXT("");
        Description = TEXT("");
        Location = TEXT("");
        AlignmentImpact = 0.0f;
        EmotionalWeight = 0.0f;
        Timestamp = 0.0f;
        bIsPublic = false;
    }
};

/**
 * Context injection data for AI prompts
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNarrativeContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    TArray<FNarrativeMemory> RelevantMemories;

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    FString PlayerAlignment; // Current alignment summary

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    FString ReputationSummary; // How NPCs view the player

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    TMap<FString, FString> CompanionRelationships; // Companion -> Relationship status

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    TArray<FString> ActiveQuestContext; // Current quest context

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    FString LocationHistory; // Recent locations visited

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    TMap<FString, float> EmotionalState; // Current emotional context

    FNarrativeContext()
    {
        PlayerAlignment = TEXT("neutral");
        ReputationSummary = TEXT("unknown");
        LocationHistory = TEXT("");
    }
};

/**
 * Memory events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryAdded, const FNarrativeMemory&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMemoryUpdated, const FNarrativeMemory&, OldMemory, const FNarrativeMemory&, NewMemory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContextGenerated, const FNarrativeContext&, Context);

/**
 * Narrative Memory Component - Tracks player decisions and provides context for AI
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UNarrativeMemoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNarrativeMemoryComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize narrative memory system
     * @param QuestManager Quest manager for quest context
     * @param CompanionManager Companion manager for relationship context
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    void InitializeNarrativeMemory(UQuestManagerComponent* QuestManager, UCompanionManagerComponent* CompanionManager);

    /**
     * Add a new memory entry
     * @param Memory The memory to add
     * @return The generated memory ID
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    FString AddMemory(const FNarrativeMemory& Memory);

    /**
     * Add a simple memory entry
     * @param EventType Type of event
     * @param Title Short title for the memory
     * @param Description Detailed description
     * @param Importance How important this memory is
     * @param AlignmentImpact Moral alignment impact (-1.0 to 1.0)
     * @return The generated memory ID
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    FString AddSimpleMemory(EMemoryEventType EventType, const FString& Title, const FString& Description, 
                           EMemoryImportance Importance = EMemoryImportance::Minor, float AlignmentImpact = 0.0f);

    /**
     * Update an existing memory
     * @param MemoryID ID of the memory to update
     * @param UpdatedMemory New memory data
     * @return True if memory was updated
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    bool UpdateMemory(const FString& MemoryID, const FNarrativeMemory& UpdatedMemory);

    /**
     * Get memory by ID
     * @param MemoryID ID of the memory to retrieve
     * @return The memory entry, or empty if not found
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Memory")
    FNarrativeMemory GetMemory(const FString& MemoryID) const;

    /**
     * Search memories by criteria
     * @param EventType Filter by event type (optional)
     * @param MinImportance Minimum importance level
     * @param Tags Tags to search for
     * @param MaxResults Maximum number of results to return
     * @return Array of matching memories
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    TArray<FNarrativeMemory> SearchMemories(EMemoryEventType EventType = EMemoryEventType::Custom, 
                                           EMemoryImportance MinImportance = EMemoryImportance::Trivial,
                                           const TArray<FString>& Tags = TArray<FString>(),
                                           int32 MaxResults = 10) const;

    /**
     * Get recent memories
     * @param TimeWindow How far back to look (in seconds)
     * @param MaxResults Maximum number of results
     * @return Array of recent memories
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    TArray<FNarrativeMemory> GetRecentMemories(float TimeWindow = 3600.0f, int32 MaxResults = 10) const;

    /**
     * Generate narrative context for AI prompts
     * @param ContextType Type of context needed ("dialogue", "quest", "general")
     * @param NPCName Name of NPC for targeted context (optional)
     * @param Location Current location for location-specific context (optional)
     * @return Generated narrative context
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    FNarrativeContext GenerateNarrativeContext(const FString& ContextType = TEXT("general"), 
                                              const FString& NPCName = TEXT(""), 
                                              const FString& Location = TEXT(""));

    /**
     * Get player alignment summary
     * @return Text description of player's moral alignment
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Memory")
    FString GetPlayerAlignmentSummary() const;

    /**
     * Get reputation with specific NPC or faction
     * @param NPCOrFaction Name of NPC or faction
     * @return Reputation description
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Memory")
    FString GetReputationWith(const FString& NPCOrFaction) const;

    /**
     * Clear all memories (for testing or new game)
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    void ClearAllMemories();

    /**
     * Save memory data
     * @return Serialized memory data as JSON string
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    FString SaveMemoryData() const;

    /**
     * Load memory data
     * @param SaveData Serialized memory data as JSON string
     * @return True if data was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Memory")
    bool LoadMemoryData(const FString& SaveData);

    /**
     * Get memory statistics
     * @return Map of statistics (total memories, by type, etc.)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Memory")
    TMap<FString, int32> GetMemoryStatistics() const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnMemoryAdded OnMemoryAdded;

    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnMemoryUpdated OnMemoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnContextGenerated OnContextGenerated;

protected:
    // Memory storage
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Memory")
    TArray<FNarrativeMemory> Memories;

    // Component references
    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    // Memory settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    int32 MaxMemories; // Maximum number of memories to keep

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float MemoryDecayRate; // How quickly memories lose importance over time

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    bool bAutoTrackQuests; // Automatically track quest events

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    bool bAutoTrackCompanions; // Automatically track companion interactions

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float ContextUpdateInterval; // How often to update context (seconds)

    // Cached context data
    UPROPERTY()
    FNarrativeContext CachedContext;

    UPROPERTY()
    float LastContextUpdate;

private:
    // Helper methods
    FString GenerateMemoryID();
    void CleanupOldMemories();
    float CalculateMemoryRelevance(const FNarrativeMemory& Memory, const FString& ContextType, 
                                  const FString& NPCName, const FString& Location) const;
    void UpdateCachedContext();
    FString GenerateAlignmentSummary() const;
    FString GenerateReputationSummary() const;
    TMap<FString, FString> GenerateCompanionRelationshipSummary() const;
    TArray<FString> GenerateActiveQuestContext() const;
    FString GenerateLocationHistory() const;

    // Auto-tracking event handlers
    UFUNCTION()
    void OnQuestStarted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnQuestCompleted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnCompanionLoyaltyChanged(const FActiveCompanion& Companion);

public:
    /**
     * Blueprint implementable events for custom memory logic
     */

    /**
     * Called when a memory is added (for custom processing)
     * @param Memory The memory that was added
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Memory Events")
    void OnMemoryAddedEvent(const FNarrativeMemory& Memory);

    /**
     * Called to generate custom context (override in Blueprint)
     * @param ContextType Type of context being generated
     * @param NPCName Target NPC (if any)
     * @param Location Current location (if any)
     * @return Custom context data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Memory Events")
    FNarrativeContext GenerateCustomContext(const FString& ContextType, const FString& NPCName, const FString& Location);

    /**
     * Called to filter memories for context (override in Blueprint)
     * @param Memories All available memories
     * @param ContextType Type of context being generated
     * @return Filtered memories for context
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Memory Events")
    TArray<FNarrativeMemory> FilterMemoriesForContext(const TArray<FNarrativeMemory>& Memories, const FString& ContextType);
};
