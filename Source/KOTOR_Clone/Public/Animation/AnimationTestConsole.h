// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Animation/VOPerformanceIntegrationComponent.h"
#include "Animation/MetaHumanFacialAnimationComponent.h"
#include "Animation/AnimationTagMap.h"
#include "AnimationTestConsole.generated.h"

/**
 * Animation test preset
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAnimationTestPreset
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    FString PresetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    EPerformanceEmotion Emotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    EPerformanceTone Tone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    FString TestLine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    float IntensityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    bool bIncludeVO;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Test Preset")
    bool bIncludeFacialAnimation;

    FAnimationTestPreset()
    {
        PresetName = TEXT("Default Test");
        Emotion = EPerformanceEmotion::Neutral;
        Tone = EPerformanceTone::Normal;
        TestLine = TEXT("This is a test line.");
        IntensityLevel = 0.5f;
        bIncludeVO = true;
        bIncludeFacialAnimation = true;
    }
};

/**
 * Animation test result
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAnimationTestResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Animation Test Result")
    bool bSuccess;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Test Result")
    FString TestID;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Test Result")
    UAnimMontage* SelectedMontage;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Test Result")
    float TestDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Test Result")
    FString ErrorMessage;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Test Result")
    FDateTime TestTimestamp;

    FAnimationTestResult()
    {
        bSuccess = false;
        TestID = TEXT("");
        SelectedMontage = nullptr;
        TestDuration = 0.0f;
        ErrorMessage = TEXT("");
        TestTimestamp = FDateTime::Now();
    }
};

/**
 * Animation test events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimationTestStarted, const FAnimationTestPreset&, Preset, const FAnimationTestResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimationTestCompleted, const FAnimationTestResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPresetChanged, const FString&, OldPreset, const FString&, NewPreset);

/**
 * Animation Test Console - Blueprint console for testing emotion/tone/line combinations
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AAnimationTestConsole : public AActor
{
    GENERATED_BODY()

public:
    AAnimationTestConsole();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Initialize animation test console
     * @param TestActor Actor to test animations on
     * @param TagMap Animation tag map to use
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    void InitializeTestConsole(AActor* TestActor, UAnimationTagMap* TagMap);

    /**
     * Run animation test
     * @param Preset Test preset to run
     * @return Animation test result
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    FAnimationTestResult RunAnimationTest(const FAnimationTestPreset& Preset);

    /**
     * Run quick test with parameters
     * @param Emotion Emotion to test
     * @param Tone Tone to test
     * @param TestLine Test dialogue line
     * @param bIncludeVO Whether to include voice synthesis
     * @return Animation test result
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    FAnimationTestResult RunQuickTest(EPerformanceEmotion Emotion, EPerformanceTone Tone, 
                                     const FString& TestLine, bool bIncludeVO = true);

    /**
     * Stop current test
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    void StopCurrentTest();

    /**
     * Add test preset
     * @param Preset Preset to add
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    void AddTestPreset(const FAnimationTestPreset& Preset);

    /**
     * Remove test preset
     * @param PresetName Name of preset to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    void RemoveTestPreset(const FString& PresetName);

    /**
     * Load test preset
     * @param PresetName Name of preset to load
     * @return Loaded preset data
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    FAnimationTestPreset LoadTestPreset(const FString& PresetName);

    /**
     * Get all preset names
     * @return Array of preset names
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Test Console")
    TArray<FString> GetAllPresetNames() const;

    /**
     * Run all presets
     * @return Number of tests started
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    int32 RunAllPresets();

    /**
     * Set test actor
     * @param TestActor Actor to test animations on
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    void SetTestActor(AActor* TestActor);

    /**
     * Get test results
     * @return Array of all test results
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Test Console")
    TArray<FAnimationTestResult> GetTestResults() const { return TestResults; }

    /**
     * Clear test results
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    void ClearTestResults();

    /**
     * Export test results
     * @param FilePath Path to export to
     * @return True if export was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Test Console")
    bool ExportTestResults(const FString& FilePath);

    /**
     * Check if test is running
     * @return True if test is currently running
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Test Console")
    bool IsTestRunning() const { return bTestRunning; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Animation Test Events")
    FOnAnimationTestStarted OnAnimationTestStarted;

    UPROPERTY(BlueprintAssignable, Category = "Animation Test Events")
    FOnAnimationTestCompleted OnAnimationTestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Animation Test Events")
    FOnPresetChanged OnPresetChanged;

protected:
    // Console components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Console Components")
    UStaticMeshComponent* ConsoleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Console Components")
    UWidgetComponent* ConsoleWidget;

    // Test presets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Presets")
    TArray<FAnimationTestPreset> TestPresets;

    // Test results
    UPROPERTY(BlueprintReadOnly, Category = "Test Results")
    TArray<FAnimationTestResult> TestResults;

    // Current test state
    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    bool bTestRunning;

    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    FAnimationTestPreset CurrentPreset;

    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    FString CurrentTestID;

    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    float TestStartTime;

    // Component references
    UPROPERTY()
    AActor* TestActor;

    UPROPERTY()
    UAnimationTagMap* AnimationTagMap;

    UPROPERTY()
    UProceduralPerformanceComponentV2* PerformanceComponent;

    UPROPERTY()
    UVOPerformanceIntegrationComponent* VOIntegrationComponent;

    UPROPERTY()
    UMetaHumanFacialAnimationComponent* FacialAnimationComponent;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Console Settings")
    bool bAutoRunTests; // Automatically run tests when preset changes

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Console Settings")
    float TestTimeout; // Maximum time for a test

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Console Settings")
    bool bLogTestResults; // Log test results to console

    // Timer handle
    FTimerHandle TestTimeoutTimer;

private:
    // Helper methods
    void LoadDefaultPresets();
    FString GenerateTestID();
    void SetupTestActor();
    void StartTest(const FAnimationTestPreset& Preset);
    void CompleteTest(bool bSuccess, const FString& ErrorMessage = TEXT(""));
    void LogTestResult(const FAnimationTestResult& Result);

    // Timer callback
    UFUNCTION()
    void OnTestTimeout();

    // Component events
    UFUNCTION()
    void OnPerformanceStarted(const FPerformanceMetadata& Metadata, const FPerformanceResult& Result);

    UFUNCTION()
    void OnPerformanceCompleted(const FString& PerformanceID);

    UFUNCTION()
    void OnVOPerformanceStarted(const FVOPerformanceData& VOData, const FVOPerformanceResult& Result);

    UFUNCTION()
    void OnVOPerformanceCompleted(const FString& SessionID);

public:
    /**
     * Blueprint implementable events for custom test console logic
     */

    /**
     * Called when animation test starts (for custom setup)
     * @param Preset The test preset
     * @param Result The test result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Test Events")
    void OnAnimationTestStartedEvent(const FAnimationTestPreset& Preset, const FAnimationTestResult& Result);

    /**
     * Called when animation test completes (for custom processing)
     * @param Result The test result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Test Events")
    void OnAnimationTestCompletedEvent(const FAnimationTestResult& Result);

    /**
     * Called to customize test preset (override in Blueprint)
     * @param BasePreset Base preset
     * @return Customized preset
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Test Events")
    FAnimationTestPreset CustomizeTestPreset(const FAnimationTestPreset& BasePreset);

    /**
     * Called to validate test parameters (override in Blueprint)
     * @param Preset Preset to validate
     * @return True if preset is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Test Events")
    bool ValidateTestPreset(const FAnimationTestPreset& Preset);

    /**
     * Called to generate test report (override in Blueprint)
     * @param Results Array of test results
     * @return Custom test report
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Test Events")
    FString GenerateTestReport(const TArray<FAnimationTestResult>& Results);

#if WITH_EDITOR
    // Editor-only functions
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
