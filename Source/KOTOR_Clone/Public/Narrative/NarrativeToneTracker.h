// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "NarrativeToneTracker.generated.h"

/**
 * Emotional tone types
 */
UENUM(BlueprintType)
enum class ENarrativeTone : uint8
{
    Hopeful             UMETA(DisplayName = "Hopeful"),
    Grim                UMETA(DisplayName = "Grim"),
    Triumphant          UMETA(DisplayName = "Triumphant"),
    Tragic              UMETA(DisplayName = "Tragic"),
    Mysterious          UMETA(DisplayName = "Mysterious"),
    Tense               UMETA(DisplayName = "Tense"),
    Peaceful            UMETA(DisplayName = "Peaceful"),
    Chaotic             UMETA(DisplayName = "Chaotic"),
    Melancholic         UMETA(DisplayName = "Melancholic"),
    Inspiring           UMETA(DisplayName = "Inspiring"),
    Ominous             UMETA(DisplayName = "Ominous"),
    Lighthearted        UMETA(DisplayName = "Lighthearted")
};

/**
 * Emotional signal types
 */
UENUM(BlueprintType)
enum class EEmotionalSignal : uint8
{
    Joy                 UMETA(DisplayName = "Joy"),
    Sadness             UMETA(DisplayName = "Sadness"),
    Fear                UMETA(DisplayName = "Fear"),
    Anger               UMETA(DisplayName = "Anger"),
    Surprise            UMETA(DisplayName = "Surprise"),
    Disgust             UMETA(DisplayName = "Disgust"),
    Hope                UMETA(DisplayName = "Hope"),
    Despair             UMETA(DisplayName = "Despair"),
    Love                UMETA(DisplayName = "Love"),
    Hatred              UMETA(DisplayName = "Hatred"),
    Pride               UMETA(DisplayName = "Pride"),
    Shame               UMETA(DisplayName = "Shame")
};

