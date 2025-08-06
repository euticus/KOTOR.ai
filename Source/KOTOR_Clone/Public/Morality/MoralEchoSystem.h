// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Character/MutationManagerComponent.h"
#include "Simulation/WorldStateSimulator.h"
#include "MoralEchoSystem.generated.h"

/**
 * Echo effect types
 */
UENUM(BlueprintType)
enum class EEchoEffectType : uint8
{
    Environmental       UMETA(DisplayName = "Environmental"),
    Lighting            UMETA(DisplayName = "Lighting"),
    Weather             UMETA(DisplayName = "Weather"),
    NPCBehavior         UMETA(DisplayName = "NPC Behavior"),
    EnemyEvolution      UMETA(DisplayName = "Enemy Evolution"),
    Hallucination       UMETA(DisplayName = "Hallucination"),
    GhostNPC            UMETA(DisplayName = "Ghost NPC"),
    VoiceEcho           UMETA(DisplayName = "Voice Echo"),
    VisualDistortion    UMETA(DisplayName = "Visual Distortion"),
    Atmospheric         UMETA(DisplayName = "Atmospheric")
};

/**
 * Moral alignment spectrum
 */
UENUM(BlueprintType)
enum class EMoralAlignment : uint8
{
    PureLight           UMETA(DisplayName = "Pure Light"),
    Light               UMETA(DisplayName = "Light"),
    LightNeutral        UMETA(DisplayName = "Light Neutral"),
    TrueNeutral         UMETA(DisplayName = "True Neutral"),
    DarkNeutral         UMETA(DisplayName = "Dark Neutral"),
    Dark                UMETA(DisplayName = "Dark"),
    PureDark            UMETA(DisplayName = "Pure Dark")
};

