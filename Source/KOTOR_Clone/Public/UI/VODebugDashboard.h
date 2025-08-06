// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "VODebugDashboard.generated.h"

/**
 * VO test entry data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FVOTestEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FString TestID;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FString TestName;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FString SpeakerName;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FString DialogueText;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FVoiceCharacterData VoiceData;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    EVoiceSynthesisMode SynthesisMode;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    bool bIsPlaying;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    bool bHasError;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FString ErrorMessage;

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    float GenerationTime; // Time taken to generate

    UPROPERTY(BlueprintReadWrite, Category = "VO Test Entry")
    FDateTime CreatedTime;

    FVOTestEntry()
    {
        TestID = TEXT("");
        TestName = TEXT("Unnamed Test");
        SpeakerName = TEXT("Test Speaker");
        DialogueText = TEXT("This is a test line.");
        SynthesisMode = EVoiceSynthesisMode::Streaming;
        bIsPlaying = false;
        bHasError = false;
        ErrorMessage = TEXT("");
        GenerationTime = 0.0f;
        CreatedTime = FDateTime::Now();
    }
};

/**
 * VO provider status
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FVOProviderStatus
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    EVoiceProvider Provider;

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    bool bIsAvailable;

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    bool bIsConnected;

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    FString StatusMessage;

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    float ResponseTime; // Average response time in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    int32 RequestsToday; // Number of requests made today

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    int32 RequestLimit; // Daily request limit

    UPROPERTY(BlueprintReadWrite, Category = "Provider Status")
    float CostPerRequest; // Cost per request

    FVOProviderStatus()
    {
        Provider = EVoiceProvider::ElevenLabs;
        bIsAvailable = false;
        bIsConnected = false;
        StatusMessage = TEXT("Not tested");
        ResponseTime = 0.0f;
        RequestsToday = 0;
        RequestLimit = 1000;
        CostPerRequest = 0.01f;
    }
};

/**
 * VO debug events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVOTestStarted, const FVOTestEntry&, TestEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVOTestCompleted, const FString&, TestID, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVOProviderTested, const FVOProviderStatus&, ProviderStatus);

/**
 * VO Debug Dashboard - Developer tools for voice synthesis testing and management
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UVODebugDashboard : public UUserWidget
{
    GENERATED_BODY()

public:
    UVODebugDashboard(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize VO debug dashboard
     * @param VoiceComponent Voice synthesis component to debug
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void InitializeVODebugDashboard(UVoiceSynthesisComponent* VoiceComponent);

    /**
     * Add test entry
     * @param TestEntry Test entry to add
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void AddTestEntry(const FVOTestEntry& TestEntry);

    /**
     * Remove test entry
     * @param TestID ID of test to remove
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void RemoveTestEntry(const FString& TestID);

    /**
     * Run voice test
     * @param TestID ID of test to run
     * @return True if test started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    bool RunVoiceTest(const FString& TestID);

    /**
     * Run all tests
     * @return Number of tests started
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    int32 RunAllTests();

    /**
     * Stop all tests
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void StopAllTests();

    /**
     * Test voice provider
     * @param Provider Provider to test
     * @return True if test started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    bool TestVoiceProvider(EVoiceProvider Provider);

    /**
     * Test all providers
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void TestAllProviders();

    /**
     * Clear voice cache
     * @param SpeakerName Speaker to clear (empty = all)
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void ClearVoiceCache(const FString& SpeakerName = TEXT(""));

    /**
     * Export test results
     * @param FilePath Path to export to
     * @return True if export was successful
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    bool ExportTestResults(const FString& FilePath);

    /**
     * Import test entries
     * @param FilePath Path to import from
     * @return Number of entries imported
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    int32 ImportTestEntries(const FString& FilePath);

    /**
     * Get provider status
     * @param Provider Provider to check
     * @return Provider status
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    FVOProviderStatus GetProviderStatus(EVoiceProvider Provider) const;

    /**
     * Refresh dashboard data
     */
    UFUNCTION(BlueprintCallable, Category = "VO Debug")
    void RefreshDashboard();

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "VO Debug Events")
    FOnVOTestStarted OnVOTestStarted;

    UPROPERTY(BlueprintAssignable, Category = "VO Debug Events")
    FOnVOTestCompleted OnVOTestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "VO Debug Events")
    FOnVOProviderTested OnVOProviderTested;

