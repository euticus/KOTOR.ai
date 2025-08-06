// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Animation/AnimationTagMap.h"
#include "VOPerformanceIntegrationComponent.generated.h"

/**
 * VO performance sync mode
 */
UENUM(BlueprintType)
enum class EVOPerformanceSyncMode : uint8
{
    Automatic           UMETA(DisplayName = "Automatic Sync"),
    Manual              UMETA(DisplayName = "Manual Trigger"),
    Delayed             UMETA(DisplayName = "Delayed Start"),
    Overlapped          UMETA(DisplayName = "Overlapped"),
    Sequential          UMETA(DisplayName = "Sequential")
};

/**
 * VO performance data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FVOPerformanceData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    FString SpeakerName;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    FString DialogueLine;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    FPerformanceMetadata PerformanceMetadata;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    EVOPerformanceSyncMode SyncMode;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    float DelayTime; // Delay before starting (for Delayed mode)

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    float OverlapTime; // Overlap time (for Overlapped mode)

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Data")
    bool bWaitForVOCompletion; // Whether to wait for VO to complete

    FVOPerformanceData()
    {
        SpeakerName = TEXT("");
        DialogueLine = TEXT("");
        SyncMode = EVOPerformanceSyncMode::Automatic;
        DelayTime = 0.0f;
        OverlapTime = 0.5f;
        bWaitForVOCompletion = true;
    }
};

/**
 * VO performance result
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FVOPerformanceResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Result")
    bool bVOStarted;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Result")
    bool bPerformanceStarted;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Result")
    FString VORequestID;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Result")
    FString PerformanceID;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Result")
    float EstimatedDuration;

    UPROPERTY(BlueprintReadWrite, Category = "VO Performance Result")
    FString ErrorMessage;

    FVOPerformanceResult()
    {
        bVOStarted = false;
        bPerformanceStarted = false;
        VORequestID = TEXT("");
        PerformanceID = TEXT("");
        EstimatedDuration = 0.0f;
        ErrorMessage = TEXT("");
    }
};

/**
 * VO performance events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVOPerformanceStarted, const FVOPerformanceData&, VOData, const FVOPerformanceResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVOPerformanceCompleted, const FString&, SessionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVOPerformanceSynced, const FString&, VORequestID, const FString&, PerformanceID);

/**
 * VO Performance Integration Component - Sync voice playback with montage trigger
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UVOPerformanceIntegrationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVOPerformanceIntegrationComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize VO performance integration
     * @param VoiceComponent Voice synthesis component
     * @param PerformanceComponent Performance component
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    void InitializeVOPerformanceIntegration(UVoiceSynthesisComponent* VoiceComponent, 
                                           UProceduralPerformanceComponentV2* PerformanceComponent);

    /**
     * Play dialogue with performance
     * @param VOData VO performance data
     * @return VO performance result
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    FVOPerformanceResult PlayDialogueWithPerformance(const FVOPerformanceData& VOData);

    /**
     * Trigger dialogue with emotion and tone
     * @param SpeakerName Name of the speaker
     * @param DialogueLine Dialogue text
     * @param Emotion Performance emotion
     * @param Tone Performance tone
     * @param SyncMode Synchronization mode
     * @return VO performance result
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    FVOPerformanceResult TriggerDialogue(const FString& SpeakerName, const FString& DialogueLine, 
                                        EPerformanceEmotion Emotion, EPerformanceTone Tone,
                                        EVOPerformanceSyncMode SyncMode = EVOPerformanceSyncMode::Automatic);

    /**
     * Stop current VO and performance
     * @param bFadeOut Whether to fade out audio
     * @param FadeTime Fade out time
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    void StopVOAndPerformance(bool bFadeOut = true, float FadeTime = 0.5f);

    /**
     * Set sync mode
     * @param SyncMode New synchronization mode
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    void SetSyncMode(EVOPerformanceSyncMode SyncMode);

    /**
     * Check if VO and performance are active
     * @return True if both VO and performance are active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VO Performance Integration")
    bool IsVOAndPerformanceActive() const;

    /**
     * Get current VO performance data
     * @return Current VO performance data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VO Performance Integration")
    FVOPerformanceData GetCurrentVOPerformanceData() const { return CurrentVOData; }

    /**
     * Set performance delay
     * @param DelayTime Delay time in seconds
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    void SetPerformanceDelay(float DelayTime);

    /**
     * Set overlap time
     * @param OverlapTime Overlap time in seconds
     */
    UFUNCTION(BlueprintCallable, Category = "VO Performance Integration")
    void SetOverlapTime(float OverlapTime);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "VO Performance Events")
    FOnVOPerformanceStarted OnVOPerformanceStarted;

    UPROPERTY(BlueprintAssignable, Category = "VO Performance Events")
    FOnVOPerformanceCompleted OnVOPerformanceCompleted;

    UPROPERTY(BlueprintAssignable, Category = "VO Performance Events")
    FOnVOPerformanceSynced OnVOPerformanceSynced;

