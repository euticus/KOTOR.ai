// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "ControlRig.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "MetaHumanFacialAnimationComponent.generated.h"

/**
 * Facial expression types for MetaHuman
 */
UENUM(BlueprintType)
enum class EMetaHumanExpression : uint8
{
    Neutral             UMETA(DisplayName = "Neutral"),
    Happy               UMETA(DisplayName = "Happy"),
    Sad                 UMETA(DisplayName = "Sad"),
    Angry               UMETA(DisplayName = "Angry"),
    Surprised           UMETA(DisplayName = "Surprised"),
    Disgusted           UMETA(DisplayName = "Disgusted"),
    Fearful             UMETA(DisplayName = "Fearful"),
    Contemptuous        UMETA(DisplayName = "Contemptuous"),
    Confused            UMETA(DisplayName = "Confused"),
    Determined          UMETA(DisplayName = "Determined"),
    Loving              UMETA(DisplayName = "Loving"),
    Hateful             UMETA(DisplayName = "Hateful"),
    Proud               UMETA(DisplayName = "Proud"),
    Ashamed             UMETA(DisplayName = "Ashamed"),
    Hopeful             UMETA(DisplayName = "Hopeful"),
    Despairing          UMETA(DisplayName = "Despairing")
};

/**
 * Viseme types for lip sync
 */
UENUM(BlueprintType)
enum class EMetaHumanViseme : uint8
{
    Sil                 UMETA(DisplayName = "Silence"),
    PP                  UMETA(DisplayName = "PP (p, b, m)"),
    FF                  UMETA(DisplayName = "FF (f, v)"),
    TH                  UMETA(DisplayName = "TH (th)"),
    DD                  UMETA(DisplayName = "DD (t, d, n, l)"),
    kk                  UMETA(DisplayName = "kk (k, g)"),
    CH                  UMETA(DisplayName = "CH (ch, j, sh, zh)"),
    SS                  UMETA(DisplayName = "SS (s, z)"),
    nn                  UMETA(DisplayName = "nn (n, ng)"),
    RR                  UMETA(DisplayName = "RR (r)"),
    aa                  UMETA(DisplayName = "aa (a)"),
    E                   UMETA(DisplayName = "E (e)"),
    I                   UMETA(DisplayName = "I (i)"),
    O                   UMETA(DisplayName = "O (o)"),
    U                   UMETA(DisplayName = "U (u)")
};

