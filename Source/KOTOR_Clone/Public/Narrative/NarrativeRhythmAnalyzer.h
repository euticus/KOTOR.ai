// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Timeline/CampaignTimelineComponent.h"
#include "NarrativeRhythmAnalyzer.generated.h"

/**
 * Pacing analysis types
 */
UENUM(BlueprintType)
enum class EPacingAnalysisType : uint8
{
    Combat          UMETA(DisplayName = "Combat"),
    Dialogue        UMETA(DisplayName = "Dialogue"),
    Exploration     UMETA(DisplayName = "Exploration"),
    MoralChoice     UMETA(DisplayName = "Moral Choice"),
    StoryProgression UMETA(DisplayName = "Story Progression"),
    Tension         UMETA(DisplayName = "Tension"),
    Relief          UMETA(DisplayName = "Relief"),
    Mystery         UMETA(DisplayName = "Mystery")
};

/**
 * Pacing metrics
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPacingMetrics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    float CombatPercentage; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    float DialoguePercentage; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    float ExplorationPercentage; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    int32 MoralEventCount;

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    float TensionLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    float StoryProgressionRate; // Events per hour

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    int32 QuestsCompleted;

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    float AverageQuestDuration; // Minutes

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Metrics")
    TMap<FString, int32> EventTypeCounts; // Event type -> count

    FPacingMetrics()
    {
        CombatPercentage = 0.0f;
        DialoguePercentage = 0.0f;
        ExplorationPercentage = 0.0f;
        MoralEventCount = 0;
        TensionLevel = 0.0f;
        StoryProgressionRate = 0.0f;
        QuestsCompleted = 0;
        AverageQuestDuration = 0.0f;
    }
};

/**
 * Pacing analysis result
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPacingAnalysis
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    FString AnalysisID;

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    float AnalysisTimestamp;

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    FPacingMetrics Metrics;

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    FString PacingSummary; // LLM-generated summary

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    TArray<FString> Recommendations; // Pacing recommendations

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    float OverallPacingScore; // 0.0 to 1.0 (1.0 = perfect pacing)

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    TMap<FString, float> PacingIssues; // Issue type -> severity

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Analysis")
    FString NextQuestSuggestion; // Suggested next quest type

    FPacingAnalysis()
    {
        AnalysisID = TEXT("");
        AnalysisTimestamp = 0.0f;
        PacingSummary = TEXT("");
        OverallPacingScore = 0.5f;
        NextQuestSuggestion = TEXT("exploration");
    }
};

/**
 * Pacing adjustment action
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPacingAdjustment
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Adjustment")
    FString AdjustmentType; // "cooldown", "tension", "story_push", "exploration"

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Adjustment")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Adjustment")
    TMap<FString, FString> Parameters; // Adjustment parameters

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Adjustment")
    float Priority; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Pacing Adjustment")
    bool bIsApplied;

    FPacingAdjustment()
    {
        AdjustmentType = TEXT("cooldown");
        Description = TEXT("Pacing adjustment");
        Priority = 0.5f;
        bIsApplied = false;
    }
};

/**
 * Narrative rhythm events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPacingAnalysisCompleted, const FPacingAnalysis&, Analysis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPacingAdjustmentTriggered, const FPacingAdjustment&, Adjustment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPacingIssueDetected, const FString&, IssueType, float, Severity);

/**
 * Narrative Rhythm Analyzer - Monitors and adjusts campaign pacing
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UNarrativeRhythmAnalyzer : public UActorComponent
{
    GENERATED_BODY()

public:
    UNarrativeRhythmAnalyzer();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize narrative rhythm analyzer
     * @param QuestManager Quest manager for quest tracking
     * @param NarrativeMemory Narrative memory for event analysis
     * @param Timeline Campaign timeline for historical data
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    void InitializeRhythmAnalyzer(UQuestManagerComponent* QuestManager,
                                 UNarrativeMemoryComponent* NarrativeMemory,
                                 UCampaignTimelineComponent* Timeline);

    /**
     * Perform pacing analysis
     * @param AnalysisWindow Time window to analyze (in seconds)
     * @return Pacing analysis result
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    FPacingAnalysis PerformPacingAnalysis(float AnalysisWindow = 7200.0f); // 2 hours default

    /**
     * Trigger automatic pacing analysis
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    void TriggerAutomaticAnalysis();

    /**
     * Apply pacing adjustment
     * @param Adjustment Adjustment to apply
     * @return True if adjustment was applied successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    bool ApplyPacingAdjustment(const FPacingAdjustment& Adjustment);

    /**
     * Get pacing recommendations
     * @param CurrentMetrics Current pacing metrics
     * @return Array of pacing recommendations
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    TArray<FString> GetPacingRecommendations(const FPacingMetrics& CurrentMetrics);

    /**
     * Calculate pacing metrics for time window
     * @param StartTime Start of analysis window
     * @param EndTime End of analysis window
     * @return Calculated pacing metrics
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    FPacingMetrics CalculatePacingMetrics(float StartTime, float EndTime);

    /**
     * Detect pacing issues
     * @param Metrics Pacing metrics to analyze
     * @return Map of detected issues and their severity
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    TMap<FString, float> DetectPacingIssues(const FPacingMetrics& Metrics);

    /**
     * Generate next quest suggestion
     * @param CurrentMetrics Current pacing metrics
     * @param RecentQuests Recently completed quests
     * @return Suggested quest type
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    FString GenerateNextQuestSuggestion(const FPacingMetrics& CurrentMetrics, const TArray<FString>& RecentQuests);

    /**
     * Set automatic analysis enabled
     * @param bEnabled Whether to enable automatic analysis
     * @param AnalysisInterval Interval between analyses (seconds)
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    void SetAutomaticAnalysisEnabled(bool bEnabled, float AnalysisInterval = 1800.0f); // 30 minutes default

    /**
     * Get analysis history
     * @param MaxResults Maximum number of results to return
     * @return Array of previous analyses
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Rhythm")
    TArray<FPacingAnalysis> GetAnalysisHistory(int32 MaxResults = 10) const;

    /**
     * Get current pacing score
     * @return Current overall pacing score (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Rhythm")
    float GetCurrentPacingScore() const;

    /**
     * Export analysis data
     * @return JSON string of analysis data
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    FString ExportAnalysisData() const;

    /**
     * Import analysis data
     * @param AnalysisData JSON string of analysis data
     * @return True if import was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Rhythm")
    bool ImportAnalysisData(const FString& AnalysisData);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Narrative Rhythm Events")
    FOnPacingAnalysisCompleted OnPacingAnalysisCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Narrative Rhythm Events")
    FOnPacingAdjustmentTriggered OnPacingAdjustmentTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Narrative Rhythm Events")
    FOnPacingIssueDetected OnPacingIssueDetected;

protected:
    // Analysis data
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Rhythm")
    TArray<FPacingAnalysis> AnalysisHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Narrative Rhythm")
    TArray<FPacingAdjustment> PendingAdjustments;

    // Component references
    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UCampaignTimelineComponent* TimelineRef;

    // Analysis settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis Settings")
    bool bAutomaticAnalysisEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis Settings")
    float AnalysisInterval; // Seconds between automatic analyses

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis Settings")
    int32 QuestTriggerThreshold; // Number of quests before analysis

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis Settings")
    float IdealCombatPercentage; // Target combat percentage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis Settings")
    float IdealDialoguePercentage; // Target dialogue percentage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis Settings")
    float IdealExplorationPercentage; // Target exploration percentage

    // Pacing thresholds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pacing Thresholds")
    float TensionBuildupThreshold; // When to inject tension

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pacing Thresholds")
    float CooldownThreshold; // When to inject cooldown

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pacing Thresholds")
    int32 MaxConsecutiveCombat; // Max combat encounters in a row

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pacing Thresholds")
    int32 MaxConsecutiveDialogue; // Max dialogue encounters in a row

    // Timer handles
    FTimerHandle AnalysisTimer;

    // Tracking data
    UPROPERTY()
    int32 QuestsCompletedSinceLastAnalysis;

    UPROPERTY()
    float LastAnalysisTime;

private:
    // Helper methods
    FString GenerateAnalysisID();
    FString GeneratePacingSummary(const FPacingMetrics& Metrics);
    TArray<FString> AnalyzePacingTrends(const FPacingMetrics& Metrics);
    float CalculateOverallPacingScore(const FPacingMetrics& Metrics);
    void ProcessAutomaticAdjustments(const FPacingAnalysis& Analysis);
    FPacingAdjustment CreateCooldownAdjustment();
    FPacingAdjustment CreateTensionAdjustment();
    FPacingAdjustment CreateExplorationAdjustment();

    // Event handlers
    UFUNCTION()
    void OnQuestCompleted(const FActiveQuest& Quest);

public:
    /**
     * Blueprint implementable events for custom rhythm analysis
     */

    /**
     * Called when pacing analysis completes (for custom processing)
     * @param Analysis The completed analysis
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Rhythm Events")
    void OnPacingAnalysisCompletedEvent(const FPacingAnalysis& Analysis);

    /**
     * Called when pacing adjustment is triggered (for custom effects)
     * @param Adjustment The triggered adjustment
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Rhythm Events")
    void OnPacingAdjustmentTriggeredEvent(const FPacingAdjustment& Adjustment);

    /**
     * Called to generate custom pacing summary (override in Blueprint)
     * @param Metrics The pacing metrics
     * @return Custom pacing summary
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Rhythm Events")
    FString GenerateCustomPacingSummary(const FPacingMetrics& Metrics);

    /**
     * Called to create custom pacing adjustment (override in Blueprint)
     * @param Analysis The pacing analysis
     * @return Custom pacing adjustment
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Rhythm Events")
    FPacingAdjustment CreateCustomPacingAdjustment(const FPacingAnalysis& Analysis);
};
