// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Character/MutationManagerComponent.h"
#include "MythicArtifactSystem.generated.h"

/**
 * Artifact types
 */
UENUM(BlueprintType)
enum class EArtifactType : uint8
{
    Lightsaber      UMETA(DisplayName = "Lightsaber"),
    ForceArtifact   UMETA(DisplayName = "Force Artifact"),
    AncientWeapon   UMETA(DisplayName = "Ancient Weapon"),
    SithHolocron    UMETA(DisplayName = "Sith Holocron"),
    JediHolocron    UMETA(DisplayName = "Jedi Holocron"),
    CrystalCore     UMETA(DisplayName = "Crystal Core"),
    Armor           UMETA(DisplayName = "Legendary Armor"),
    Talisman        UMETA(DisplayName = "Mystical Talisman"),
    Relic           UMETA(DisplayName = "Ancient Relic"),
    Custom          UMETA(DisplayName = "Custom Artifact")
};

/**
 * Artifact evolution stages
 */
UENUM(BlueprintType)
enum class EArtifactStage : uint8
{
    Dormant         UMETA(DisplayName = "Dormant"),
    Awakening       UMETA(DisplayName = "Awakening"),
    Active          UMETA(DisplayName = "Active"),
    Empowered       UMETA(DisplayName = "Empowered"),
    Transcendent    UMETA(DisplayName = "Transcendent"),
    Corrupted       UMETA(DisplayName = "Corrupted"),
    Purified        UMETA(DisplayName = "Purified")
};