/**
 * Control rig curve data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FControlRigCurveData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Control Rig Curve")
    FString CurveName;

    UPROPERTY(BlueprintReadWrite, Category = "Control Rig Curve")
    float CurrentValue;

    UPROPERTY(BlueprintReadWrite, Category = "Control Rig Curve")
    float TargetValue;

    UPROPERTY(BlueprintReadWrite, Category = "Control Rig Curve")
    float BlendSpeed; // Speed of blending to target

    UPROPERTY(BlueprintReadWrite, Category = "Control Rig Curve")
    bool bIsBlending;

    FControlRigCurveData()
    {
        CurveName = TEXT("");
        CurrentValue = 0.0f;
        TargetValue = 0.0f;
        BlendSpeed = 5.0f;
        bIsBlending = false;
    }
};

/**
 * Phoneme timing data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPhonemeTimingData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Timing")
    EMetaHumanViseme Viseme;

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Timing")
    float StartTime; // Time in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Timing")
    float Duration; // Duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Phoneme Timing")
    float Intensity; // 0.0 to 1.0

    FPhonemeTimingData()
    {
        Viseme = EMetaHumanViseme::Sil;
        StartTime = 0.0f;
        Duration = 0.1f;
        Intensity = 1.0f;
    }
};

/**
 * Facial animation sequence
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FFacialAnimationSequence
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    FString SequenceID;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    EMetaHumanExpression Expression;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    TArray<FPhonemeTimingData> PhonemeSequence;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    float TotalDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    float ExpressionIntensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    bool bEnableBlinks;

    UPROPERTY(BlueprintReadWrite, Category = "Facial Animation Sequence")
    bool bEnableEyeTracking;

    FFacialAnimationSequence()
    {
        SequenceID = TEXT("");
        Expression = EMetaHumanExpression::Neutral;
        TotalDuration = 0.0f;
        ExpressionIntensity = 1.0f;
        bEnableBlinks = true;
        bEnableEyeTracking = true;
    }
};

/**
 * MetaHuman facial animation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFacialAnimationStarted, const FFacialAnimationSequence&, Sequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFacialAnimationCompleted, const FString&, SequenceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVisemeTriggered, EMetaHumanViseme, Viseme, float, Intensity);

/**
 * MetaHuman Facial Animation Component - Control Rig curves for expressions and visemes with phoneme syncing
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMetaHumanFacialAnimationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMetaHumanFacialAnimationComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize facial animation component
     * @param SkeletalMesh MetaHuman skeletal mesh component
     * @param FaceControlRig Control rig for facial animation
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void InitializeFacialAnimation(USkeletalMeshComponent* SkeletalMesh, UControlRig* FaceControlRig);

    /**
     * Play facial animation sequence
     * @param Sequence Facial animation sequence to play
     * @return True if sequence started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    bool PlayFacialAnimationSequence(const FFacialAnimationSequence& Sequence);

    /**
     * Set facial expression
     * @param Expression Expression to set
     * @param Intensity Intensity from 0.0 to 1.0
     * @param BlendTime Time to blend to expression
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void SetFacialExpression(EMetaHumanExpression Expression, float Intensity = 1.0f, float BlendTime = 0.3f);

    /**
     * Trigger viseme
     * @param Viseme Viseme to trigger
     * @param Intensity Intensity from 0.0 to 1.0
     * @param Duration Duration of the viseme
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void TriggerViseme(EMetaHumanViseme Viseme, float Intensity = 1.0f, float Duration = 0.1f);

    /**
     * Generate phoneme sequence from text
     * @param Text Text to analyze
     * @param Duration Total duration for the sequence
     * @return Array of phoneme timing data
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    TArray<FPhonemeTimingData> GeneratePhonemeSequence(const FString& Text, float Duration);

    /**
     * Set control rig curve value
     * @param CurveName Name of the curve
     * @param Value Value to set (0.0 to 1.0)
     * @param BlendTime Time to blend to value
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void SetControlRigCurve(const FString& CurveName, float Value, float BlendTime = 0.1f);

    /**
     * Get control rig curve value
     * @param CurveName Name of the curve
     * @return Current curve value
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaHuman Facial Animation")
    float GetControlRigCurve(const FString& CurveName) const;

    /**
     * Stop current facial animation
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void StopFacialAnimation();

    /**
     * Enable/disable automatic blinking
     * @param bEnabled Whether to enable automatic blinking
     * @param BlinkInterval Average time between blinks
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void SetAutomaticBlinking(bool bEnabled, float BlinkInterval = 3.0f);

    /**
     * Enable/disable eye tracking
     * @param bEnabled Whether to enable eye tracking
     * @param Target Target to look at (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Facial Animation")
    void SetEyeTracking(bool bEnabled, AActor* Target = nullptr);

    /**
     * Check if facial animation is active
     * @return True if facial animation is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaHuman Facial Animation")
    bool IsFacialAnimationActive() const { return bAnimationActive; }

    /**
     * Get current expression
     * @return Current facial expression
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MetaHuman Facial Animation")
    EMetaHumanExpression GetCurrentExpression() const { return CurrentExpression; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Facial Animation Events")
    FOnFacialAnimationStarted OnFacialAnimationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Facial Animation Events")
    FOnFacialAnimationCompleted OnFacialAnimationCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Facial Animation Events")
    FOnVisemeTriggered OnVisemeTriggered;

protected:
    // Current animation state
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    FFacialAnimationSequence CurrentSequence;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    bool bAnimationActive;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    float AnimationStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    int32 CurrentPhonemeIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EMetaHumanExpression CurrentExpression;

    // Component references
    UPROPERTY()
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY()
    UControlRig* FaceControlRig;

    // Control rig curves
    UPROPERTY(BlueprintReadOnly, Category = "Control Rig")
    TMap<FString, FControlRigCurveData> ControlRigCurves;

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial Animation Settings")
    bool bEnableLipSync;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial Animation Settings")
    bool bEnableFacialExpressions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial Animation Settings")
    float DefaultExpressionIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Facial Animation Settings")
    float VisemeBlendSpeed; // How quickly to blend between visemes

    // Expression to curve mappings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Expression Mapping")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested maps such as TMap<Key, TMap<...>>
     * There are two approaches to fix this:
     * 1. Flatten to a single TArray and filter at runtime
     * 2. Create a wrapper struct for the inner map:
     *    USTRUCT() struct FMapWrapper { TMap<Key2, Value> InnerMap; };
     *    TMap<EMetaHumanExpression, FMapWrapper> ExpressionCurveMappings;
     *
     * We've chosen approach #1 for simplicity.
     */
    TArray<FKeyValuePair>ExpressionCurveMappings;

    // Viseme to curve mappings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viseme Mapping")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested maps such as TMap<Key, TMap<...>>
     * There are two approaches to fix this:
     * 1. Flatten to a single TArray and filter at runtime
     * 2. Create a wrapper struct for the inner map:
     *    USTRUCT() struct FMapWrapper { TMap<Key2, Value> InnerMap; };
     *    TMap<EMetaHumanViseme, FMapWrapper> VisemeCurveMappings;
     *
     * We've chosen approach #1 for simplicity.
     */
    TArray<FKeyValuePair>VisemeCurveMappings;

    // Timer handles
    FTimerHandle BlinkTimer;

