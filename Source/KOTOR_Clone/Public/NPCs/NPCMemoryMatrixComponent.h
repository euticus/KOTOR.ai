// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Politics/FactionDiplomacySystem.h"
#include "NPCMemoryMatrixComponent.generated.h"

/**
 * Memory types
 */
UENUM(BlueprintType)
enum class ENPCMemoryType : uint8
{
    FirstImpression     UMETA(DisplayName = "First Impression"),
    DirectInteraction   UMETA(DisplayName = "Direct Interaction"),
    Witnessed           UMETA(DisplayName = "Witnessed Event"),
    Gossip              UMETA(DisplayName = "Gossip/Hearsay"),
    Reputation          UMETA(DisplayName = "Reputation"),
    Faction             UMETA(DisplayName = "Faction Standing"),
    Personal            UMETA(DisplayName = "Personal Relationship"),
    Professional        UMETA(DisplayName = "Professional Relationship")
};

/**
 * Memory reliability
 */
UENUM(BlueprintType)
enum class EMemoryReliability : uint8
{
    Certain         UMETA(DisplayName = "Certain"),
    Confident       UMETA(DisplayName = "Confident"),
    Likely          UMETA(DisplayName = "Likely"),
    Uncertain       UMETA(DisplayName = "Uncertain"),
    Doubtful        UMETA(DisplayName = "Doubtful"),
    Rumor           UMETA(DisplayName = "Rumor")
};

/**
 * NPC memory entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNPCMemoryEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    FString MemoryID;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    ENPCMemoryType MemoryType;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    FString Subject; // Who/what this memory is about

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    FString Event; // What happened

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    FString Context; // Additional context

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    EMemoryReliability Reliability;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    float EmotionalWeight; // -1.0 (very negative) to 1.0 (very positive)

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    float Timestamp; // When this memory was formed

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    FString Source; // Who told them this (for gossip)

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    TArray<FString> Witnesses; // Who else was present

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    bool bSharedWithOthers; // Whether this has been gossiped

    UPROPERTY(BlueprintReadWrite, Category = "NPC Memory")
    float DecayRate; // How fast this memory fades

    FNPCMemoryEntry()
    {
        MemoryID = TEXT("");
        MemoryType = ENPCMemoryType::DirectInteraction;
        Subject = TEXT("");
        Event = TEXT("");
        Context = TEXT("");
        Reliability = EMemoryReliability::Certain;
        EmotionalWeight = 0.0f;
        Timestamp = 0.0f;
        Source = TEXT("");
        bSharedWithOthers = false;
        DecayRate = 0.01f; // Slow decay by default
    }
};

/**
 * Social relationship data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSocialRelationship
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    FString NPCID;

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    FString RelatedNPCID;

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    FString RelationshipType; // "friend", "enemy", "family", "colleague", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    float TrustLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    float GossipProbability; // Chance of sharing gossip

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    TArray<FString> SharedMemories; // Memory IDs they both know

    UPROPERTY(BlueprintReadWrite, Category = "Social Relationship")
    float LastInteraction; // When they last spoke

    FSocialRelationship()
    {
        NPCID = TEXT("");
        RelatedNPCID = TEXT("");
        RelationshipType = TEXT("acquaintance");
        TrustLevel = 0.5f;
        GossipProbability = 0.3f;
        LastInteraction = 0.0f;
    }
};

/**
 * Gossip propagation data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGossipPropagation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Gossip Propagation")
    FString OriginalMemoryID;

    UPROPERTY(BlueprintReadWrite, Category = "Gossip Propagation")
    FString SourceNPC;

    UPROPERTY(BlueprintReadWrite, Category = "Gossip Propagation")
    FString TargetNPC;

    UPROPERTY(BlueprintReadWrite, Category = "Gossip Propagation")
    float PropagationTime;

    UPROPERTY(BlueprintReadWrite, Category = "Gossip Propagation")
    float ReliabilityDecay; // How much reliability decreases

    UPROPERTY(BlueprintReadWrite, Category = "Gossip Propagation")
    bool bCompleted;

    FGossipPropagation()
    {
        OriginalMemoryID = TEXT("");
        SourceNPC = TEXT("");
        TargetNPC = TEXT("");
        PropagationTime = 0.0f;
        ReliabilityDecay = 0.1f;
        bCompleted = false;
    }
};

/**
 * NPC memory events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCMemoryAdded, const FString&, NPCID, const FNPCMemoryEntry&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGossipPropagated, const FGossipPropagation&, Gossip);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCReactionTriggered, const FString&, NPCID, const FString&, ReactionType);

/**
 * NPC Memory Matrix Component - Tracks NPC memories, relationships, and gossip propagation
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UNPCMemoryMatrixComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNPCMemoryMatrixComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize NPC memory matrix
     * @param NarrativeMemory Narrative memory for player actions
     * @param FactionSystem Faction system for reputation tracking
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    void InitializeMemoryMatrix(UNarrativeMemoryComponent* NarrativeMemory, 
                               UFactionDiplomacySystem* FactionSystem);

    /**
     * Add memory to NPC
     * @param NPCID ID of the NPC
     * @param Memory Memory entry to add
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    void AddNPCMemory(const FString& NPCID, const FNPCMemoryEntry& Memory);

    /**
     * Record player interaction with NPC
     * @param NPCID ID of the NPC
     * @param InteractionType Type of interaction
     * @param Context Additional context
     * @param EmotionalWeight Emotional impact (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    void RecordPlayerInteraction(const FString& NPCID, const FString& InteractionType, 
                                const FString& Context, float EmotionalWeight);

    /**
     * Propagate gossip between NPCs
     * @param SourceNPC NPC sharing the gossip
     * @param TargetNPC NPC receiving the gossip
     * @param MemoryID Memory being shared
     * @return True if gossip was successfully propagated
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    bool PropagateGossip(const FString& SourceNPC, const FString& TargetNPC, const FString& MemoryID);

    /**
     * Add social relationship between NPCs
     * @param NPCID First NPC
     * @param RelatedNPCID Second NPC
     * @param RelationshipType Type of relationship
     * @param TrustLevel Trust level (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    void AddSocialRelationship(const FString& NPCID, const FString& RelatedNPCID, 
                              const FString& RelationshipType, float TrustLevel);

    /**
     * Get NPC memories about subject
     * @param NPCID ID of the NPC
     * @param Subject Subject to search for
     * @return Array of relevant memories
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Memory")
    TArray<FNPCMemoryEntry> GetNPCMemoriesAbout(const FString& NPCID, const FString& Subject) const;

    /**
     * Get NPC's first impression of player
     * @param NPCID ID of the NPC
     * @return First impression memory entry
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Memory")
    FNPCMemoryEntry GetNPCFirstImpression(const FString& NPCID) const;

    /**
     * Get NPC's overall opinion of player
     * @param NPCID ID of the NPC
     * @return Overall opinion score (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Memory")
    float GetNPCOpinionOfPlayer(const FString& NPCID) const;

    /**
     * Get social relationships for NPC
     * @param NPCID ID of the NPC
     * @return Array of social relationships
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Memory")
    TArray<FSocialRelationship> GetNPCSocialRelationships(const FString& NPCID) const;

    /**
     * Check if NPC knows about event
     * @param NPCID ID of the NPC
     * @param Event Event to check
     * @return True if NPC has memory of the event
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Memory")
    bool DoesNPCKnowAbout(const FString& NPCID, const FString& Event) const;

    /**
     * Generate dialogue context for NPC
     * @param NPCID ID of the NPC
     * @return Context string for dialogue generation
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    FString GenerateDialogueContext(const FString& NPCID) const;

    /**
     * Trigger automatic gossip propagation
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    void TriggerAutomaticGossipPropagation();

    /**
     * Set gossip propagation enabled
     * @param bEnabled Whether to enable automatic gossip propagation
     */
    UFUNCTION(BlueprintCallable, Category = "NPC Memory")
    void SetGossipPropagationEnabled(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "NPC Memory Events")
    FOnNPCMemoryAdded OnNPCMemoryAdded;

    UPROPERTY(BlueprintAssignable, Category = "NPC Memory Events")
    FOnGossipPropagated OnGossipPropagated;

    UPROPERTY(BlueprintAssignable, Category = "NPC Memory Events")
    FOnNPCReactionTriggered OnNPCReactionTriggered;

