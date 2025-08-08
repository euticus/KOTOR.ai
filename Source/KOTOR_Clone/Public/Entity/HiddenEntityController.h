// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Character/MutationManagerComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "HiddenEntityController.generated.h"

/**
 * Entity influence types
 */
UENUM(BlueprintType)
enum class EEntityInfluenceType : uint8
{
    Subtle              UMETA(DisplayName = "Subtle Manipulation"),
    Whispers            UMETA(DisplayName = "Whispers"),
    Dreams              UMETA(DisplayName = "Dreams"),
    Visions             UMETA(DisplayName = "Visions"),
    Compulsion          UMETA(DisplayName = "Compulsion"),
    Possession          UMETA(DisplayName = "Possession"),
    Hallucination       UMETA(DisplayName = "Hallucination"),
    MemoryAlteration    UMETA(DisplayName = "Memory Alteration"),
    EmotionalManipulation UMETA(DisplayName = "Emotional Manipulation"),
    ChoiceInfluence     UMETA(DisplayName = "Choice Influence")
};

/**
 * Entity manifestation levels
 */
UENUM(BlueprintType)
enum class EEntityManifestation : uint8
{
    Dormant             UMETA(DisplayName = "Dormant"),
    Stirring            UMETA(DisplayName = "Stirring"),
    Whispering          UMETA(DisplayName = "Whispering"),
    Influencing         UMETA(DisplayName = "Influencing"),
    Manifesting         UMETA(DisplayName = "Manifesting"),
    Controlling         UMETA(DisplayName = "Controlling"),
    Revealed            UMETA(DisplayName = "Revealed"),
    Confronted          UMETA(DisplayName = "Confronted")
};

/**
 * Hidden entity data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FHiddenEntity
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    FString EntityID;

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    FString EntityName; // True name (hidden from player initially)

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    FString EntityType; // "force_parasite", "ancient_spirit", "sith_lord", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    FString Description; // Entity's nature and goals

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    EEntityManifestation ManifestationLevel;

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    float InfluencePower; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    float PlayerAwareness; // 0.0 to 1.0, how aware player is of entity

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    TArray<FString> Goals; // Entity's objectives

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    TArray<FString> Triggers; // Events that strengthen the entity

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    TArray<FString> Weaknesses; // Things that weaken the entity

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    TMap<FString, float> InfluenceHistory; // Event -> influence gained

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    bool bIsRevealed; // Whether player knows about the entity

    UPROPERTY(BlueprintReadWrite, Category = "Hidden Entity")
    float LastInfluenceTime; // When entity last acted

    FHiddenEntity()
    {
        EntityID = TEXT("");
        EntityName = TEXT("Unknown Entity");
        EntityType = TEXT("force_parasite");
        Description = TEXT("");
        ManifestationLevel = EEntityManifestation::Dormant;
        InfluencePower = 0.0f;
        PlayerAwareness = 0.0f;
        bIsRevealed = false;
        LastInfluenceTime = 0.0f;
    }
};

/**
 * Entity influence event
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FEntityInfluenceEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    FString EventID;

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    FString EntityID;

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    EEntityInfluenceType InfluenceType;

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    FString InfluenceDescription; // LLM-generated description

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    FString TriggerEvent; // What triggered this influence

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    float Intensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    float Duration; // How long influence lasts

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    TMap<FString, FString> EffectParameters; // Specific effect parameters

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    bool bIsSubtle; // Whether influence is obvious to player

    UPROPERTY(BlueprintReadWrite, Category = "Entity Influence")
    float Timestamp; // When influence occurred

    FEntityInfluenceEvent()
    {
        EventID = TEXT("");
        EntityID = TEXT("");
        InfluenceType = EEntityInfluenceType::Subtle;
        InfluenceDescription = TEXT("");
        TriggerEvent = TEXT("");
        Intensity = 0.5f;
        Duration = 60.0f;
        bIsSubtle = true;
        Timestamp = 0.0f;
    }
};

/**
 * Entity events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntityInfluenceTriggered, const FEntityInfluenceEvent&, InfluenceEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEntityManifestationChanged, const FString&, EntityID, EEntityManifestation, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntityRevealed, const FHiddenEntity&, Entity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerAwarenessChanged, const FString&, EntityID, float, NewAwareness);

/**
 * Hidden Entity Controller - Manages subtle narrative forces manipulating the player
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UHiddenEntityController : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Initialize hidden entity controller
     * @param CampaignLoader Campaign loader for entity data
     * @param NarrativeMemory Narrative memory for tracking player actions
     * @param MutationManager Mutation manager for corruption effects
     * @param CompanionManager Companion manager for influence on companions
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    void InitializeEntityController(UCampaignLoaderSubsystem* CampaignLoader,
                                   UNarrativeMemoryComponent* NarrativeMemory,
                                   UMutationManagerComponent* MutationManager,
                                   UCompanionManagerComponent* CompanionManager);

    /**
     * Create hidden entity
     * @param EntityType Type of entity to create
     * @param EntityName Name of the entity
     * @param Goals Entity's objectives
     * @return Created hidden entity
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    FHiddenEntity CreateHiddenEntity(const FString& EntityType, const FString& EntityName, 
                                    const TArray<FString>& Goals);

    /**
     * Trigger entity influence
     * @param EntityID ID of the entity
     * @param InfluenceType Type of influence to trigger
     * @param TriggerEvent Event that triggered the influence
     * @param Intensity Intensity of the influence
     * @return Generated influence event
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    FEntityInfluenceEvent TriggerEntityInfluence(const FString& EntityID, 
                                                 EEntityInfluenceType InfluenceType,
                                                 const FString& TriggerEvent, 
                                                 float Intensity = 0.5f);

    /**
     * Update entity manifestation level
     * @param EntityID ID of the entity
     * @param PowerIncrease Amount to increase manifestation power
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    void UpdateEntityManifestation(const FString& EntityID, float PowerIncrease);

    /**
     * Increase player awareness of entity
     * @param EntityID ID of the entity
     * @param AwarenessIncrease Amount to increase awareness
     * @param Reason Reason for awareness increase
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    void IncreasePlayerAwareness(const FString& EntityID, float AwarenessIncrease, const FString& Reason);

    /**
     * Reveal entity to player
     * @param EntityID ID of the entity to reveal
     * @param RevealMethod How the entity is revealed
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    void RevealEntity(const FString& EntityID, const FString& RevealMethod);

    /**
     * Check if entity should influence player action
     * @param ActionContext Context of the action
     * @return Entity influence event (if any)
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    FEntityInfluenceEvent CheckForEntityInfluence(const FString& ActionContext);

    /**
     * Get active hidden entities
     * @return Array of active hidden entities
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hidden Entity")
    TArray<FHiddenEntity> GetActiveHiddenEntities() const;

    /**
     * Get entity by ID
     * @param EntityID ID of the entity
     * @return Hidden entity data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hidden Entity")
    FHiddenEntity GetHiddenEntity(const FString& EntityID) const;

    /**
     * Get revealed entities
     * @return Array of entities that have been revealed to player
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hidden Entity")
    TArray<FHiddenEntity> GetRevealedEntities() const;

    /**
     * Get entity influence history
     * @param EntityID ID of the entity
     * @return Array of influence events for the entity
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hidden Entity")
    TArray<FEntityInfluenceEvent> GetEntityInfluenceHistory(const FString& EntityID) const;

    /**
     * Set entity influence enabled
     * @param bEnabled Whether to enable entity influence
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    void SetEntityInfluenceEnabled(bool bEnabled);

    /**
     * Set entity influence frequency
     * @param Frequency How often entities attempt to influence (seconds)
     */
    UFUNCTION(BlueprintCallable, Category = "Hidden Entity")
    void SetEntityInfluenceFrequency(float Frequency);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Entity Events")
    FOnEntityInfluenceTriggered OnEntityInfluenceTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Entity Events")
    FOnEntityManifestationChanged OnEntityManifestationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Entity Events")
    FOnEntityRevealed OnEntityRevealed;

    UPROPERTY(BlueprintAssignable, Category = "Entity Events")
    FOnPlayerAwarenessChanged OnPlayerAwarenessChanged;

