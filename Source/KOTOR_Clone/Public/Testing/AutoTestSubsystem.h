// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "NPCs/ProceduralNPCActor.h"
#include "Enemies/EnemyActor.h"
#include "Loot/LootPickupSystem.h"
#include "AutoTestSubsystem.generated.h"

/**
 * Test types
 */
UENUM(BlueprintType)
enum class EAutoTestType : uint8
{
    CampaignLoading     UMETA(DisplayName = "Campaign Loading"),
    NPCSpawning         UMETA(DisplayName = "NPC Spawning"),
    QuestSystem         UMETA(DisplayName = "Quest System"),
    EnemyEncounter      UMETA(DisplayName = "Enemy Encounter"),
    LootDrop            UMETA(DisplayName = "Loot Drop"),
    DialogueSystem      UMETA(DisplayName = "Dialogue System"),
    GalaxyMap           UMETA(DisplayName = "Galaxy Map"),
    MeditationSystem    UMETA(DisplayName = "Meditation System"),
    PhotoMode           UMETA(DisplayName = "Photo Mode"),
    SystemIntegration   UMETA(DisplayName = "System Integration")
};

/**
 * Test result
 */
UENUM(BlueprintType)
enum class ETestResult : uint8
{
    NotRun              UMETA(DisplayName = "Not Run"),
    Running             UMETA(DisplayName = "Running"),
    Passed              UMETA(DisplayName = "Passed"),
    Failed              UMETA(DisplayName = "Failed"),
    Skipped             UMETA(DisplayName = "Skipped"),
    Timeout             UMETA(DisplayName = "Timeout")
};

