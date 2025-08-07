// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "ProceduralPerformanceComponentV2.generated.h"

/**
 * Performance emotion types
 */
UENUM(BlueprintType)
enum class EPerformanceEmotionV2 : uint8
{
    Neutral             UMETA(DisplayName = "Neutral"),
    Happy               UMETA(DisplayName = "Happy"),
    Sad                 UMETA(DisplayName = "Sad"),
    Angry               UMETA(DisplayName = "Angry"),
    Fear                UMETA(DisplayName = "Fear"),
    Surprise            UMETA(DisplayName = "Surprise"),
    Disgust             UMETA(DisplayName = "Disgust"),
    Contempt            UMETA(DisplayName = "Contempt"),
    Pride               UMETA(DisplayName = "Pride"),
    Shame               UMETA(DisplayName = "Shame"),
    Hope                UMETA(DisplayName = "Hope"),
    Despair             UMETA(DisplayName = "Despair"),
    Love                UMETA(DisplayName = "Love"),
    Hate                UMETA(DisplayName = "Hate"),
    Determination       UMETA(DisplayName = "Determination"),
    Confusion           UMETA(DisplayName = "Confusion")
};

/**
 * Performance tone types
 */
UENUM(BlueprintType)
enum class EPerformanceTone : uint8
{
    Normal              UMETA(DisplayName = "Normal"),
    Whisper             UMETA(DisplayName = "Whisper"),
    Shout               UMETA(DisplayName = "Shout"),
    Mocking             UMETA(DisplayName = "Mocking"),
    Urgent              UMETA(DisplayName = "Urgent"),
    Threatening         UMETA(DisplayName = "Threatening"),
    Pleading            UMETA(DisplayName = "Pleading"),
    Commanding          UMETA(DisplayName = "Commanding"),
    Questioning         UMETA(DisplayName = "Questioning"),
    Storytelling        UMETA(DisplayName = "Storytelling"),
    Mysterious          UMETA(DisplayName = "Mysterious"),
    Seductive           UMETA(DisplayName = "Seductive"),
    Wise                UMETA(DisplayName = "Wise"),
    Childlike           UMETA(DisplayName = "Childlike"),
    Elderly             UMETA(DisplayName = "Elderly"),
    Robotic             UMETA(DisplayName = "Robotic")
};