/**
 * Echo effect data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FEchoEffect
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    FString EffectID;

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    EEchoEffectType EffectType;

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    FString EffectName;

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    EMoralAlignment TriggerAlignment; // Alignment that triggers this effect

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    float IntensityThreshold; // Minimum intensity to trigger

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    float CurrentIntensity; // Current effect intensity

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    float MaxIntensity; // Maximum possible intensity

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    TMap<FString, float> EffectParameters; // Effect-specific parameters

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    TArray<FString> AffectedLocations; // Locations where effect applies

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Echo Effect")
    float ActivationTime; // When effect was activated

    FEchoEffect()
    {
        EffectID = TEXT("");
        EffectType = EEchoEffectType::Environmental;
        EffectName = TEXT("Echo Effect");
        Description = TEXT("");
        TriggerAlignment = EMoralAlignment::TrueNeutral;
        IntensityThreshold = 0.1f;
        CurrentIntensity = 0.0f;
        MaxIntensity = 1.0f;
        bIsActive = false;
        ActivationTime = 0.0f;
    }
};

/**
 * Moral echo data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMoralEcho
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    FString EchoID;

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    FString TriggerEvent; // Event that created this echo

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    EMoralAlignment Alignment;

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    float Intensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    FVector Location; // Where echo occurred

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    float Radius; // Radius of echo influence

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    float CreationTime; // When echo was created

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    float DecayRate; // How fast echo fades

    UPROPERTY(BlueprintReadWrite, Category = "Moral Echo")
    TArray<FString> EchoMessages; // Messages from past self

    FMoralEcho()
    {
        EchoID = TEXT("");
        TriggerEvent = TEXT("");
        Alignment = EMoralAlignment::TrueNeutral;
        Intensity = 0.5f;
        Location = FVector::ZeroVector;
        Radius = 1000.0f;
        CreationTime = 0.0f;
        DecayRate = 0.01f;
    }
};

/**
 * Echo events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEchoEffectTriggered, const FEchoEffect&, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoralEchoCreated, const FMoralEcho&, Echo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEchoVolumeTriggered, const FString&, EchoMessage, EMoralAlignment, Alignment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnvironmentalShift, EEchoEffectType, EffectType, float, Intensity);

/**
 * Echo Scene Trigger Volume - Triggers echo effects in specific areas
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AEchoSceneTriggerVolume : public ATriggerVolume
{
    GENERATED_BODY()

public:
    AEchoSceneTriggerVolume();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Set echo trigger data
     * @param EchoData Echo data for this volume
     */
    UFUNCTION(BlueprintCallable, Category = "Echo Volume")
    void SetEchoTriggerData(const FMoralEcho& EchoData);

    /**
     * Get echo trigger data
     * @return Echo data for this volume
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Echo Volume")
    FMoralEcho GetEchoTriggerData() const { return EchoData; }

protected:
    // Echo data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Volume")
    FMoralEcho EchoData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Volume")
    float TriggerProbability; // Chance of triggering when entered

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Volume")
    float CooldownTime; // Time between triggers

    UPROPERTY()
    float LastTriggerTime;

    // Overlap events
    UFUNCTION()
    void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when echo volume is triggered
     * @param TriggeringActor Actor that triggered the volume
     * @param EchoMessage Message from the echo
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Echo Volume Events")
    void OnEchoVolumeTriggered(AActor* TriggeringActor, const FString& EchoMessage);

    /**
     * Called to generate custom echo message
     * @param TriggeringActor Actor that triggered the volume
     * @return Custom echo message
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Echo Volume Events")
    FString GenerateCustomEchoMessage(AActor* TriggeringActor);
};

/**
 * Moral Echo System - Galaxy reshapes based on player morality
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMoralEchoSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UMoralEchoSystem();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize moral echo system
     * @param NarrativeMemory Narrative memory for moral choices
     * @param MutationManager Mutation manager for corruption tracking
     * @param WorldSimulator World simulator for environmental effects
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    void InitializeMoralEchoSystem(UNarrativeMemoryComponent* NarrativeMemory,
                                  UMutationManagerComponent* MutationManager,
                                  UWorldStateSimulator* WorldSimulator);

    /**
     * Create moral echo from event
     * @param TriggerEvent Event that created the echo
     * @param Alignment Moral alignment of the event
     * @param Intensity Intensity of the moral choice
     * @param Location Where the event occurred
     * @return Created moral echo
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    FMoralEcho CreateMoralEcho(const FString& TriggerEvent, EMoralAlignment Alignment, 
                              float Intensity, const FVector& Location);

    /**
     * Apply environmental shift
     * @param EffectType Type of environmental effect
     * @param Alignment Moral alignment driving the shift
     * @param Intensity Intensity of the shift
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    void ApplyEnvironmentalShift(EEchoEffectType EffectType, EMoralAlignment Alignment, float Intensity);

    /**
     * Spawn echo trigger volume
     * @param EchoData Echo data for the volume
     * @param Location Where to spawn the volume
     * @return Spawned echo volume
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    AEchoSceneTriggerVolume* SpawnEchoTriggerVolume(const FMoralEcho& EchoData, const FVector& Location);

    /**
     * Update global moral alignment
     * @param AlignmentChange Change in alignment
     * @param TriggerEvent Event that caused the change
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    void UpdateGlobalMoralAlignment(float AlignmentChange, const FString& TriggerEvent);

    /**
     * Get current global alignment
     * @return Current global moral alignment
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Moral Echo")
    EMoralAlignment GetCurrentGlobalAlignment() const;

    /**
     * Get global alignment value
     * @return Global alignment value (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Moral Echo")
    float GetGlobalAlignmentValue() const { return GlobalAlignmentValue; }

    /**
     * Get active echo effects
     * @return Array of currently active echo effects
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Moral Echo")
    TArray<FEchoEffect> GetActiveEchoEffects() const;

    /**
     * Get moral echoes in radius
     * @param Location Center location
     * @param Radius Search radius
     * @return Array of moral echoes in radius
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Moral Echo")
    TArray<FMoralEcho> GetMoralEchoesInRadius(const FVector& Location, float Radius) const;

    /**
     * Set echo intensity multiplier
     * @param Multiplier Multiplier for echo effects (1.0 = normal)
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    void SetEchoIntensityMultiplier(float Multiplier);

    /**
     * Enable/disable echo effects
     * @param bEnabled Whether to enable echo effects
     */
    UFUNCTION(BlueprintCallable, Category = "Moral Echo")
    void SetEchoEffectsEnabled(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Moral Echo Events")
    FOnEchoEffectTriggered OnEchoEffectTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Moral Echo Events")
    FOnMoralEchoCreated OnMoralEchoCreated;

    UPROPERTY(BlueprintAssignable, Category = "Moral Echo Events")
    FOnEchoVolumeTriggered OnEchoVolumeTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Moral Echo Events")
    FOnEnvironmentalShift OnEnvironmentalShift;

protected:
    // Echo data
    UPROPERTY(BlueprintReadOnly, Category = "Moral Echo")
    TArray<FMoralEcho> MoralEchoes;

    UPROPERTY(BlueprintReadOnly, Category = "Moral Echo")
    TArray<FEchoEffect> ActiveEchoEffects;

    UPROPERTY(BlueprintReadOnly, Category = "Moral Echo")
    TArray<AEchoSceneTriggerVolume*> EchoVolumes;

    // Global alignment tracking
    UPROPERTY(BlueprintReadOnly, Category = "Moral Echo")
    float GlobalAlignmentValue; // -1.0 (pure dark) to 1.0 (pure light)

    UPROPERTY(BlueprintReadOnly, Category = "Moral Echo")
    TArray<float> AlignmentHistory; // Recent alignment values

    // Component references
    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UMutationManagerComponent* MutationManagerRef;

    UPROPERTY()
    UWorldStateSimulator* WorldSimulatorRef;

    // Echo settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Settings")
    bool bEchoEffectsEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Settings")
    float EchoIntensityMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Settings")
    float EchoDecayRate; // How fast echoes fade

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo Settings")
    int32 MaxActiveEchoes; // Maximum number of active echoes

    // Effect templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Templates")
    TMap<EMoralAlignment, TArray<FEchoEffect>> AlignmentEffectTemplates;

private:
    // Helper methods
    FString GenerateEchoID();
    FString GenerateEffectID();
    EMoralAlignment CalculateGlobalAlignment();
    void ProcessEchoDecay();
    void UpdateEnvironmentalEffects();
    void TriggerEchoEffect(const FEchoEffect& Effect);
    void CleanupOldEchoes();
    FEchoEffect* FindActiveEffect(EEchoEffectType EffectType);

    // Event handlers
    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

    UFUNCTION()
    void OnCorruptionChanged(int32 OldCorruption, int32 NewCorruption);

public:
    /**
     * Blueprint implementable events for custom echo logic
     */

    /**
     * Called when echo effect is triggered (for custom effects)
     * @param Effect The triggered echo effect
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Moral Echo Events")
    void OnEchoEffectTriggeredEvent(const FEchoEffect& Effect);

    /**
     * Called when moral echo is created (for custom processing)
     * @param Echo The created moral echo
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Moral Echo Events")
    void OnMoralEchoCreatedEvent(const FMoralEcho& Echo);

    /**
     * Called to apply custom environmental effects (override in Blueprint)
     * @param EffectType Type of effect to apply
     * @param Alignment Moral alignment
     * @param Intensity Effect intensity
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Moral Echo Events")
    void ApplyCustomEnvironmentalEffects(EEchoEffectType EffectType, EMoralAlignment Alignment, float Intensity);

    /**
     * Called to generate custom echo message (override in Blueprint)
     * @param Echo The moral echo
     * @param TriggeringActor Actor that triggered the echo
     * @return Custom echo message
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Moral Echo Events")
    FString GenerateCustomEchoMessage(const FMoralEcho& Echo, AActor* TriggeringActor);
};