/**
 * Test case data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAutoTestCase
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    FString TestID;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    FString TestName;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    EAutoTestType TestType;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    TArray<FString> Prerequisites;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    float TimeoutSeconds;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    TMap<FString, FString> TestParameters;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    ETestResult Result;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    FString ErrorMessage;

    UPROPERTY(BlueprintReadWrite, Category = "Test Case")
    float ExecutionTime;

    FAutoTestCase()
    {
        TestID = TEXT("");
        TestName = TEXT("Unnamed Test");
        TestType = EAutoTestType::SystemIntegration;
        Description = TEXT("");
        TimeoutSeconds = 30.0f;
        Result = ETestResult::NotRun;
        ErrorMessage = TEXT("");
        ExecutionTime = 0.0f;
    }
};

/**
 * Test suite data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAutoTestSuite
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Test Suite")
    FString SuiteID;

    UPROPERTY(BlueprintReadWrite, Category = "Test Suite")
    FString SuiteName;

    UPROPERTY(BlueprintReadWrite, Category = "Test Suite")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Test Suite")
    TArray<FAutoTestCase> TestCases;

    UPROPERTY(BlueprintReadWrite, Category = "Test Suite")
    bool bRunInParallel;

    UPROPERTY(BlueprintReadWrite, Category = "Test Suite")
    float SuiteTimeoutSeconds;

    FAutoTestSuite()
    {
        SuiteID = TEXT("");
        SuiteName = TEXT("Unnamed Suite");
        Description = TEXT("");
        bRunInParallel = false;
        SuiteTimeoutSeconds = 300.0f;
    }
};

/**
 * Auto test events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTestSuiteStarted, const FAutoTestSuite&, TestSuite);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTestSuiteCompleted, const FAutoTestSuite&, TestSuite, int32, PassedTests);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTestCaseStarted, const FAutoTestCase&, TestCase, int32, TestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTestCaseCompleted, const FAutoTestCase&, TestCase, ETestResult, Result);

/**
 * Auto Test Subsystem - Automated testing harness for all KOTOR.ai systems
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UAutoTestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Run auto tests
     * @param bRunOnStartup Whether this is a startup test run
     * @return True if tests were started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    bool RunAutoTests(bool bRunOnStartup = false);

    /**
     * Run specific test suite
     * @param SuiteID ID of test suite to run
     * @return True if test suite was started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    bool RunTestSuite(const FString& SuiteID);

    /**
     * Run specific test case
     * @param TestID ID of test case to run
     * @return True if test case was started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    bool RunTestCase(const FString& TestID);

    /**
     * Stop all running tests
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    void StopAllTests();

    /**
     * Get test results
     * @return Map of test results
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Test")
    TMap<FString, ETestResult> GetTestResults() const;

    /**
     * Get test suite results
     * @param SuiteID ID of test suite
     * @return Test suite with results
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Test")
    FAutoTestSuite GetTestSuiteResults(const FString& SuiteID) const;

    /**
     * Check if tests are running
     * @return True if any tests are currently running
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Test")
    bool AreTestsRunning() const { return bTestsRunning; }

    /**
     * Get test progress
     * @return Progress from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Test")
    float GetTestProgress() const;

    /**
     * Add custom test case
     * @param TestCase Test case to add
     * @param SuiteID ID of suite to add to
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    void AddCustomTestCase(const FAutoTestCase& TestCase, const FString& SuiteID = TEXT("custom"));

    /**
     * Remove test case
     * @param TestID ID of test case to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    void RemoveTestCase(const FString& TestID);

    /**
     * Enable auto-testing on startup
     * @param bEnabled Whether to enable auto-testing
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Test")
    void SetAutoTestOnStartup(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Auto Test Events")
    FOnTestSuiteStarted OnTestSuiteStarted;

    UPROPERTY(BlueprintAssignable, Category = "Auto Test Events")
    FOnTestSuiteCompleted OnTestSuiteCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Auto Test Events")
    FOnTestCaseStarted OnTestCaseStarted;

    UPROPERTY(BlueprintAssignable, Category = "Auto Test Events")
    FOnTestCaseCompleted OnTestCaseCompleted;

protected:
    // Test suites
    UPROPERTY(BlueprintReadOnly, Category = "Test Suites")
    TArray<FAutoTestSuite> TestSuites;

    // Test state
    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    bool bTestsRunning;

    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    int32 CurrentTestIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    int32 TotalTestCount;

    UPROPERTY(BlueprintReadOnly, Category = "Test State")
    FString CurrentSuiteID;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Test Settings")
    bool bAutoTestOnStartup;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Test Settings")
    bool bLogTestResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Test Settings")
    float DefaultTestTimeout;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    // Timer handles
    FTimerHandle TestTimer;
    FTimerHandle TimeoutTimer;

private:
    // Helper methods
    void SetupDefaultTestSuites();
    void CreateCoreTestSuite();
    void CreateSystemTestSuite();
    void CreateIntegrationTestSuite();

    // Test execution
    void ExecuteNextTest();
    void ExecuteTestCase(const FAutoTestCase& TestCase);
    void CompleteTestCase(const FString& TestID, ETestResult Result, const FString& ErrorMessage = TEXT(""));

    // Individual test implementations
    bool TestCampaignLoading();
    bool TestNPCSpawning();
    bool TestQuestSystem();
    bool TestEnemyEncounter();
    bool TestLootDrop();
    bool TestDialogueSystem();
    bool TestGalaxyMap();
    bool TestMeditationSystem();
    bool TestPhotoMode();
    bool TestSystemIntegration();

    // Test utilities
    APawn* GetTestPawn();
    FVector GetTestLocation();
    void CleanupTestActors();

    // Timer callbacks
    UFUNCTION()
    void OnTestTimer();

    UFUNCTION()
    void OnTestTimeout();

public:
    /**
     * Blueprint implementable events for custom testing logic
     */

    /**
     * Called when test suite starts (for custom setup)
     * @param TestSuite The test suite being started
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Auto Test Events")
    void OnTestSuiteStartedEvent(const FAutoTestSuite& TestSuite);

    /**
     * Called when test case starts (for custom setup)
     * @param TestCase The test case being started
     * @param TestIndex Index of the test case
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Auto Test Events")
    void OnTestCaseStartedEvent(const FAutoTestCase& TestCase, int32 TestIndex);

    /**
     * Called when test case completes (for custom processing)
     * @param TestCase The completed test case
     * @param Result The test result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Auto Test Events")
    void OnTestCaseCompletedEvent(const FAutoTestCase& TestCase, ETestResult Result);

    /**
     * Called to execute custom test (override in Blueprint)
     * @param TestCase The test case to execute
     * @return True if test passed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Auto Test Events")
    bool ExecuteCustomTest(const FAutoTestCase& TestCase);

    /**
     * Called to validate test prerequisites (override in Blueprint)
     * @param TestCase The test case to validate
     * @return True if prerequisites are met
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Auto Test Events")
    bool ValidateTestPrerequisites(const FAutoTestCase& TestCase);

    /**
     * Called to generate test report (override in Blueprint)
     * @param TestSuite The completed test suite
     * @param PassedTests Number of passed tests
     * @return Custom test report
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Auto Test Events")
    FString GenerateCustomTestReport(const FAutoTestSuite& TestSuite, int32 PassedTests);
};