/**
 * Artifact resonance data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FArtifactResonance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Artifact Resonance")
    FString ResonanceType; // "light", "dark", "neutral", "chaos", "order"

    UPROPERTY(BlueprintReadWrite, Category = "Artifact Resonance")
    float Intensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Artifact Resonance")
    TArray<FString> TriggerEvents; // Events that strengthen this resonance

    UPROPERTY(BlueprintReadWrite, Category = "Artifact Resonance")
    TMap<FString, float> StatBonuses; // Stat -> bonus amount

    UPROPERTY(BlueprintReadWrite, Category = "Artifact Resonance")
    TArray<FString> UnlockedAbilities; // Abilities unlocked by this resonance

    FArtifactResonance()
    {
        ResonanceType = TEXT("neutral");
        Intensity = 0.0f;
    }
};

/**
 * Mythic artifact data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMythicArtifact
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    FString ArtifactID;

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    FString ArtifactName;

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    EArtifactType Type;

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    EArtifactStage CurrentStage;

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    FString Description; // LLM-generated description

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    FString Lore; // Artifact's history and legend

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    TArray<FArtifactResonance> Resonances; // Different resonance types

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    int32 EvolutionPoints; // Points accumulated toward next stage

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    int32 RequiredEvolutionPoints; // Points needed for next evolution

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    TMap<FString, float> BaseStats; // Base stat bonuses

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    TArray<FString> ActiveAbilities; // Currently active abilities

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    TArray<FString> EvolutionHistory; // Record of how artifact evolved

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    FString BondedPlayer; // Player who bonded with this artifact

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    float BondStrength; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Mythic Artifact")
    bool bIsCorrupted; // Whether artifact is corrupted

    FMythicArtifact()
    {
        ArtifactID = TEXT("");
        ArtifactName = TEXT("Unknown Artifact");
        Type = EArtifactType::Custom;
        CurrentStage = EArtifactStage::Dormant;
        Description = TEXT("");
        Lore = TEXT("");
        EvolutionPoints = 0;
        RequiredEvolutionPoints = 100;
        BondedPlayer = TEXT("");
        BondStrength = 0.0f;
        bIsCorrupted = false;
    }
};

/**
 * Artifact evolution event
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FArtifactEvolutionEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    FString EventID;

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    FString ArtifactID;

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    EArtifactStage OldStage;

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    EArtifactStage NewStage;

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    FString TriggerEvent; // What caused the evolution

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    TArray<FString> NewAbilities; // Abilities gained

    UPROPERTY(BlueprintReadWrite, Category = "Evolution Event")
    FString EvolutionDescription; // LLM-generated description

    FArtifactEvolutionEvent()
    {
        EventID = TEXT("");
        ArtifactID = TEXT("");
        OldStage = EArtifactStage::Dormant;
        NewStage = EArtifactStage::Awakening;
        TriggerEvent = TEXT("");
        EvolutionDescription = TEXT("");
    }
};

/**
 * Artifact events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArtifactEvolved, const FArtifactEvolutionEvent&, EvolutionEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnArtifactBonded, const FMythicArtifact&, Artifact, const FString&, PlayerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArtifactCorrupted, const FMythicArtifact&, Artifact);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArtifactPurified, const FMythicArtifact&, Artifact);

/**
 * Mythic Artifact System - Manages legendary items that evolve with player choices
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMythicArtifactSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UMythicArtifactSystem();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize artifact system
     * @param NarrativeMemory Narrative memory for tracking player actions
     * @param MutationManager Mutation manager for corruption tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    void InitializeArtifactSystem(UNarrativeMemoryComponent* NarrativeMemory, 
                                 UMutationManagerComponent* MutationManager);

    /**
     * Create mythic artifact
     * @param Type Type of artifact to create
     * @param ArtifactName Name for the artifact
     * @param PlayerID Player who will bond with the artifact
     * @return Created artifact
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    FMythicArtifact CreateMythicArtifact(EArtifactType Type, const FString& ArtifactName, const FString& PlayerID);

    /**
     * Bond player to artifact
     * @param ArtifactID ID of the artifact
     * @param PlayerID ID of the player
     * @return True if bonding was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    bool BondPlayerToArtifact(const FString& ArtifactID, const FString& PlayerID);

    /**
     * Add evolution points to artifact
     * @param ArtifactID ID of the artifact
     * @param Points Points to add
     * @param TriggerEvent Event that triggered the points
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    void AddEvolutionPoints(const FString& ArtifactID, int32 Points, const FString& TriggerEvent);

    /**
     * Evolve artifact to next stage
     * @param ArtifactID ID of the artifact to evolve
     * @param TriggerEvent Event that triggered evolution
     * @return Evolution event data
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    FArtifactEvolutionEvent EvolveArtifact(const FString& ArtifactID, const FString& TriggerEvent);

    /**
     * Corrupt artifact
     * @param ArtifactID ID of the artifact to corrupt
     * @param CorruptionSource Source of corruption
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    void CorruptArtifact(const FString& ArtifactID, const FString& CorruptionSource);

    /**
     * Purify corrupted artifact
     * @param ArtifactID ID of the artifact to purify
     * @param PurificationMethod Method used for purification
     * @return True if purification was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    bool PurifyArtifact(const FString& ArtifactID, const FString& PurificationMethod);

    /**
     * Update artifact resonance
     * @param ArtifactID ID of the artifact
     * @param ResonanceType Type of resonance to update
     * @param IntensityChange Change in resonance intensity
     * @param TriggerEvent Event that triggered the change
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    void UpdateArtifactResonance(const FString& ArtifactID, const FString& ResonanceType, 
                                float IntensityChange, const FString& TriggerEvent);

    /**
     * Get artifact by ID
     * @param ArtifactID ID of the artifact
     * @return Artifact data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mythic Artifacts")
    FMythicArtifact GetArtifact(const FString& ArtifactID) const;

    /**
     * Get player's bonded artifacts
     * @param PlayerID ID of the player
     * @return Array of bonded artifacts
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mythic Artifacts")
    TArray<FMythicArtifact> GetPlayerArtifacts(const FString& PlayerID) const;

    /**
     * Get all artifacts
     * @return Array of all artifacts
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mythic Artifacts")
    TArray<FMythicArtifact> GetAllArtifacts() const { return MythicArtifacts; }

    /**
     * Check if artifact can evolve
     * @param ArtifactID ID of the artifact
     * @return True if artifact has enough evolution points
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mythic Artifacts")
    bool CanArtifactEvolve(const FString& ArtifactID) const;

    /**
     * Get artifact evolution history
     * @param ArtifactID ID of the artifact
     * @return Array of evolution events
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mythic Artifacts")
    TArray<FArtifactEvolutionEvent> GetArtifactEvolutionHistory(const FString& ArtifactID) const;

    /**
     * Generate artifact lore
     * @param Artifact Artifact to generate lore for
     * @return Generated lore text
     */
    UFUNCTION(BlueprintCallable, Category = "Mythic Artifacts")
    FString GenerateArtifactLore(const FMythicArtifact& Artifact);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Artifact Events")
    FOnArtifactEvolved OnArtifactEvolved;

    UPROPERTY(BlueprintAssignable, Category = "Artifact Events")
    FOnArtifactBonded OnArtifactBonded;

    UPROPERTY(BlueprintAssignable, Category = "Artifact Events")
    FOnArtifactCorrupted OnArtifactCorrupted;

    UPROPERTY(BlueprintAssignable, Category = "Artifact Events")
    FOnArtifactPurified OnArtifactPurified;

