// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "MetasoundSource.h"
#include "Audio/ProceduralMusicSubsystem.h"
#include "MusicBlendingComponent.generated.h"

/**
 * Blend curve types
 */
UENUM(BlueprintType)
enum class EBlendCurveType : uint8
{
    Linear              UMETA(DisplayName = "Linear"),
    EaseIn              UMETA(DisplayName = "Ease In"),
    EaseOut             UMETA(DisplayName = "Ease Out"),
    EaseInOut           UMETA(DisplayName = "Ease In Out"),
    Exponential         UMETA(DisplayName = "Exponential"),
    Logarithmic         UMETA(DisplayName = "Logarithmic"),
    Sine                UMETA(DisplayName = "Sine"),
    Custom              UMETA(DisplayName = "Custom Curve")
};

/**
 * Blend synchronization types
 */
UENUM(BlueprintType)
enum class EBlendSyncType : uint8
{
    Immediate           UMETA(DisplayName = "Immediate"),
    NextBeat            UMETA(DisplayName = "Next Beat"),
    NextMeasure         UMETA(DisplayName = "Next Measure"),
    NextPhrase          UMETA(DisplayName = "Next Phrase"),
    NextSection         UMETA(DisplayName = "Next Section"),
    FadePoint           UMETA(DisplayName = "Fade Point"),
    Custom              UMETA(DisplayName = "Custom Timing")
};

