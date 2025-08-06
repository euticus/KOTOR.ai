// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "MutationManagerComponent.generated.h"

/**
 * Mutation types
 */
UENUM(BlueprintType)
enum class EMutationType : uint8
{
    Physical        UMETA(DisplayName = "Physical"),
    Mental          UMETA(DisplayName = "Mental"),
    Spiritual       UMETA(DisplayName = "Spiritual"),
    Sensory         UMETA(DisplayName = "Sensory"),
    Vocal           UMETA(DisplayName = "Vocal"),
    Aura            UMETA(DisplayName = "Aura")
};

/**
 * Corruption sources
 */
UENUM(BlueprintType)
enum class ECorruptionSource : uint8
{
    KillingInnocents    UMETA(DisplayName = "Killing Innocents"),
    ForceAbuse          UMETA(DisplayName = "Force Abuse"),
    Betrayal            UMETA(DisplayName = "Betrayal"),
    Torture             UMETA(DisplayName = "Torture"),
    DarkSideChoices     UMETA(DisplayName = "Dark Side Choices"),
    SithArtifacts       UMETA(DisplayName = "Sith Artifacts"),
    DarkSideTeaching    UMETA(DisplayName = "Dark Side Teaching"),
    Manipulation        UMETA(DisplayName = "Manipulation")
};

