// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MetasoundSource.h"
#include "MetasoundParameterTransmitter.h"
#include "Components/AudioComponent.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "MetaSoundMusicController.generated.h"

/**
 * MetaSound parameter types
 */
UENUM(BlueprintType)
enum class EMetaSoundParameterType : uint8
{
    Float               UMETA(DisplayName = "Float"),
    Int32               UMETA(DisplayName = "Int32"),
    Bool                UMETA(DisplayName = "Bool"),
    String              UMETA(DisplayName = "String"),
    Trigger             UMETA(DisplayName = "Trigger")
};

/**
 * MetaSound parameter data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMetaSoundParameterData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    FString ParameterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    EMetaSoundParameterType ParameterType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    float FloatValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    int32 IntValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    bool BoolValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    FString StringValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    bool bSmoothTransition; // Whether to smooth parameter changes

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Parameter")
    float TransitionTime; // Time for smooth transitions

    FMetaSoundParameterData()
    {
        ParameterName = TEXT("");
        ParameterType = EMetaSoundParameterType::Float;
        FloatValue = 0.0f;
        IntValue = 0;
        BoolValue = false;
        StringValue = TEXT("");
        bSmoothTransition = true;
        TransitionTime = 1.0f;
    }
};

/**
 * MetaSound music mapping
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMetaSoundMusicMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Music Mapping")
    FString MappingID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Music Mapping")
    EMusicBiome Biome;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Music Mapping")
    EMusicTone Tone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Music Mapping")
    bool bCombatMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Music Mapping")
    TArray<FMetaSoundParameterData> Parameters; // Parameters to set for this mapping

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Music Mapping")
    float TransitionTime; // Time to transition to this mapping

    FMetaSoundMusicMapping()
    {
        MappingID = TEXT("");
        Biome = EMusicBiome::Generic;
        Tone = EMusicTone::Neutral;
        bCombatMode = false;
        TransitionTime = 3.0f;
    }
};

/**
 * MetaSound events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMetaSoundParameterChanged, const FString&, ParameterName, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaSoundMappingApplied, const FString&, MappingID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMetaSoundTriggerFired, const FString&, TriggerName);

/**
 * MetaSound Music Controller - MetaSound graph with parameterized inputs for dynamic music control
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMetaSoundMusicController : public UActorComponent
{
    GENERATED_BODY()

public:
    UMetaSoundMusicController();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize MetaSound music controller
     * @param MetaSoundSource MetaSound source to control
     * @param MusicSubsystem Procedural music subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void InitializeMetaSoundController(UMetaSoundSource* MetaSoundSource, UProceduralMusicSubsystemV2* MusicSubsystem);

    /**
     * Set MetaSound parameter
     * @param ParameterName Name of parameter
     * @param Value Value to set
     * @param bSmoothTransition Whether to smooth the transition
     * @param TransitionTime Time for transition
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetMetaSoundParameter(const FString& ParameterName, float Value, bool bSmoothTransition = true, float TransitionTime = 1.0f);

    /**
     * Set MetaSound int parameter
     * @param ParameterName Name of parameter
     * @param Value Value to set
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetMetaSoundIntParameter(const FString& ParameterName, int32 Value);

    /**
     * Set MetaSound bool parameter
     * @param ParameterName Name of parameter
     * @param Value Value to set
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetMetaSoundBoolParameter(const FString& ParameterName, bool Value);

    /**
     * Set MetaSound string parameter
     * @param ParameterName Name of parameter
     * @param Value Value to set
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetMetaSoundStringParameter(const FString& ParameterName, const FString& Value);

    /**
     * Trigger MetaSound parameter
     * @param TriggerName Name of trigger parameter
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void TriggerMetaSoundParameter(const FString& TriggerName);

    /**
     * Apply MetaSound mapping
     * @param MappingID ID of mapping to apply
     * @return True if mapping was applied successfully
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    bool ApplyMetaSoundMapping(const FString& MappingID);

    /**
     * Add MetaSound mapping
     * @param Mapping Mapping to add
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void AddMetaSoundMapping(const FMetaSoundMusicMapping& Mapping);

    /**
     * Remove MetaSound mapping
     * @param MappingID ID of mapping to remove
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void RemoveMetaSoundMapping(const FString& MappingID);

    /**
     * Update for music state
     * @param Biome Current biome
     * @param Tone Current tone
     * @param bCombatMode Whether in combat mode
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void UpdateForMusicState(EMusicBiome Biome, EMusicTone Tone, bool bCombatMode);

    /**
     * Set biome parameter
     * @param Biome Biome to set
     * @param TransitionTime Time to transition
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetBiomeParameter(EMusicBiome Biome, float TransitionTime = 3.0f);

    /**
     * Set tone parameter
     * @param Tone Tone to set
     * @param TransitionTime Time to transition
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetToneParameter(EMusicTone Tone, float TransitionTime = 2.0f);

    /**
     * Set combat mode parameter
     * @param bCombatMode Whether in combat mode
     * @param TransitionTime Time to transition
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetCombatModeParameter(bool bCombatMode, float TransitionTime = 1.5f);

    /**
     * Set intensity parameter
     * @param Intensity Intensity value (0.0 to 1.0)
     * @param TransitionTime Time to transition
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetIntensityParameter(float Intensity, float TransitionTime = 1.0f);

    /**
     * Set tension parameter
     * @param Tension Tension value (0.0 to 1.0)
     * @param TransitionTime Time to transition
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void SetTensionParameter(float Tension, float TransitionTime = 1.0f);

    /**
     * Get current parameter value
     * @param ParameterName Name of parameter
     * @return Current parameter value
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaSound Music Controller")
    float GetCurrentParameterValue(const FString& ParameterName) const;

    /**
     * Check if MetaSound is playing
     * @return True if MetaSound is currently playing
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaSound Music Controller")
    bool IsMetaSoundPlaying() const;

    /**
     * Start MetaSound playback
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void StartMetaSound();

    /**
     * Stop MetaSound playback
     * @param FadeOutTime Time to fade out
     */
    UFUNCTION(BlueprintCallable, Category = "MetaSound Music Controller")
    void StopMetaSound(float FadeOutTime = 3.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "MetaSound Events")
    FOnMetaSoundParameterChanged OnMetaSoundParameterChanged;

    UPROPERTY(BlueprintAssignable, Category = "MetaSound Events")
    FOnMetaSoundMappingApplied OnMetaSoundMappingApplied;

    UPROPERTY(BlueprintAssignable, Category = "MetaSound Events")
    FOnMetaSoundTriggerFired OnMetaSoundTriggerFired;

