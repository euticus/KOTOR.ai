// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/ProgressBar.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "ScenarioTestingWidget.generated.h"

/**
 * Test scenario types
 */
UENUM(BlueprintType)
enum class EScenarioType : uint8
{
    Combat          UMETA(DisplayName = "Combat Encounter"),
    Dialogue        UMETA(DisplayName = "Dialogue Scene"),
    Quest           UMETA(DisplayName = "Quest Sequence"),
    Exploration     UMETA(DisplayName = "Exploration Area"),
    Story           UMETA(DisplayName = "Story Event"),
    Mixed           UMETA(DisplayName = "Mixed Scenario"),
    Custom          UMETA(DisplayName = "Custom Scenario")
};

/**
 * Test scenario parameters
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FScenarioTestParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FString ScenarioName;

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    EScenarioType Type;

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    int32 DurationMinutes; // Expected duration

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FString Biome; // Environment type

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FString DifficultyTier; // early, mid, late

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    TArray<FString> RequiredNPCs; // NPCs needed for scenario

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    TArray<FString> RequiredEnemies; // Enemies needed

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    TMap<FString, FString> CustomParameters; // Additional parameters

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    bool bAutoGenerate; // Whether to auto-generate content

    FScenarioTestParams()
    {
        ScenarioName = TEXT("Test Scenario");
        Type = EScenarioType::Mixed;
        Description = TEXT("A test scenario for AIDM validation");
        DurationMinutes = 15;
        Biome = TEXT("urban");
        DifficultyTier = TEXT("early");
        bAutoGenerate = true;
    }
};

/**
 * Test result data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FScenarioTestResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    FString ScenarioName;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    bool bTestPassed;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    float ExecutionTime; // How long the test took

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    int32 NPCsSpawned;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    int32 EnemiesSpawned;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    int32 QuestsGenerated;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    TArray<FString> ErrorMessages;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    TArray<FString> WarningMessages;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    TMap<FString, float> PerformanceMetrics;

    UPROPERTY(BlueprintReadWrite, Category = "Test Result")
    FString GeneratedContent; // JSON of generated content

    FScenarioTestResult()
    {
        ScenarioName = TEXT("");
        bTestPassed = false;
        ExecutionTime = 0.0f;
        NPCsSpawned = 0;
        EnemiesSpawned = 0;
        QuestsGenerated = 0;
        GeneratedContent = TEXT("");
    }
};

/**
 * Scenario testing events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScenarioTestStarted, const FScenarioTestParams&, TestParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScenarioTestCompleted, const FScenarioTestResult&, TestResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScenarioTestProgress, const FString&, ScenarioName, float, Progress);

/**
 * Scenario Testing Widget - Development tool for generating and testing AIDM scenarios
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UScenarioTestingWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UScenarioTestingWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize scenario testing system
     * @param CampaignLoader Campaign loader for content generation
     * @param AIDirector AI Director for scenario execution
     * @param QuestManager Quest manager for quest testing
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    void InitializeScenarioTesting(UCampaignLoaderSubsystem* CampaignLoader, 
                                  UAIDirectorComponent* AIDirector, 
                                  UQuestManagerComponent* QuestManager);

    /**
     * Generate a test scenario
     * @param TestParams Parameters for scenario generation
     * @return Generated scenario ID
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    FString GenerateTestScenario(const FScenarioTestParams& TestParams);

    /**
     * Execute a test scenario
     * @param ScenarioID ID of the scenario to execute
     * @return True if execution started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    bool ExecuteTestScenario(const FString& ScenarioID);

    /**
     * Run automated test suite
     * @param TestSuiteName Name of the test suite to run
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    void RunAutomatedTestSuite(const FString& TestSuiteName = TEXT("Default"));

    /**
     * Create quick test scenario
     * @param Type Type of scenario to create
     * @param DurationMinutes Expected duration
     * @return Generated scenario ID
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    FString CreateQuickTestScenario(EScenarioType Type, int32 DurationMinutes = 15);

    /**
     * Validate scenario content
     * @param ScenarioID ID of the scenario to validate
     * @return Validation result
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    FScenarioTestResult ValidateScenario(const FString& ScenarioID);

    /**
     * Get test result
     * @param ScenarioID ID of the scenario
     * @return Test result data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scenario Testing")
    FScenarioTestResult GetTestResult(const FString& ScenarioID) const;

    /**
     * Get all test results
     * @return Array of all test results
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scenario Testing")
    TArray<FScenarioTestResult> GetAllTestResults() const;

    /**
     * Clear test results
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    void ClearTestResults();

    /**
     * Export test results to file
     * @param FilePath Path to export file
     * @return True if export was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    bool ExportTestResults(const FString& FilePath);

    /**
     * Import test scenarios from file
     * @param FilePath Path to import file
     * @return True if import was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    bool ImportTestScenarios(const FString& FilePath);

    /**
     * Check if test is currently running
     * @return True if a test is in progress
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scenario Testing")
    bool IsTestRunning() const { return bTestInProgress; }

    /**
     * Get current test progress (0.0 to 1.0)
     * @return Current test progress
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scenario Testing")
    float GetTestProgress() const { return CurrentTestProgress; }

    /**
     * Stop current test
     */
    UFUNCTION(BlueprintCallable, Category = "Scenario Testing")
    void StopCurrentTest();

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Scenario Testing Events")
    FOnScenarioTestStarted OnScenarioTestStarted;

    UPROPERTY(BlueprintAssignable, Category = "Scenario Testing Events")
    FOnScenarioTestCompleted OnScenarioTestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Scenario Testing Events")
    FOnScenarioTestProgress OnScenarioTestProgress;