/**
 * Mutation data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMutationData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    FString MutationID;

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    EMutationType Type;

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    int32 RequiredCorruption; // Corruption level needed to trigger

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    float Intensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    TMap<FString, float> VisualEffects; // Effect name -> intensity

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    TArray<FString> AudioEffects; // Sound effect IDs

    UPROPERTY(BlueprintReadWrite, Category = "Mutation")
    TMap<FString, float> StatModifiers; // Stat -> modifier

    FMutationData()
    {
        MutationID = TEXT("");
        Type = EMutationType::Physical;
        Name = TEXT("Unknown Mutation");
        Description = TEXT("");
        RequiredCorruption = 0;
        bIsActive = false;
        Intensity = 0.0f;
    }
};

/**
 * Hallucination event
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FHallucinationEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    FString EventID;

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    FString TriggerContext; // What triggered this hallucination

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    FString HallucinationText; // LLM-generated content

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    float Duration; // How long the hallucination lasts

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    TArray<FString> VisualEffects; // Visual distortions

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    TArray<FString> AudioEffects; // Whispers, voices

    UPROPERTY(BlueprintReadWrite, Category = "Hallucination")
    bool bBlocksInput; // Whether player can act during hallucination

    FHallucinationEvent()
    {
        EventID = TEXT("");
        TriggerContext = TEXT("");
        HallucinationText = TEXT("");
        Duration = 5.0f;
        bBlocksInput = false;
    }
};

/**
 * Redemption path data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FRedemptionPath
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Redemption")
    FString PathID;

    UPROPERTY(BlueprintReadWrite, Category = "Redemption")
    FString PathName;

    UPROPERTY(BlueprintReadWrite, Category = "Redemption")
    TArray<FString> RequiredQuests; // Quests that must be completed

    UPROPERTY(BlueprintReadWrite, Category = "Redemption")
    int32 CorruptionReduction; // How much corruption is reduced

    UPROPERTY(BlueprintReadWrite, Category = "Redemption")
    TArray<FString> ReversedMutations; // Mutations that are reversed

    UPROPERTY(BlueprintReadWrite, Category = "Redemption")
    bool bIsAvailable; // Whether this path is currently available

    FRedemptionPath()
    {
        PathID = TEXT("");
        PathName = TEXT("Path of Redemption");
        CorruptionReduction = 50;
        bIsAvailable = false;
    }
};

/**
 * Mutation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCorruptionChanged, int32, OldCorruption, int32, NewCorruption);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMutationTriggered, const FMutationData&, Mutation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHallucinationStarted, const FHallucinationEvent&, Hallucination);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRedemptionPathUnlocked, const FRedemptionPath&, Path);

/**
 * Mutation Manager Component - Handles dark side corruption and mutations
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMutationManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMutationManagerComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize mutation system
     * @param NarrativeMemory Narrative memory for tracking corruption sources
     * @param VoiceSynthesis Voice synthesis for whispers and voices
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void InitializeMutationSystem(UNarrativeMemoryComponent* NarrativeMemory, UVoiceSynthesisComponent* VoiceSynthesis);

    /**
     * Add corruption from specific source
     * @param Source Source of corruption
     * @param Amount Amount of corruption to add
     * @param Context Additional context for the corruption
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void AddCorruption(ECorruptionSource Source, int32 Amount, const FString& Context = TEXT(""));

    /**
     * Reduce corruption (for redemption)
     * @param Amount Amount of corruption to reduce
     * @param Reason Reason for reduction
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void ReduceCorruption(int32 Amount, const FString& Reason = TEXT(""));

    /**
     * Trigger specific mutation
     * @param MutationID ID of mutation to trigger
     * @param Intensity Intensity of the mutation (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void TriggerMutation(const FString& MutationID, float Intensity = 1.0f);

    /**
     * Remove mutation
     * @param MutationID ID of mutation to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void RemoveMutation(const FString& MutationID);

    /**
     * Trigger hallucination event
     * @param TriggerContext What triggered the hallucination
     * @param CustomText Custom hallucination text (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void TriggerHallucination(const FString& TriggerContext, const FString& CustomText = TEXT(""));

    /**
     * Start redemption path
     * @param PathID ID of redemption path to start
     * @return True if path was started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    bool StartRedemptionPath(const FString& PathID);

    /**
     * Complete redemption path step
     * @param PathID ID of redemption path
     * @param QuestID ID of completed quest
     */
    UFUNCTION(BlueprintCallable, Category = "Mutation Manager")
    void CompleteRedemptionStep(const FString& PathID, const FString& QuestID);

    /**
     * Get current corruption level
     * @return Current corruption level
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mutation Manager")
    int32 GetCorruptionLevel() const { return CorruptionLevel; }

    /**
     * Get active mutations
     * @return Array of active mutations
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mutation Manager")
    TArray<FMutationData> GetActiveMutations() const;

    /**
     * Get available redemption paths
     * @return Array of available redemption paths
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mutation Manager")
    TArray<FRedemptionPath> GetAvailableRedemptionPaths() const;

    /**
     * Check if mutation is active
     * @param MutationID ID of mutation to check
     * @return True if mutation is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mutation Manager")
    bool IsMutationActive(const FString& MutationID) const;

    /**
     * Get corruption from source
     * @param Source Corruption source
     * @return Total corruption from this source
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mutation Manager")
    int32 GetCorruptionFromSource(ECorruptionSource Source) const;

    /**
     * Get mutation intensity
     * @param MutationID ID of mutation
     * @return Current intensity (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mutation Manager")
    float GetMutationIntensity(const FString& MutationID) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Mutation Events")
    FOnCorruptionChanged OnCorruptionChanged;

    UPROPERTY(BlueprintAssignable, Category = "Mutation Events")
    FOnMutationTriggered OnMutationTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Mutation Events")
    FOnHallucinationStarted OnHallucinationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Mutation Events")
    FOnRedemptionPathUnlocked OnRedemptionPathUnlocked;

protected:
    // Corruption tracking
    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    int32 CorruptionLevel;

    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    TMap<ECorruptionSource, int32> CorruptionSources;

    // Mutations
    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    TArray<FMutationData> AvailableMutations;

    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    TArray<FMutationData> ActiveMutations;

    // Redemption paths
    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    TArray<FRedemptionPath> RedemptionPaths;

    // Current hallucination
    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    FHallucinationEvent CurrentHallucination;

    UPROPERTY(BlueprintReadOnly, Category = "Mutation Manager")
    bool bHallucinationActive;

    // Component references
    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisRef;

    UPROPERTY()
    UAudioComponent* WhisperAudioComponent;

    // Mutation settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation Settings")
    TArray<int32> MutationThresholds; // Corruption levels that trigger mutations

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation Settings")
    float HallucinationChance; // Chance per corruption point to trigger hallucination

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation Settings")
    float WhisperFrequency; // How often whispers occur (seconds)

    // Visual effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
    UMaterialParameterCollection* MutationMaterialCollection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
    TMap<FString, UMaterialInterface*> MutationMaterials;

    // Audio effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effects")
    TMap<FString, USoundBase*> MutationSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effects")
    TArray<FString> WhisperTexts; // Possible whisper texts

    // Timer handles
    FTimerHandle HallucinationTimer;
    FTimerHandle WhisperTimer;

private:
    // Helper methods
    void LoadDefaultMutations();
    void LoadRedemptionPaths();
    void CheckForMutationTriggers();
    void ApplyMutationEffects(const FMutationData& Mutation);
    void RemoveMutationEffects(const FMutationData& Mutation);
    FString GenerateHallucinationText(const FString& TriggerContext);
    void PlayWhisper();
    void EndHallucination();
    FString GenerateMutationID();
    FString GenerateEventID();

public:
    /**
     * Blueprint implementable events for custom mutation logic
     */

    /**
     * Called when corruption changes (for custom effects)
     * @param OldCorruption Previous corruption level
     * @param NewCorruption New corruption level
     * @param Source Source of corruption change
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Mutation Events")
    void OnCorruptionChangedEvent(int32 OldCorruption, int32 NewCorruption, ECorruptionSource Source);

    /**
     * Called when mutation is triggered (for custom visual effects)
     * @param Mutation The triggered mutation
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Mutation Events")
    void OnMutationTriggeredEvent(const FMutationData& Mutation);

    /**
     * Called when hallucination starts (for custom effects)
     * @param Hallucination The hallucination event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Mutation Events")
    void OnHallucinationStartedEvent(const FHallucinationEvent& Hallucination);

    /**
     * Called to generate custom hallucination (override in Blueprint)
     * @param TriggerContext What triggered the hallucination
     * @param CorruptionLevel Current corruption level
     * @return Custom hallucination text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Mutation Events")
    FString GenerateCustomHallucination(const FString& TriggerContext, int32 CorruptionLevel);

    /**
     * Called to apply custom mutation effects (override in Blueprint)
     * @param Mutation The mutation being applied
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Mutation Events")
    void ApplyCustomMutationEffects(const FMutationData& Mutation);
};