/**
 * Audio layer blend data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAudioLayerBlend
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    FString LayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    UAudioComponent* AudioComponent;

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    float CurrentVolume; // Current volume (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    float TargetVolume; // Target volume (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    float BlendStartTime; // When blend started

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    float BlendDuration; // How long blend takes

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    EBlendCurveType BlendCurve; // Curve type for blending

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    EBlendSyncType SyncType; // Synchronization type

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    bool bIsBlending; // Whether currently blending

    UPROPERTY(BlueprintReadWrite, Category = "Audio Layer Blend")
    bool bFadeOutWhenComplete; // Fade out when target reached

    FAudioLayerBlend()
    {
        LayerID = TEXT("");
        AudioComponent = nullptr;
        CurrentVolume = 0.0f;
        TargetVolume = 0.0f;
        BlendStartTime = 0.0f;
        BlendDuration = 2.0f;
        BlendCurve = EBlendCurveType::EaseInOut;
        SyncType = EBlendSyncType::Immediate;
        bIsBlending = false;
        bFadeOutWhenComplete = false;
    }
};

/**
 * Music timing data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicTimingData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Timing")
    float BPM; // Beats per minute

    UPROPERTY(BlueprintReadWrite, Category = "Music Timing")
    int32 BeatsPerMeasure; // Time signature numerator

    UPROPERTY(BlueprintReadWrite, Category = "Music Timing")
    int32 BeatUnit; // Time signature denominator

    UPROPERTY(BlueprintReadWrite, Category = "Music Timing")
    float CurrentBeat; // Current beat position

    UPROPERTY(BlueprintReadWrite, Category = "Music Timing")
    int32 CurrentMeasure; // Current measure number

    UPROPERTY(BlueprintReadWrite, Category = "Music Timing")
    float PlaybackTime; // Total playback time

    FMusicTimingData()
    {
        BPM = 120.0f;
        BeatsPerMeasure = 4;
        BeatUnit = 4;
        CurrentBeat = 0.0f;
        CurrentMeasure = 0;
        PlaybackTime = 0.0f;
    }
};

/**
 * Blend events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLayerBlendStarted, const FString&, LayerID, float, BlendDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLayerBlendCompleted, const FString&, LayerID, float, FinalVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicBeatChanged, float, CurrentBeat);

/**
 * Music Blending Component - Lets music change without hard cuts using smooth transitions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMusicBlendingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMusicBlendingComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize music blending
     * @param MusicSubsystem Procedural music subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void InitializeMusicBlending(UProceduralMusicSubsystem* MusicSubsystem);

    /**
     * Start layer blend
     * @param LayerID ID of layer to blend
     * @param TargetVolume Target volume (0.0 to 1.0)
     * @param BlendDuration Duration of blend in seconds
     * @param BlendCurve Curve type for blending
     * @param SyncType Synchronization type
     * @return True if blend started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    bool StartLayerBlend(const FString& LayerID, float TargetVolume, float BlendDuration = 2.0f, 
                        EBlendCurveType BlendCurve = EBlendCurveType::EaseInOut, 
                        EBlendSyncType SyncType = EBlendSyncType::Immediate);

    /**
     * Stop layer blend
     * @param LayerID ID of layer to stop blending
     * @param bSnapToTarget Whether to snap to target volume immediately
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void StopLayerBlend(const FString& LayerID, bool bSnapToTarget = false);

    /**
     * Set layer volume immediately
     * @param LayerID ID of layer
     * @param Volume Volume to set (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void SetLayerVolumeImmediate(const FString& LayerID, float Volume);

    /**
     * Fade in layer
     * @param LayerID ID of layer to fade in
     * @param FadeTime Duration of fade in seconds
     * @param TargetVolume Target volume (default 1.0)
     * @param SyncType Synchronization type
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void FadeInLayer(const FString& LayerID, float FadeTime = 2.0f, float TargetVolume = 1.0f, 
                    EBlendSyncType SyncType = EBlendSyncType::NextMeasure);

    /**
     * Fade out layer
     * @param LayerID ID of layer to fade out
     * @param FadeTime Duration of fade in seconds
     * @param SyncType Synchronization type
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void FadeOutLayer(const FString& LayerID, float FadeTime = 2.0f, 
                     EBlendSyncType SyncType = EBlendSyncType::NextMeasure);

    /**
     * Crossfade between layers
     * @param FadeOutLayerID Layer to fade out
     * @param FadeInLayerID Layer to fade in
     * @param CrossfadeTime Duration of crossfade
     * @param SyncType Synchronization type
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void CrossfadeLayers(const FString& FadeOutLayerID, const FString& FadeInLayerID, 
                        float CrossfadeTime = 3.0f, EBlendSyncType SyncType = EBlendSyncType::NextMeasure);

    /**
     * Set music timing
     * @param TimingData Music timing data
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void SetMusicTiming(const FMusicTimingData& TimingData);

    /**
     * Get current music timing
     * @return Current music timing data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Blending")
    FMusicTimingData GetCurrentMusicTiming() const { return CurrentTiming; }

    /**
     * Get layer current volume
     * @param LayerID ID of layer
     * @return Current volume of layer
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Blending")
    float GetLayerCurrentVolume(const FString& LayerID) const;

    /**
     * Check if layer is blending
     * @param LayerID ID of layer to check
     * @return True if layer is currently blending
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Blending")
    bool IsLayerBlending(const FString& LayerID) const;

    /**
     * Get active blends
     * @return Array of currently active blend layer IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music Blending")
    TArray<FString> GetActiveBlends() const;

    /**
     * Stop all blends
     * @param bSnapToTargets Whether to snap all layers to their target volumes
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void StopAllBlends(bool bSnapToTargets = false);

    /**
     * Set master volume
     * @param Volume Master volume (0.0 to 1.0)
     * @param BlendTime Time to blend to new volume
     */
    UFUNCTION(BlueprintCallable, Category = "Music Blending")
    void SetMasterVolume(float Volume, float BlendTime = 1.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Music Blending Events")
    FOnLayerBlendStarted OnLayerBlendStarted;

    UPROPERTY(BlueprintAssignable, Category = "Music Blending Events")
    FOnLayerBlendCompleted OnLayerBlendCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Music Blending Events")
    FOnMusicBeatChanged OnMusicBeatChanged;

protected:
    // Active layer blends
    UPROPERTY(BlueprintReadOnly, Category = "Layer Blends")
    TMap<FString, FAudioLayerBlend> ActiveBlends;

    // Music timing
    UPROPERTY(BlueprintReadOnly, Category = "Music Timing")
    FMusicTimingData CurrentTiming;

    UPROPERTY(BlueprintReadOnly, Category = "Music Timing")
    float LastBeatTime;

    // Component reference
    UPROPERTY()
    UProceduralMusicSubsystem* ProceduralMusicSubsystem;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blending Settings")
    float MasterVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blending Settings")
    bool bEnableMusicalSynchronization; // Whether to sync to musical timing

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blending Settings")
    float BeatTolerance; // Tolerance for beat synchronization

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blending Settings")
    UCurveFloat* CustomBlendCurve; // Custom blend curve

    // Master volume blend
    UPROPERTY(BlueprintReadOnly, Category = "Master Volume")
    float CurrentMasterVolume;

    UPROPERTY(BlueprintReadOnly, Category = "Master Volume")
    float TargetMasterVolume;

    UPROPERTY(BlueprintReadOnly, Category = "Master Volume")
    bool bMasterVolumeBlending;

    UPROPERTY(BlueprintReadOnly, Category = "Master Volume")
    float MasterVolumeBlendStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Master Volume")
    float MasterVolumeBlendDuration;

private:
    // Helper methods
    void UpdateLayerBlends(float DeltaTime);
    void UpdateMusicTiming(float DeltaTime);
    void UpdateMasterVolumeBlend(float DeltaTime);
    float CalculateBlendValue(float Alpha, EBlendCurveType CurveType);
    float GetNextSyncTime(EBlendSyncType SyncType);
    bool ShouldStartBlend(const FAudioLayerBlend& Blend);
    void CompleteLayerBlend(const FString& LayerID);

    // Curve calculations
    float CalculateLinearCurve(float Alpha);
    float CalculateEaseInCurve(float Alpha);
    float CalculateEaseOutCurve(float Alpha);
    float CalculateEaseInOutCurve(float Alpha);
    float CalculateExponentialCurve(float Alpha);
    float CalculateLogarithmicCurve(float Alpha);
    float CalculateSineCurve(float Alpha);
    float CalculateCustomCurve(float Alpha);

    // Timing calculations
    float GetSecondsPerBeat();
    float GetSecondsPerMeasure();
    float GetTimeToNextBeat();
    float GetTimeToNextMeasure();

public:
    /**
     * Blueprint implementable events for custom music blending logic
     */

    /**
     * Called when layer blend starts (for custom processing)
     * @param LayerID ID of blending layer
     * @param BlendDuration Duration of the blend
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Blending Events")
    void OnLayerBlendStartedEvent(const FString& LayerID, float BlendDuration);

    /**
     * Called when layer blend completes (for custom processing)
     * @param LayerID ID of completed layer
     * @param FinalVolume Final volume reached
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Blending Events")
    void OnLayerBlendCompletedEvent(const FString& LayerID, float FinalVolume);

    /**
     * Called to customize blend curve (override in Blueprint)
     * @param LayerID ID of layer being blended
     * @param Alpha Blend alpha (0.0 to 1.0)
     * @param CurveType Curve type being used
     * @return Custom curve value
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Blending Events")
    float CustomizeBlendCurve(const FString& LayerID, float Alpha, EBlendCurveType CurveType);

    /**
     * Called to determine sync timing (override in Blueprint)
     * @param LayerID ID of layer
     * @param SyncType Sync type requested
     * @return Custom sync time in seconds
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Blending Events")
    float DetermineCustomSyncTime(const FString& LayerID, EBlendSyncType SyncType);
};