private:
    // Helper methods
    void InitializeCurveMappings();
    void UpdateFacialAnimation(float DeltaTime);
    void UpdateControlRigCurves(float DeltaTime);
    void ProcessCurrentPhoneme();
    void SetCurveValue(const FString& CurveName, float Value);
    void BlendToExpression(EMetaHumanExpression Expression, float Intensity, float BlendTime);
    EMetaHumanViseme GetVisemeFromCharacter(TCHAR Character);
    void TriggerAutomaticBlink();

    // Text analysis
    TArray<FPhonemeTimingData> AnalyzeTextForVisemes(const FString& Text, float Duration);
    float CalculateVisemeIntensity(EMetaHumanViseme Viseme, const FString& Context);

    // Timer callbacks
    UFUNCTION()
    void OnBlinkTimer();

public:
    /**
     * Blueprint implementable events for custom facial animation logic
     */

    /**
     * Called when facial animation starts (for custom setup)
     * @param Sequence The facial animation sequence
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Facial Animation Events")
    void OnFacialAnimationStartedEvent(const FFacialAnimationSequence& Sequence);

    /**
     * Called when viseme is triggered (for custom effects)
     * @param Viseme Type of viseme
     * @param Intensity Viseme intensity
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Facial Animation Events")
    void OnVisemeTriggeredEvent(EMetaHumanViseme Viseme, float Intensity);

    /**
     * Called to customize phoneme sequence (override in Blueprint)
     * @param Text Text being analyzed
     * @param Duration Total duration
     * @return Custom phoneme sequence
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Facial Animation Events")
    TArray<FPhonemeTimingData> GenerateCustomPhonemeSequence(const FString& Text, float Duration);

    /**
     * Called to customize facial expression (override in Blueprint)
     * @param BaseExpression Base expression
     * @param DialogueContext Context of the dialogue
     * @return Customized expression
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Facial Animation Events")
    EMetaHumanExpression CustomizeFacialExpression(EMetaHumanExpression BaseExpression, const FString& DialogueContext);
};