protected:
    // UI Components (to be bound in Blueprint)
    
    // Scenario Creation Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UEditableTextBox* ScenarioNameInput;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UComboBoxString* ScenarioTypeSelector;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UEditableTextBox* ScenarioDescriptionInput;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UEditableTextBox* DurationInput;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UComboBoxString* BiomeSelector;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UComboBoxString* DifficultySelector;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Creation")
    class UCheckBox* AutoGenerateCheckbox;

    // Control Buttons
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* GenerateScenarioButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* ExecuteTestButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* RunTestSuiteButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* StopTestButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* ClearResultsButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* ExportResultsButton;

    // Test Progress Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Progress")
    class UTextBlock* CurrentTestText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Progress")
    class UProgressBar* TestProgressBar;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Progress")
    class UTextBlock* TestStatusText;

    // Results Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Results")
    class UScrollBox* TestResultsScrollBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Results")
    class UTextBlock* TestSummaryText;

    // Log Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Log")
    class UScrollBox* TestLogScrollBox;

    // Test data
    UPROPERTY(BlueprintReadOnly, Category = "Scenario Testing")
    TMap<FString, FScenarioTestParams> TestScenarios;

    UPROPERTY(BlueprintReadOnly, Category = "Scenario Testing")
    TMap<FString, FScenarioTestResult> TestResults;

    // Current test state
    UPROPERTY(BlueprintReadOnly, Category = "Scenario Testing")
    bool bTestInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Scenario Testing")
    FString CurrentTestScenario;

    UPROPERTY(BlueprintReadOnly, Category = "Scenario Testing")
    float CurrentTestProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Scenario Testing")
    float TestStartTime;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UAIDirectorComponent* AIDirectorRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    // Test settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Settings")
    float TestTimeoutSeconds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Settings")
    bool bAutoCleanupAfterTest;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Settings")
    int32 MaxConcurrentTests;

    // Predefined test suites
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Settings")
    TMap<FString, TArray<FScenarioTestParams>> TestSuites;

private:
    // Helper methods
    void RefreshUI();
    void UpdateTestProgress();
    void CompleteCurrentTest(bool bSuccess, const FString& ErrorMessage = TEXT(""));
    FString GenerateScenarioID();
    void AddTestLogEntry(const FString& Message, const FString& LogLevel = TEXT("Info"));
    void CreateDefaultTestSuites();
    FScenarioTestParams CreateCombatScenario(int32 Duration);
    FScenarioTestParams CreateDialogueScenario(int32 Duration);
    FScenarioTestParams CreateQuestScenario(int32 Duration);
    FScenarioTestParams CreateExplorationScenario(int32 Duration);

    // Button event handlers
    UFUNCTION()
    void OnGenerateScenarioClicked();

    UFUNCTION()
    void OnExecuteTestClicked();

    UFUNCTION()
    void OnRunTestSuiteClicked();

    UFUNCTION()
    void OnStopTestClicked();

    UFUNCTION()
    void OnClearResultsClicked();

    UFUNCTION()
    void OnExportResultsClicked();

    // Test execution methods
    void ExecuteCombatTest(const FScenarioTestParams& TestParams);
    void ExecuteDialogueTest(const FScenarioTestParams& TestParams);
    void ExecuteQuestTest(const FScenarioTestParams& TestParams);
    void ExecuteExplorationTest(const FScenarioTestParams& TestParams);
    void ExecuteCustomTest(const FScenarioTestParams& TestParams);

public:
    /**
     * Blueprint implementable events for custom testing logic
     */

    /**
     * Called when a test scenario is generated (for custom validation)
     * @param TestParams The generated test parameters
     * @return True if the scenario is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Testing Events")
    bool ValidateGeneratedScenario(const FScenarioTestParams& TestParams);

    /**
     * Called to execute custom test scenario (override in Blueprint)
     * @param TestParams The test parameters
     * @return Test result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Testing Events")
    FScenarioTestResult ExecuteCustomTestScenario(const FScenarioTestParams& TestParams);

    /**
     * Called when test is completed (for custom processing)
     * @param TestResult The test result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Testing Events")
    void OnTestCompletedEvent(const FScenarioTestResult& TestResult);

    /**
     * Called to generate custom test parameters (override in Blueprint)
     * @param Type The scenario type
     * @param Duration Expected duration
     * @return Custom test parameters
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Testing Events")
    FScenarioTestParams GenerateCustomTestParams(EScenarioType Type, int32 Duration);
};