protected:
    // Entity data
    UPROPERTY(BlueprintReadOnly, Category = "Hidden Entity")
    TArray<FHiddenEntity> HiddenEntities;

    UPROPERTY(BlueprintReadOnly, Category = "Hidden Entity")
    TArray<FEntityInfluenceEvent> InfluenceHistory;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UMutationManagerComponent* MutationManagerRef;

    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    // Entity settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Settings")
    bool bEntityInfluenceEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Settings")
    float InfluenceFrequency; // Seconds between influence attempts

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Settings")
    float SubtletyThreshold; // Threshold for subtle vs obvious influence

    // Entity templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Templates")
    TMap<FString, FHiddenEntity> EntityTemplates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Templates")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<EEntityInfluenceType, TArray<FString>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by EEntityInfluenceType when needed.
     */
    TArray<FString>InfluencePromptTemplates;

    // Timer handles
    FTimerHandle InfluenceTimer;

private:
    // Helper methods
    void LoadEntityTemplates();
    FString GenerateEntityID();
    FString GenerateEventID();
    FHiddenEntity* FindEntity(const FString& EntityID);
    EEntityManifestation CalculateManifestationLevel(float InfluencePower);
    bool ShouldTriggerInfluence(const FHiddenEntity& Entity, const FString& Context);
    FString GenerateInfluenceDescription(const FHiddenEntity& Entity, EEntityInfluenceType InfluenceType, const FString& Context);
    void ProcessAutomaticInfluence();

    // Event handlers
    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

    UFUNCTION()
    void OnCorruptionChanged(int32 OldCorruption, int32 NewCorruption);

public:
    /**
     * Blueprint implementable events for custom entity logic
     */

    /**
     * Called when entity influence is triggered (for custom effects)
     * @param InfluenceEvent The triggered influence event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Entity Events")
    void OnEntityInfluenceTriggeredEvent(const FEntityInfluenceEvent& InfluenceEvent);

    /**
     * Called when entity manifestation changes (for custom effects)
     * @param EntityID The entity whose manifestation changed
     * @param OldLevel Previous manifestation level
     * @param NewLevel New manifestation level
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Entity Events")
    void OnEntityManifestationChangedEvent(const FString& EntityID, EEntityManifestation OldLevel, EEntityManifestation NewLevel);

    /**
     * Called to generate custom entity influence (override in Blueprint)
     * @param Entity The entity attempting influence
     * @param Context Current context
     * @return Custom influence event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Entity Events")
    FEntityInfluenceEvent GenerateCustomEntityInfluence(const FHiddenEntity& Entity, const FString& Context);

    /**
     * Called to determine if entity should influence action (override in Blueprint)
     * @param Entity The entity
     * @param ActionContext Context of the action
     * @return True if entity should influence
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Entity Events")
    bool ShouldEntityInfluenceAction(const FHiddenEntity& Entity, const FString& ActionContext);

    /**
     * Called when entity is revealed (for custom revelation sequence)
     * @param Entity The revealed entity
     * @param RevealMethod How the entity was revealed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Entity Events")
    void OnEntityRevealedEvent(const FHiddenEntity& Entity, const FString& RevealMethod);
};