protected:
    // Current VO performance data
    UPROPERTY(BlueprintReadOnly, Category = "VO Performance State")
    FVOPerformanceData CurrentVOData;

    UPROPERTY(BlueprintReadOnly, Category = "VO Performance State")
    bool bVOActive;

    UPROPERTY(BlueprintReadOnly, Category = "VO Performance State")
    bool bPerformanceActive;

    UPROPERTY(BlueprintReadOnly, Category = "VO Performance State")
    FString CurrentSessionID;

    // Component references
    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    UPROPERTY()
    UProceduralPerformanceComponentV2* PerformanceComponent;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    EVOPerformanceSyncMode DefaultSyncMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    float DefaultDelayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    float DefaultOverlapTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration Settings")
    bool bAutoGenerateMetadata; // Auto-generate performance metadata from dialogue

    // Timer handles
    FTimerHandle DelayTimer;
    FTimerHandle OverlapTimer;

private:
    // Helper methods
    FString GenerateSessionID();
    FPerformanceMetadata GenerateMetadataFromDialogue(const FString& DialogueLine, EPerformanceEmotion Emotion, EPerformanceTone Tone);
    void StartVOPlayback(const FVOPerformanceData& VOData);
    void StartPerformance(const FVOPerformanceData& VOData);
    void HandleSyncMode(const FVOPerformanceData& VOData);

    // Timer callbacks
    UFUNCTION()
    void OnDelayTimer();

    UFUNCTION()
    void OnOverlapTimer();

    // Voice synthesis events
    UFUNCTION()
    void OnVoiceSynthesisStarted(const FString& RequestID, const FString& SpeakerName);

    UFUNCTION()
    void OnVoiceSynthesisCompleted(const FString& RequestID, USoundWave* GeneratedAudio);

    UFUNCTION()
    void OnVoiceSynthesisFailed(const FString& RequestID, const FString& ErrorMessage);

    // Performance events
    UFUNCTION()
    void OnPerformanceStarted(const FPerformanceMetadata& Metadata, const FPerformanceResult& Result);

    UFUNCTION()
    void OnPerformanceCompleted(const FString& PerformanceID);

public:
    /**
     * Blueprint implementable events for custom VO performance logic
     */

    /**
     * Called when VO performance starts (for custom setup)
     * @param VOData VO performance data
     * @param Result VO performance result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Performance Events")
    void OnVOPerformanceStartedEvent(const FVOPerformanceData& VOData, const FVOPerformanceResult& Result);

    /**
     * Called when VO and performance are synced (for custom effects)
     * @param VORequestID VO request ID
     * @param PerformanceID Performance ID
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Performance Events")
    void OnVOPerformanceSyncedEvent(const FString& VORequestID, const FString& PerformanceID);

    /**
     * Called to customize performance metadata (override in Blueprint)
     * @param DialogueLine Dialogue text
     * @param Emotion Performance emotion
     * @param Tone Performance tone
     * @return Custom performance metadata
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Performance Events")
    FPerformanceMetadata CustomizePerformanceMetadata(const FString& DialogueLine, EPerformanceEmotion Emotion, EPerformanceTone Tone);

    /**
     * Called to determine sync timing (override in Blueprint)
     * @param VOData VO performance data
     * @return Custom sync timing in seconds
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Performance Events")
    float DetermineSyncTiming(const FVOPerformanceData& VOData);
};