/**
 * Entry for tracking individual emotional signals with a timestamp.
 * Replaces use of TPair<EEmotionalSignal,float> which is not UHT-compatible.
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FEmotionalSignalEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Emotional Signal")
    EEmotionalSignal Signal;

    UPROPERTY(BlueprintReadWrite, Category = "Emotional Signal")
    float Timestamp;

    FEmotionalSignalEntry()
        : Signal(EEmotionalSignal::Joy)
        , Timestamp(0.0f)
    {}

    FEmotionalSignalEntry(EEmotionalSignal InSignal, float InTimestamp)
        : Signal(InSignal)
        , Timestamp(InTimestamp)
    {}
};

/**
 * Tone analysis data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FToneAnalysis
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    FString AnalysisID;

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    float AnalysisTimestamp;

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    ENarrativeTone DominantTone;

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    TMap<ENarrativeTone, float> ToneWeights; // Tone -> weight (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    TMap<EEmotionalSignal, int32> EmotionalSignals; // Signal -> count

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    float ToneStability; // 0.0 to 1.0, higher = more stable

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    TArray<FString> RecentEvents; // Events that influenced tone

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    FString ToneSummary; // LLM-generated tone summary

    UPROPERTY(BlueprintReadWrite, Category = "Tone Analysis")
    TArray<FString> ToneRecommendations; // Suggested tone adjustments

    FToneAnalysis()
    {
        AnalysisID = TEXT("");
        AnalysisTimestamp = 0.0f;
        DominantTone = ENarrativeTone::Peaceful;
        ToneStability = 0.5f;
        ToneSummary = TEXT("");
    }
};

/**
 * Tone adjustment data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FToneAdjustment
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    FString AdjustmentID;

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    ENarrativeTone TargetTone;

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    FString AdjustmentType; // "quest", "dialogue", "environment", "music"

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    TMap<FString, FString> Parameters; // Adjustment parameters

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    float Intensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    float Duration; // How long adjustment should last

    UPROPERTY(BlueprintReadWrite, Category = "Tone Adjustment")
    bool bIsApplied;

    FToneAdjustment()
    {
        AdjustmentID = TEXT("");
        TargetTone = ENarrativeTone::Peaceful;
        AdjustmentType = TEXT("quest");
        Description = TEXT("Tone adjustment");
        Intensity = 0.5f;
        Duration = 3600.0f; // 1 hour
        bIsApplied = false;
    }
};

/**
 * Tone events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToneAnalysisCompleted, const FToneAnalysis&, Analysis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToneShiftDetected, ENarrativeTone, OldTone, ENarrativeTone, NewTone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToneAdjustmentTriggered, const FToneAdjustment&, Adjustment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEmotionalSignalDetected, EEmotionalSignal, Signal, float, Intensity);

/**
 * Narrative Tone Tracker - Monitors and regulates narrative mood and tone
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UNarrativeToneTracker : public UActorComponent
{
    GENERATED_BODY()

public:
    UNarrativeToneTracker();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize narrative tone tracker
     * @param CampaignLoader Campaign loader for context
     * @param NarrativeMemory Narrative memory for event analysis
     * @param CompanionManager Companion manager for dialogue tone
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    void InitializeToneTracker(UCampaignLoaderSubsystem* CampaignLoader,
                              UNarrativeMemoryComponent* NarrativeMemory,
                              UCompanionManagerComponent* CompanionManager);

    /**
     * Analyze current narrative tone
     * @param AnalysisWindow Time window to analyze (seconds)
     * @return Tone analysis result
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    FToneAnalysis AnalyzeNarrativeTone(float AnalysisWindow = 3600.0f); // 1 hour default

    /**
     * Add emotional signal
     * @param Signal Type of emotional signal
     * @param Intensity Signal intensity (0.0 to 1.0)
     * @param Context Context that triggered the signal
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    void AddEmotionalSignal(EEmotionalSignal Signal, float Intensity, const FString& Context);

    /**
     * Apply tone adjustment
     * @param Adjustment Tone adjustment to apply
     * @return True if adjustment was applied successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    bool ApplyToneAdjustment(const FToneAdjustment& Adjustment);

    /**
     * Generate tone adjustment recommendation
     * @param CurrentTone Current dominant tone
     * @param DesiredDirection Direction to shift tone
     * @return Generated tone adjustment
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    FToneAdjustment GenerateToneAdjustment(ENarrativeTone CurrentTone, const FString& DesiredDirection);

    /**
     * Get current dominant tone
     * @return Current dominant narrative tone
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Tone")
    ENarrativeTone GetCurrentDominantTone() const;

    /**
     * Get tone weights
     * @return Map of current tone weights
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Tone")
    TMap<ENarrativeTone, float> GetCurrentToneWeights() const;

    /**
     * Get recent emotional signals
     * @param TimeWindow Time window to check (seconds)
     * @return Map of emotional signals in time window
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Tone")
    TMap<EEmotionalSignal, int32> GetRecentEmotionalSignals(float TimeWindow = 1800.0f) const; // 30 minutes

    /**
     * Generate AIDM tone context
     * @return Context string for AIDM prompts
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    FString GenerateAIDMToneContext() const;

    /**
     * Set tone tracking enabled
     * @param bEnabled Whether to enable tone tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    void SetToneTrackingEnabled(bool bEnabled);

    /**
     * Set automatic tone adjustment enabled
     * @param bEnabled Whether to enable automatic adjustments
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Tone")
    void SetAutomaticAdjustmentEnabled(bool bEnabled);

    /**
     * Get tone analysis history
     * @param MaxResults Maximum number of results
     * @return Array of previous tone analyses
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Tone")
    TArray<FToneAnalysis> GetToneAnalysisHistory(int32 MaxResults = 10) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Tone Events")
    FOnToneAnalysisCompleted OnToneAnalysisCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Tone Events")
    FOnToneShiftDetected OnToneShiftDetected;

    UPROPERTY(BlueprintAssignable, Category = "Tone Events")
    FOnToneAdjustmentTriggered OnToneAdjustmentTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Tone Events")
    FOnEmotionalSignalDetected OnEmotionalSignalDetected;

protected:
    // Tone data
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Tone")
    TArray<FToneAnalysis> ToneAnalysisHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Narrative Tone")
    TMap<ENarrativeTone, float> CurrentToneWeights;

    UPROPERTY(BlueprintReadOnly, Category = "Narrative Tone")
    TArray<FToneAdjustment> ActiveAdjustments;

    // Emotional signals tracking
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Tone")
    /*
     * UE5 reflection does not support TPair directly inside containers.
     * We therefore use FEmotionalSignalEntry defined above.
     */
    TArray<FEmotionalSignalEntry> RecentSignals; // Signal + timestamp

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    // Tone settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tone Settings")
    bool bToneTrackingEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tone Settings")
    bool bAutomaticAdjustmentEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tone Settings")
    float AnalysisInterval; // Seconds between tone analyses

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tone Settings")
    float ToneShiftThreshold; // Threshold for detecting tone shifts

    // Tone mapping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tone Mapping")
    TMap<FString, ENarrativeTone> EventToneMapping; // Event type -> tone

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tone Mapping")
    TMap<FString, EEmotionalSignal> EventSignalMapping; // Event type -> signal

    // Timer handles
    FTimerHandle AnalysisTimer;
    FTimerHandle AdjustmentTimer;

private:
    // Helper methods
    FString GenerateAnalysisID();
    FString GenerateAdjustmentID();
    ENarrativeTone CalculateDominantTone();
    float CalculateToneStability();
    TArray<FString> GenerateToneRecommendations(const FToneAnalysis& Analysis);
    void ProcessAutomaticAdjustments();
    void UpdateToneWeights();
    void CleanupOldSignals();

    // Event handlers
    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

public:
    /**
     * Blueprint implementable events for custom tone logic
     */

    /**
     * Called when tone analysis completes (for custom processing)
     * @param Analysis The completed tone analysis
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Tone Events")
    void OnToneAnalysisCompletedEvent(const FToneAnalysis& Analysis);

    /**
     * Called when tone shift is detected (for custom effects)
     * @param OldTone Previous dominant tone
     * @param NewTone New dominant tone
     * @param ShiftIntensity Intensity of the shift
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Tone Events")
    void OnToneShiftDetectedEvent(ENarrativeTone OldTone, ENarrativeTone NewTone, float ShiftIntensity);

    /**
     * Called to generate custom tone adjustment (override in Blueprint)
     * @param CurrentTone Current dominant tone
     * @param ToneHistory Recent tone history
     * @return Custom tone adjustment
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Tone Events")
    FToneAdjustment GenerateCustomToneAdjustment(ENarrativeTone CurrentTone, const TArray<FToneAnalysis>& ToneHistory);

    /**
     * Called to apply custom environmental tone effects (override in Blueprint)
     * @param TargetTone Tone to apply
     * @param Intensity Effect intensity
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Tone Events")
    void ApplyCustomEnvironmentalToneEffects(ENarrativeTone TargetTone, float Intensity);
};
