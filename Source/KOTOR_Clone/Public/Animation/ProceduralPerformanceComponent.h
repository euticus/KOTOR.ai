// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "ProceduralPerformanceComponent.generated.h"

/**
 * Performance emotion types
 */
UENUM(BlueprintType)
enum class EPerformanceEmotion : uint8
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
 * Dialogue tone types
 */
UENUM(BlueprintType)
enum class EDialogueTone : uint8
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
 * Actor role types
 */
UENUM(BlueprintType)
enum class EActorRole : uint8
{
    Player              UMETA(DisplayName = "Player"),
    Companion           UMETA(DisplayName = "Companion"),
    NPC                 UMETA(DisplayName = "NPC"),
    Enemy               UMETA(DisplayName = "Enemy"),
    Boss                UMETA(DisplayName = "Boss"),
    Vendor              UMETA(DisplayName = "Vendor"),
    QuestGiver          UMETA(DisplayName = "Quest Giver"),
    Guard               UMETA(DisplayName = "Guard"),
    Civilian            UMETA(DisplayName = "Civilian"),
    Narrator            UMETA(DisplayName = "Narrator")
};

/**
 * Performance data structure
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPerformanceData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    EPerformanceEmotion Emotion;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    EDialogueTone DialogueTone;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    EActorRole ActorRole;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    FString DialogueText;

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    float IntensityLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    float Duration; // Duration of the performance

    UPROPERTY(BlueprintReadWrite, Category = "Performance Data")
    TMap<FString, FString> CustomTags; // Additional metadata

    FPerformanceData()
    {
        Emotion = EPerformanceEmotion::Neutral;
        DialogueTone = EDialogueTone::Normal;
        ActorRole = EActorRole::NPC;
        DialogueText = TEXT("");
        IntensityLevel = 0.5f;
        Duration = 3.0f;
    }
};

/**
 * Animation mapping data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAnimationMapping
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    FString AnimationTag;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    UAnimMontage* BodyMontage;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    UAnimMontage* FaceMontage;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    float BlendInTime;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    float BlendOutTime;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    bool bLooping;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Mapping")
    float PlayRate;

    FAnimationMapping()
    {
        AnimationTag = TEXT("");
        BodyMontage = nullptr;
        FaceMontage = nullptr;
        BlendInTime = 0.25f;
        BlendOutTime = 0.25f;
        bLooping = false;
        PlayRate = 1.0f;
    }
};

/**
 * Performance events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceStarted, const FPerformanceData&, PerformanceData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceCompleted, const FString&, PerformanceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimationTriggered, const FString&, AnimationTag, UAnimMontage*, Montage);

/**
 * Procedural Performance Component - Controls body + face animation synced with AIDM dialogue
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UProceduralPerformanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UProceduralPerformanceComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize performance component
     * @param SkeletalMesh Skeletal mesh component for animations
     * @param VoiceComponent Voice synthesis component for sync
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void InitializePerformance(USkeletalMeshComponent* SkeletalMesh, UVoiceSynthesisComponent* VoiceComponent);

    /**
     * Start performance
     * @param PerformanceData Data for the performance
     * @return Performance ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    FString StartPerformance(const FPerformanceData& PerformanceData);

    /**
     * Stop current performance
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void StopPerformance();

    /**
     * Add animation mapping
     * @param Mapping Animation mapping to add
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void AddAnimationMapping(const FAnimationMapping& Mapping);

    /**
     * Remove animation mapping
     * @param AnimationTag Tag of mapping to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void RemoveAnimationMapping(const FString& AnimationTag);

    /**
     * Load animation mappings from JSON
     * @param FilePath Path to JSON file
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    bool LoadAnimationMappings(const FString& FilePath);

    /**
     * Get animation for performance data
     * @param PerformanceData Performance data to analyze
     * @return Animation mapping for the performance
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    FAnimationMapping GetAnimationForPerformance(const FPerformanceData& PerformanceData) const;

    /**
     * Check if performance is active
     * @return True if performance is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Performance")
    bool IsPerformanceActive() const { return bPerformanceActive; }

    /**
     * Get current performance data
     * @return Current performance data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Performance")
    FPerformanceData GetCurrentPerformanceData() const { return CurrentPerformanceData; }

    /**
     * Set facial expression intensity
     * @param Intensity Intensity from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void SetFacialExpressionIntensity(float Intensity);

    /**
     * Trigger lip sync for dialogue
     * @param DialogueText Text being spoken
     * @param Duration Duration of the dialogue
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Performance")
    void TriggerLipSync(const FString& DialogueText, float Duration);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceStarted OnPerformanceStarted;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceCompleted OnPerformanceCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnAnimationTriggered OnAnimationTriggered;

protected:
    // Animation mappings
    UPROPERTY(BlueprintReadOnly, Category = "Animation Mappings")
    TMap<FString, FAnimationMapping> AnimationMappings;

    // Current performance state
    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    FPerformanceData CurrentPerformanceData;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    bool bPerformanceActive;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    FString CurrentPerformanceID;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    float PerformanceStartTime;

    // Component references
    UPROPERTY()
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    // Current animations
    UPROPERTY(BlueprintReadOnly, Category = "Current Animations")
    UAnimMontage* CurrentBodyMontage;

    UPROPERTY(BlueprintReadOnly, Category = "Current Animations")
    UAnimMontage* CurrentFaceMontage;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    bool bAutoSyncWithVoice; // Automatically sync with voice synthesis

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    float DefaultBlendTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    bool bEnableFacialAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    bool bEnableLipSync;

    // Timer handle
    FTimerHandle PerformanceTimer;

private:
    // Helper methods
    void LoadDefaultAnimationMappings();
    FString GenerateAnimationTag(const FPerformanceData& PerformanceData) const;
    void PlayBodyAnimation(UAnimMontage* Montage, float BlendInTime, float PlayRate);
    void PlayFacialAnimation(UAnimMontage* Montage, float BlendInTime, float PlayRate);
    void StopBodyAnimation(float BlendOutTime);
    void StopFacialAnimation(float BlendOutTime);
    void OnPerformanceTimerComplete();

    // Animation callbacks
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
    /**
     * Blueprint implementable events for custom performance logic
     */

    /**
     * Called when performance starts (for custom setup)
     * @param PerformanceData The performance data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    void OnPerformanceStartedEvent(const FPerformanceData& PerformanceData);

    /**
     * Called when animation is triggered (for custom effects)
     * @param AnimationTag Tag of the triggered animation
     * @param Montage The animation montage being played
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    void OnAnimationTriggeredEvent(const FString& AnimationTag, UAnimMontage* Montage);

    /**
     * Called to customize animation selection (override in Blueprint)
     * @param PerformanceData Performance data for customization
     * @return Custom animation mapping
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    FAnimationMapping CustomizeAnimationSelection(const FPerformanceData& PerformanceData);

    /**
     * Called to generate custom animation tag (override in Blueprint)
     * @param PerformanceData Performance data
     * @return Custom animation tag
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Performance Events")
    FString GenerateCustomAnimationTag(const FPerformanceData& PerformanceData);
};