protected:
    // MetaSound mappings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Mappings")
    TMap<FString, FMetaSoundMusicMapping> MetaSoundMappings;

    // Current parameter values
    UPROPERTY(BlueprintReadOnly, Category = "Parameter Values")
    TMap<FString, float> CurrentParameterValues;

    // Component references
    UPROPERTY()
    UMetaSoundSource* MetaSoundSource;

    UPROPERTY()
    UAudioComponent* AudioComponent;

    UPROPERTY()
    UProceduralMusicSubsystemV2* ProceduralMusicSubsystem;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Settings")
    bool bMetaSoundEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Settings")
    float DefaultTransitionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaSound Settings")
    float MasterVolume;

    // Parameter smoothing
    UPROPERTY(BlueprintReadOnly, Category = "Parameter Smoothing")
    TMap<FString, float> TargetParameterValues;

    UPROPERTY(BlueprintReadOnly, Category = "Parameter Smoothing")
    TMap<FString, float> ParameterTransitionTimes;

    UPROPERTY(BlueprintReadOnly, Category = "Parameter Smoothing")
    TMap<FString, float> ParameterTransitionStartTimes;

private:
    // Helper methods
    void LoadDefaultMappings();
    void UpdateParameterSmoothing(float DeltaTime);
    void SetParameterInternal(const FString& ParameterName, float Value);
    float BiomeToFloat(EMusicBiome Biome) const;
    float ToneToFloat(EMusicTone Tone) const;
    FMetaSoundMusicMapping* FindMappingForState(EMusicBiome Biome, EMusicTone Tone, bool bCombatMode);

public:
    /**
     * Blueprint implementable events for custom MetaSound logic
     */

    /**
     * Called when MetaSound parameter changes (for custom processing)
     * @param ParameterName Name of parameter
     * @param OldValue Previous value
     * @param NewValue New value
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MetaSound Events")
    void OnMetaSoundParameterChangedEvent(const FString& ParameterName, float OldValue, float NewValue);

    /**
     * Called when MetaSound mapping is applied (for custom effects)
     * @param MappingID ID of applied mapping
     * @param Mapping The mapping data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MetaSound Events")
    void OnMetaSoundMappingAppliedEvent(const FString& MappingID, const FMetaSoundMusicMapping& Mapping);

    /**
     * Called to customize parameter value (override in Blueprint)
     * @param ParameterName Name of parameter
     * @param RequestedValue Requested value
     * @param CurrentState Current music state
     * @return Custom parameter value
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MetaSound Events")
    float CustomizeMetaSoundParameter(const FString& ParameterName, float RequestedValue, const FString& CurrentState);

    /**
     * Called to validate MetaSound mapping (override in Blueprint)
     * @param Mapping Mapping to validate
     * @return True if mapping is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "MetaSound Events")
    bool ValidateMetaSoundMapping(const FMetaSoundMusicMapping& Mapping);
};