protected:
    // Memory data
    UPROPERTY(BlueprintReadOnly, Category = "NPC Memory")
    TMap<FString, TArray<FNPCMemoryEntry>> NPCMemories; // NPCID -> Memories

    UPROPERTY(BlueprintReadOnly, Category = "NPC Memory")
    TArray<FSocialRelationship> SocialRelationships;

    UPROPERTY(BlueprintReadOnly, Category = "NPC Memory")
    TArray<FGossipPropagation> PendingGossip;

    // Component references
    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UFactionDiplomacySystem* FactionSystemRef;

    // Memory settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    bool bGossipPropagationEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float GossipPropagationInterval; // Seconds between gossip checks

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float MemoryDecayRate; // How fast memories fade

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    int32 MaxMemoriesPerNPC; // Maximum memories to store per NPC

    // Timer handles
    FTimerHandle GossipTimer;
    FTimerHandle MemoryDecayTimer;

private:
    // Helper methods
    FString GenerateMemoryID();
    void ProcessMemoryDecay();
    void ProcessPendingGossip();
    TArray<FNPCMemoryEntry>* FindNPCMemories(const FString& NPCID);
    FSocialRelationship* FindSocialRelationship(const FString& NPCID, const FString& RelatedNPCID);
    TArray<FString> FindPotentialGossipTargets(const FString& SourceNPC, const FNPCMemoryEntry& Memory);
    float CalculateGossipProbability(const FString& SourceNPC, const FString& TargetNPC, const FNPCMemoryEntry& Memory);
    void TrimOldMemories(const FString& NPCID);

    // Event handlers
    UFUNCTION()
    void OnNarrativeMemoryAdded(const FNarrativeMemory& Memory);

    UFUNCTION()
    void OnPlayerReputationChanged(const FString& FactionID, int32 NewReputation);

public:
    /**
     * Blueprint implementable events for custom memory logic
     */

    /**
     * Called when NPC memory is added (for custom processing)
     * @param NPCID The NPC who gained the memory
     * @param Memory The memory that was added
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Memory Events")
    void OnNPCMemoryAddedEvent(const FString& NPCID, const FNPCMemoryEntry& Memory);

    /**
     * Called when gossip is propagated (for custom effects)
     * @param Gossip The gossip propagation data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Memory Events")
    void OnGossipPropagatedEvent(const FGossipPropagation& Gossip);

    /**
     * Called to generate custom NPC reaction (override in Blueprint)
     * @param NPCID The NPC reacting
     * @param Memory The memory triggering the reaction
     * @return Custom reaction type
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Memory Events")
    FString GenerateCustomNPCReaction(const FString& NPCID, const FNPCMemoryEntry& Memory);

    /**
     * Called to determine custom gossip targets (override in Blueprint)
     * @param SourceNPC NPC sharing gossip
     * @param Memory Memory being shared
     * @return Custom list of potential targets
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Memory Events")
    TArray<FString> GetCustomGossipTargets(const FString& SourceNPC, const FNPCMemoryEntry& Memory);
};
