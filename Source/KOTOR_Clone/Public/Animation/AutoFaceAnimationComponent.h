// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "Sound/SoundWave.h"
#include "AutoFaceAnimationComponent.generated.h"

/**
 * Phoneme types for lip sync
 */
UENUM(BlueprintType)
enum class EPhonemeType : uint8
{
    Silence             UMETA(DisplayName = "Silence"),
    A                   UMETA(DisplayName = "A (ah)"),
    E                   UMETA(DisplayName = "E (eh)"),
    I                   UMETA(DisplayName = "I (ih)"),
    O                   UMETA(DisplayName = "O (oh)"),
    U                   UMETA(DisplayName = "U (oo)"),
    M_B_P               UMETA(DisplayName = "M/B/P (lips closed)"),
    F_V                 UMETA(DisplayName = "F/V (teeth on lip)"),
    T_D_N_L             UMETA(DisplayName = "T/D/N/L (tongue to teeth)"),
    S_Z                 UMETA(DisplayName = "S/Z (hiss)"),
    SH_CH_J             UMETA(DisplayName = "SH/CH/J (shush)"),
    TH                  UMETA(DisplayName = "TH (think)"),
    R                   UMETA(DisplayName = "R (round)"),
    K_G                 UMETA(DisplayName = "K/G (back tongue)"),
    W                   UMETA(DisplayName = "W (round lips)"),
    Y                   UMETA(DisplayName = "Y (smile)")
};

/**
 * Facial expression types
 */
UENUM(BlueprintType)
enum class EFacialExpression : uint8
{
    Neutral             UMETA(DisplayName = "Neutral"),
    Smile               UMETA(DisplayName = "Smile"),
    Frown               UMETA(DisplayName = "Frown"),
    Surprise            UMETA(DisplayName = "Surprise"),
    Anger               UMETA(DisplayName = "Anger"),
    Sadness             UMETA(DisplayName = "Sadness"),
    Fear                UMETA(DisplayName = "Fear"),
    Disgust             UMETA(DisplayName = "Disgust"),
    Contempt            UMETA(DisplayName = "Contempt"),
    Confusion           UMETA(DisplayName = "Confusion"),
    Concentration       UMETA(DisplayName = "Concentration"),
    Pain                UMETA(DisplayName = "Pain"),
    Joy                 UMETA(DisplayName = "Joy"),
    Determination       UMETA(DisplayName = "Determination"),
    Skepticism          UMETA(DisplayName = "Skepticism"),
    Curiosity           UMETA(DisplayName = "Curiosity")
};