/**
 * Performance metadata structure
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPerformanceMetadata
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    EPerformanceEmotion Emotion;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    EPerformanceTone Tone;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    FString DialogueLine;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    float IntensityLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    float Duration; // Duration of the performance

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    FString SpeakerName; // Name of the speaking character

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    bool bSyncWithVoice; // Whether to sync with voice synthesis

    UPROPERTY(BlueprintReadWrite, Category = "Performance Metadata")
    TMap<FString, FString> CustomTags; // Additional metadata

    FPerformanceMetadata()
    {
        Emotion = EPerformanceEmotion::Neutral;
        Tone = EPerformanceTone::Normal;
        DialogueLine = TEXT("");
        IntensityLevel = 0.5f;
        Duration = 3.0f;
        SpeakerName = TEXT("");
        bSyncWithVoice = true;
    }
};

/**
 * Performance result data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPerformanceResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Performance Result")
    bool bSuccess;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Result")
    UAnimMontage* SelectedMontage;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Result")
    FString PerformanceID;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Result")
    float ActualDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Result")
    FString ErrorMessage;

    FPerformanceResult()
    {
        bSuccess = false;
        SelectedMontage = nullptr;
        PerformanceID = TEXT("");
        ActualDuration = 0.0f;
        ErrorMessage = TEXT("");
    }
};

/**
 * Performance events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerformanceStarted, const FPerformanceMetadata&, Metadata, const FPerformanceResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceCompleted, const FString&, PerformanceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMontageSelected, UAnimMontage*, Montage, const FPerformanceMetadata&, Metadata);

/**
 * Procedural Performance Component V2 - Enhanced performance system with emotion, tone, and line metadata
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UProceduralPerformanceComponentV2 : public UActorComponent
{
    GENERATED_BODY()

public:
    UProceduralPerformanceComponentV2();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize performance component
     * @param SkeletalMesh Skeletal mesh component for animations
     * @param VoiceComponent Voice synthesis component for sync (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void InitializePerformance(USkeletalMeshComponent* SkeletalMesh, UVoiceSynthesisComponent* VoiceComponent = nullptr);

    /**
     * Play performance based on metadata
     * @param Metadata Performance metadata containing emotion, tone, and line
     * @return Performance result with success status and selected montage
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    FPerformanceResult PlayPerformance(const FPerformanceMetadata& Metadata);

    /**
     * Stop current performance
     * @param BlendOutTime Time to blend out of current animation
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void StopPerformance(float BlendOutTime = 0.25f);

    /**
     * Set animation tag map
     * @param TagMap Animation tag map object
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void SetAnimationTagMap(class UAnimationTagMap* TagMap);

    /**
     * Get montage for metadata
     * @param Metadata Performance metadata to analyze
     * @return Selected animation montage
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    UAnimMontage* GetMontageForMetadata(const FPerformanceMetadata& Metadata) const;

    /**
     * Check if performance is active
     * @return True if performance is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Performance")
    bool IsPerformanceActive() const { return bPerformanceActive; }

    /**
     * Get current performance metadata
     * @return Current performance metadata
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Performance")
    FPerformanceMetadata GetCurrentMetadata() const { return CurrentMetadata; }

    /**
     * Get current performance ID
     * @return Current performance ID
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Performance")
    FString GetCurrentPerformanceID() const { return CurrentPerformanceID; }

    /**
     * Set performance intensity
     * @param Intensity Intensity from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void SetPerformanceIntensity(float Intensity);

    /**
     * Test performance with specific parameters
     * @param Emotion Emotion to test
     * @param Tone Tone to test
     * @param TestLine Test dialogue line
     * @return Performance result
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    FPerformanceResult TestPerformance(EPerformanceEmotion Emotion, EPerformanceTone Tone, const FString& TestLine);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceStarted OnPerformanceStarted;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceCompleted OnPerformanceCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnMontageSelected OnMontageSelected;

protected:
    // Current performance state
    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    FPerformanceMetadata CurrentMetadata;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    bool bPerformanceActive;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    FString CurrentPerformanceID;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    float PerformanceStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    UAnimMontage* CurrentMontage;

    // Component references
    UPROPERTY()
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    UPROPERTY()
    class UAnimationTagMap* AnimationTagMap;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    bool bAutoSyncWithVoice; // Automatically sync with voice synthesis

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    float DefaultBlendInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    float DefaultBlendOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    bool bEnablePerformanceLogging; // Log performance events

    // Timer handle
    FTimerHandle PerformanceTimer;

private:
    // Helper methods
    FString GeneratePerformanceID();
    FString GenerateAnimationTag(const FPerformanceMetadata& Metadata) const;
    void PlayMontage(UAnimMontage* Montage, const FPerformanceMetadata& Metadata);
    void OnPerformanceTimerComplete();
    void LogPerformanceEvent(const FString& Event, const FPerformanceMetadata& Metadata);

    // Animation callbacks
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // Voice synthesis events
    UFUNCTION()
    void OnVoiceSynthesisStarted(const FString& SpeakerName, const FString& DialogueLine);

    UFUNCTION()
    void OnVoiceSynthesisCompleted(const FString& SpeakerName);

public:
    /**
     * Blueprint implementable events for custom performance logic
     */

    /**
     * Called when performance starts (for custom setup)
     * @param Metadata The performance metadata
     * @param Result The performance result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    void OnPerformanceStartedEvent(const FPerformanceMetadata& Metadata, const FPerformanceResult& Result);

    /**
     * Called when montage is selected (for custom processing)
     * @param Montage The selected animation montage
     * @param Metadata The performance metadata
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    void OnMontageSelectedEvent(UAnimMontage* Montage, const FPerformanceMetadata& Metadata);

    /**
     * Called to customize montage selection (override in Blueprint)
     * @param Metadata Performance metadata
     * @param DefaultMontage Default selected montage
     * @return Custom montage selection
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    UAnimMontage* CustomizeMontageSelection(const FPerformanceMetadata& Metadata, UAnimMontage* DefaultMontage);

    /**
     * Called to validate performance metadata (override in Blueprint)
     * @param Metadata Metadata to validate
     * @return True if metadata is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    bool ValidatePerformanceMetadata(const FPerformanceMetadata& Metadata);
};