protected:
    // Artifact data
    UPROPERTY(BlueprintReadOnly, Category = "Mythic Artifacts")
    TArray<FMythicArtifact> MythicArtifacts;

    UPROPERTY(BlueprintReadOnly, Category = "Mythic Artifacts")
    TArray<FArtifactEvolutionEvent> EvolutionHistory;

    // Component references
    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UMutationManagerComponent* MutationManagerRef;

    // Artifact templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Artifact Templates")
    TMap<EArtifactType, FMythicArtifact> ArtifactTemplates;

    // Evolution settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution Settings")
    TMap<EArtifactStage, int32> StageEvolutionRequirements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution Settings")
    TMap<FString, int32> EventEvolutionPoints; // Event type -> points awarded

    // Resonance settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resonance Settings")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<FString, TArray<FString>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by FString when needed.
     */
    TArray<FString>ResonanceTriggers; // Resonance type -> trigger events

private:
    // Helper methods
    void LoadArtifactTemplates();
    FMythicArtifact* FindArtifact(const FString& ArtifactID);
    FArtifactResonance* FindResonance(FMythicArtifact& Artifact, const FString& ResonanceType);
    FString GenerateArtifactID();
    FString GenerateEventID();
    FString GenerateArtifactDescription(const FMythicArtifact& Artifact);
    TArray<FString> GetStageAbilities(EArtifactType Type, EArtifactStage Stage);
    void ProcessResonanceEffects(FMythicArtifact& Artifact);

    // Event handlers
    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

    UFUNCTION()
    void OnCorruptionChanged(int32 OldCorruption, int32 NewCorruption);

public:
    /**
     * Blueprint implementable events for custom artifact logic
     */

    /**
     * Called when artifact evolves (for custom effects)
     * @param EvolutionEvent The evolution event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Artifact Events")
    void OnArtifactEvolvedEvent(const FArtifactEvolutionEvent& EvolutionEvent);

    /**
     * Called when artifact is bonded (for custom effects)
     * @param Artifact The bonded artifact
     * @param PlayerID The player who bonded
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Artifact Events")
    void OnArtifactBondedEvent(const FMythicArtifact& Artifact, const FString& PlayerID);

    /**
     * Called to generate custom artifact lore (override in Blueprint)
     * @param Artifact The artifact to generate lore for
     * @return Custom lore text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Artifact Events")
    FString GenerateCustomArtifactLore(const FMythicArtifact& Artifact);

    /**
     * Called to determine custom evolution requirements (override in Blueprint)
     * @param Artifact The artifact being evolved
     * @param TriggerEvent The trigger event
     * @return Custom evolution points to award
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Artifact Events")
    int32 GetCustomEvolutionPoints(const FMythicArtifact& Artifact, const FString& TriggerEvent);
};