protected:
    // UI Components - Test Management
    UPROPERTY(meta = (BindWidget))
    UListView* TestEntriesListView;

    UPROPERTY(meta = (BindWidget))
    UButton* AddTestButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RemoveTestButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RunTestButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RunAllTestsButton;

    UPROPERTY(meta = (BindWidget))
    UButton* StopAllTestsButton;

    // UI Components - Test Creation
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* TestNameTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* SpeakerNameTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* DialogueTextBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* VoiceProviderComboBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* SynthesisModeComboBox;

    UPROPERTY(meta = (BindWidget))
    USlider* PitchSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* SpeedSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* EmotionSlider;

    // UI Components - Provider Status
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ElevenLabsStatusText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AzureStatusText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* OpenTTSStatusText;

    UPROPERTY(meta = (BindWidget))
    UButton* TestProvidersButton;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ProviderTestProgressBar;

    // UI Components - Cache Management
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CacheSizeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CacheEntriesText;

    UPROPERTY(meta = (BindWidget))
    UButton* ClearCacheButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RefreshCacheButton;

    // UI Components - Statistics
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TotalTestsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SuccessfulTestsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* FailedTestsText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AverageGenerationTimeText;

    // UI Components - Export/Import
    UPROPERTY(meta = (BindWidget))
    UButton* ExportResultsButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ImportTestsButton;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ExportPathTextBox;

    // Data
    UPROPERTY(BlueprintReadOnly, Category = "VO Debug Data")
    TArray<FVOTestEntry> TestEntries;

    UPROPERTY(BlueprintReadOnly, Category = "VO Debug Data")
    TArray<FVOProviderStatus> ProviderStatuses;

    // Component reference
    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisComponent;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Debug State")
    bool bTestsRunning;

    UPROPERTY(BlueprintReadOnly, Category = "Debug State")
    int32 RunningTestCount;

    UPROPERTY(BlueprintReadOnly, Category = "Debug State")
    int32 CompletedTestCount;

    UPROPERTY(BlueprintReadOnly, Category = "Debug State")
    int32 FailedTestCount;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
    bool bAutoRefresh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
    float RefreshInterval; // Seconds

    // Timer handle
    FTimerHandle RefreshTimer;

private:
    // Helper methods
    void PopulateComboBoxes();
    void UpdateTestList();
    void UpdateProviderStatuses();
    void UpdateCacheInfo();
    void UpdateStatistics();
    FVOTestEntry CreateTestEntryFromUI();
    void SelectTestEntry(const FString& TestID);
    void UpdateTestEntryInList(const FVOTestEntry& TestEntry);

    // Test management
    FVOTestEntry* FindTestEntry(const FString& TestID);
    void OnTestCompleted(const FString& TestID, bool bSuccess, const FString& ErrorMessage = TEXT(""));

    // Provider testing
    void TestProviderConnection(EVoiceProvider Provider);
    void UpdateProviderStatus(EVoiceProvider Provider, bool bSuccess, float ResponseTime, const FString& Message);

    // UI event handlers
    UFUNCTION()
    void OnAddTestButtonClicked();

    UFUNCTION()
    void OnRemoveTestButtonClicked();

    UFUNCTION()
    void OnRunTestButtonClicked();

    UFUNCTION()
    void OnRunAllTestsButtonClicked();

    UFUNCTION()
    void OnStopAllTestsButtonClicked();

    UFUNCTION()
    void OnTestProvidersButtonClicked();

    UFUNCTION()
    void OnClearCacheButtonClicked();

    UFUNCTION()
    void OnRefreshCacheButtonClicked();

    UFUNCTION()
    void OnExportResultsButtonClicked();

    UFUNCTION()
    void OnImportTestsButtonClicked();

    UFUNCTION()
    void OnTestEntrySelected(UObject* Item);

    // Voice synthesis events
    UFUNCTION()
    void OnVoiceSynthesisCompleted(const FString& RequestID, USoundWave* GeneratedAudio);

    UFUNCTION()
    void OnVoiceSynthesisFailed(const FString& RequestID, const FString& ErrorMessage);

    // Timer callback
    UFUNCTION()
    void OnRefreshTimer();

public:
    /**
     * Blueprint implementable events for custom VO debug logic
     */

    /**
     * Called when VO test starts (for custom monitoring)
     * @param TestEntry The test entry being started
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Debug Events")
    void OnVOTestStartedEvent(const FVOTestEntry& TestEntry);

    /**
     * Called when VO test completes (for custom processing)
     * @param TestID ID of completed test
     * @param bSuccess Whether test was successful
     * @param ErrorMessage Error message if failed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Debug Events")
    void OnVOTestCompletedEvent(const FString& TestID, bool bSuccess, const FString& ErrorMessage);

    /**
     * Called when provider is tested (for custom status updates)
     * @param ProviderStatus The provider status result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Debug Events")
    void OnVOProviderTestedEvent(const FVOProviderStatus& ProviderStatus);

    /**
     * Called to create custom test entry (override in Blueprint)
     * @return Custom test entry
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Debug Events")
    FVOTestEntry CreateCustomTestEntry();

    /**
     * Called to validate test parameters (override in Blueprint)
     * @param TestEntry Test entry to validate
     * @return True if test entry is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Debug Events")
    bool ValidateTestEntry(const FVOTestEntry& TestEntry);

    /**
     * Called to customize provider test (override in Blueprint)
     * @param Provider Provider to test
     * @return Custom test parameters
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "VO Debug Events")
    TMap<FString, FString> CustomizeProviderTest(EVoiceProvider Provider);
};