/**
 * Phoneme data for lip sync
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPhonemeData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Data")
    EPhonemeType PhonemeType;

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Data")
    float StartTime; // Time in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Data")
    float Duration; // Duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Data")
    float Intensity; // 0.0 to 1.0

    FPhonemeData()
    {
        PhonemeType = EPhonemeType::Silence;
        StartTime = 0.0f;
        Duration = 0.1f;
        Intensity = 1.0f;
    }
};

/**
 * Facial animation data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FFacialAnimationData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    FString DialogueText;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    EFacialExpression Expression;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    TArray<FPhonemeData> PhonemeSequence;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    float TotalDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    float ExpressionIntensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    bool bEnableBlinks;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation")
    bool bEnableEyeTracking;

    FFacialAnimationData()
    {
        DialogueText = TEXT("");
        Expression = EFacialExpression::Neutral;
        TotalDuration = 0.0f;
        ExpressionIntensity = 1.0f;
        bEnableBlinks = true;
        bEnableEyeTracking = true;
    }
};

/**
 * Face animation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFaceAnimationStarted, const FFacialAnimationData&, AnimationData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFaceAnimationCompleted, const FString&, DialogueText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhonemeTriggered, EPhonemeType, PhonemeType, float, Intensity);

/**
 * Auto Face Animation Component - Sync lip and face gestures to TTS voice line using UE native systems
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAutoFaceAnimationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAutoFaceAnimationComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize face animation component
     * @param SkeletalMesh Skeletal mesh component with facial bones
     * @param VoiceComponent Voice synthesis component for sync
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    void InitializeFaceAnimation(USkeletalMeshComponent* SkeletalMesh, UVoiceSynthesisComponent* VoiceComponent);

    /**
     * Start facial animation for dialogue
     * @param DialogueText Text being spoken
     * @param Expression Facial expression to use
     * @param Duration Duration of the dialogue
     * @return True if animation started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    bool StartFacialAnimation(const FString& DialogueText, EFacialExpression Expression, float Duration);

    /**
     * Stop current facial animation
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    void StopFacialAnimation();

    /**
     * Generate phoneme sequence from text
     * @param Text Text to analyze
     * @param Duration Total duration for the sequence
     * @return Array of phoneme data
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    TArray<FPhonemeData> GeneratePhonemeSequence(const FString& Text, float Duration);

    /**
     * Set facial expression
     * @param Expression Expression to set
     * @param Intensity Intensity from 0.0 to 1.0
     * @param BlendTime Time to blend to expression
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    void SetFacialExpression(EFacialExpression Expression, float Intensity = 1.0f, float BlendTime = 0.3f);

    /**
     * Trigger phoneme
     * @param PhonemeType Type of phoneme
     * @param Intensity Intensity from 0.0 to 1.0
     * @param Duration Duration of the phoneme
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    void TriggerPhoneme(EPhonemeType PhonemeType, float Intensity = 1.0f, float Duration = 0.1f);

    /**
     * Enable/disable automatic blinking
     * @param bEnabled Whether to enable automatic blinking
     * @param BlinkInterval Average time between blinks
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    void SetAutomaticBlinking(bool bEnabled, float BlinkInterval = 3.0f);

    /**
     * Enable/disable eye tracking
     * @param bEnabled Whether to enable eye tracking
     * @param Target Target to look at (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Face Animation")
    void SetEyeTracking(bool bEnabled, AActor* Target = nullptr);

    /**
     * Check if facial animation is active
     * @return True if facial animation is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Face Animation")
    bool IsFacialAnimationActive() const { return bAnimationActive; }

    /**
     * Get current facial expression
     * @return Current facial expression
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Face Animation")
    EFacialExpression GetCurrentExpression() const { return CurrentExpression; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Face Animation Events")
    FOnFaceAnimationStarted OnFaceAnimationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Face Animation Events")
    FOnFaceAnimationCompleted OnFaceAnimationCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Face Animation Events")
    FOnPhonemeTriggered OnPhonemeTriggered;

protected:
    // Current animation data
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    FFacialAnimationData CurrentAnimationData;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    bool bAnimationActive;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    float AnimationStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    int32 CurrentPhonemeIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EFacialExpression CurrentExpression;

    // Component references
    UPROPERTY()
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    // Eye tracking
    UPROPERTY(BlueprintReadOnly, Category = "Eye Tracking")
    bool bEyeTrackingEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Eye Tracking")
    AActor* EyeTrackingTarget;

    // Blinking
    UPROPERTY(BlueprintReadOnly, Category = "Blinking")
    bool bAutomaticBlinkingEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Blinking")
    float BlinkInterval;

    UPROPERTY(BlueprintReadOnly, Category = "Blinking")
    float LastBlinkTime;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Animation Settings")
    bool bEnableLipSync;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Animation Settings")
    bool bEnableFacialExpressions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Animation Settings")
    float DefaultExpressionIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Animation Settings")
    float PhonemeBlendSpeed; // How quickly to blend between phonemes

    // Phoneme to morph target mapping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phoneme Mapping")
    TMap<EPhonemeType, FString> PhonemeMorphTargets;

    // Expression to morph target mapping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Expression Mapping")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested maps such as TMap<Key, TMap<...>>
     * There are two approaches to fix this:
     * 1. Flatten to a single TArray and filter at runtime
     * 2. Create a wrapper struct for the inner map:
     *    USTRUCT() struct FMapWrapper { TMap<Key2, Value> InnerMap; };
     *    TMap<EFacialExpression, FMapWrapper> ExpressionMorphTargets;
     *
     * We've chosen approach #1 for simplicity.
     */
    TArray<FKeyValuePair>ExpressionMorphTargets;

    // Timer handles
    FTimerHandle BlinkTimer;

private:
    // Helper methods
    void InitializeMorphTargetMappings();
    void UpdateFacialAnimation(float DeltaTime);
    void ProcessCurrentPhoneme();
    void SetMorphTargetValue(const FString& MorphTargetName, float Value);
    void BlendToExpression(EFacialExpression Expression, float Intensity, float BlendTime);
    EPhonemeType GetPhonemeFromCharacter(TCHAR Character);
    void TriggerAutomaticBlink();

    // Text analysis
    TArray<FPhonemeData> AnalyzeTextForPhonemes(const FString& Text, float Duration);
    float CalculatePhonemeIntensity(EPhonemeType PhonemeType, const FString& Context);

    // Timer callbacks
    UFUNCTION()
    void OnBlinkTimer();

    // Voice synthesis events
    UFUNCTION()
    void OnVoicePlaybackStarted(const FString& SpeakerName, const FString& DialogueLine);

    UFUNCTION()
    void OnVoicePlaybackCompleted(const FString& SpeakerName);

public:
    /**
     * Blueprint implementable events for custom face animation logic
     */

    /**
     * Called when facial animation starts (for custom setup)
     * @param AnimationData The facial animation data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Face Animation Events")
    void OnFaceAnimationStartedEvent(const FFacialAnimationData& AnimationData);

    /**
     * Called when phoneme is triggered (for custom effects)
     * @param PhonemeType Type of phoneme
     * @param Intensity Phoneme intensity
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Face Animation Events")
    void OnPhonemeTriggeredEvent(EPhonemeType PhonemeType, float Intensity);

    /**
     * Called to customize phoneme sequence (override in Blueprint)
     * @param Text Text being analyzed
     * @param Duration Total duration
     * @return Custom phoneme sequence
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Face Animation Events")
    TArray<FPhonemeData> GenerateCustomPhonemeSequence(const FString& Text, float Duration);

    /**
     * Called to customize facial expression (override in Blueprint)
     * @param BaseExpression Base expression
     * @param DialogueContext Context of the dialogue
     * @return Customized expression
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Face Animation Events")
    EFacialExpression CustomizeFacialExpression(EFacialExpression BaseExpression, const FString& DialogueContext);
};
