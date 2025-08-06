// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Audio/ProceduralMusicSubsystem.h"
#include "Audio/MusicBlendingComponent.h"
#include "Audio/MusicGameplayIntegrationComponent.h"
#include "MusicDebuggerWidget.generated.h"

/**
 * Music layer debug info
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicLayerDebugInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    FString LayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    EMusicLayerType LayerType;

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    float CurrentVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    float TargetVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    bool bIsBlending;

    UPROPERTY(BlueprintReadWrite, Category = "Layer Debug Info")
    FString AudioFileName;

    FMusicLayerDebugInfo()
    {
        LayerID = TEXT("");
        LayerType = EMusicLayerType::Ambient;
        bIsActive = false;
        CurrentVolume = 0.0f;
        TargetVolume = 0.0f;
        bIsBlending = false;
        AudioFileName = TEXT("");
    }
};

/**
 * Music debug log entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicDebugLogEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Debug Log Entry")
    FString Timestamp;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Log Entry")
    FString EventType;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Log Entry")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Log Entry")
    FString LayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Log Entry")
    float Value;

    FMusicDebugLogEntry()
    {
        Timestamp = TEXT("");
        EventType = TEXT("");
        Description = TEXT("");
        LayerID = TEXT("");
        Value = 0.0f;
    }
};

/**
 * Music Debugger Widget - Toggle/test music layers live in editor with debug interface
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMusicDebuggerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UMusicDebuggerWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize music debugger
     * @param MusicSubsystem Procedural music subsystem
     * @param BlendingComponent Music blending component
     * @param IntegrationComponent Music integration component
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void InitializeMusicDebugger(UProceduralMusicSubsystem* MusicSubsystem,
                                UMusicBlendingComponent* BlendingComponent,
                                UMusicGameplayIntegrationComponent* IntegrationComponent);

    /**
     * Refresh debugger data
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void RefreshDebuggerData();

    /**
     * Set layer volume
     * @param LayerID ID of layer
     * @param Volume Volume to set (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void SetLayerVolume(const FString& LayerID, float Volume);

    /**
     * Toggle layer active state
     * @param LayerID ID of layer to toggle
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void ToggleLayer(const FString& LayerID);

    /**
     * Force music mood
     * @param Mood Mood to force
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void ForceMusicMood(EMusicMood Mood);

    /**
     * Force music context
     * @param Context Context to force
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void ForceMusicContext(EMusicContext Context);

    /**
     * Test combat mode
     * @param bEnable Whether to enable combat mode
     * @param Intensity Combat intensity (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void TestCombatMode(bool bEnable, float Intensity = 0.8f);

    /**
     * Test alignment transition
     * @param TargetAlignment Target alignment (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void TestAlignmentTransition(float TargetAlignment);

    /**
     * Trigger music event
     * @param TriggerType Type of trigger to test
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void TriggerMusicEvent(EMusicTriggerType TriggerType);

    /**
     * Stop all music
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void StopAllMusic();

    /**
     * Reset music system
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void ResetMusicSystem();

    /**
     * Export debug log
     * @param FilePath Path to export to
     * @return True if export was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    bool ExportDebugLog(const FString& FilePath);

    /**
     * Clear debug log
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debugger")
    void ClearDebugLog();

protected:
    // UI Components - Layer Control
    UPROPERTY(meta = (BindWidget))
    UListView* LayerListView;

    UPROPERTY(meta = (BindWidget))
    USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    UButton* StopAllButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResetSystemButton;

    // UI Components - Music State
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* MoodComboBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* ContextComboBox;

    UPROPERTY(meta = (BindWidget))
    USlider* IntensitySlider;

    UPROPERTY(meta = (BindWidget))
    USlider* TensionSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* EnergySlider;

    // UI Components - Test Controls
    UPROPERTY(meta = (BindWidget))
    UCheckBox* CombatModeCheckBox;

    UPROPERTY(meta = (BindWidget))
    USlider* CombatIntensitySlider;

    UPROPERTY(meta = (BindWidget))
    USlider* AlignmentSlider;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* TriggerEventComboBox;

    UPROPERTY(meta = (BindWidget))
    UButton* TriggerEventButton;

    // UI Components - Status Display
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentMoodText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentContextText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ActiveLayersText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BlendingLayersText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* MasterVolumeProgressBar;

    // UI Components - Debug Log
    UPROPERTY(meta = (BindWidget))
    UListView* DebugLogListView;

    UPROPERTY(meta = (BindWidget))
    UButton* ClearLogButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ExportLogButton;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ExportPathTextBox;

    // UI Components - Performance
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PerformanceStatsText;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* AutoRefreshCheckBox;

    // Data
    UPROPERTY(BlueprintReadOnly, Category = "Debug Data")
    TArray<FMusicLayerDebugInfo> LayerDebugInfo;

    UPROPERTY(BlueprintReadOnly, Category = "Debug Data")
    TArray<FMusicDebugLogEntry> DebugLog;

    // Component references
    UPROPERTY()
    UProceduralMusicSubsystem* ProceduralMusicSubsystem;

    UPROPERTY()
    UMusicBlendingComponent* MusicBlendingComponent;

    UPROPERTY()
    UMusicGameplayIntegrationComponent* MusicIntegrationComponent;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugger Settings")
    bool bAutoRefresh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugger Settings")
    float RefreshInterval; // Seconds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugger Settings")
    int32 MaxLogEntries;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Debugger State")
    bool bDebuggerActive;

    UPROPERTY(BlueprintReadOnly, Category = "Debugger State")
    float LastRefreshTime;

    // Timer handle
    FTimerHandle RefreshTimer;

private:
    // Helper methods
    void PopulateComboBoxes();
    void UpdateLayerList();
    void UpdateStatusDisplay();
    void UpdatePerformanceStats();
    void AddDebugLogEntry(const FString& EventType, const FString& Description, const FString& LayerID = TEXT(""), float Value = 0.0f);
    void TrimDebugLog();

    // UI event handlers
    UFUNCTION()
    void OnMasterVolumeSliderChanged(float Value);

    UFUNCTION()
    void OnStopAllButtonClicked();

    UFUNCTION()
    void OnResetSystemButtonClicked();

    UFUNCTION()
    void OnMoodComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnContextComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnIntensitySliderChanged(float Value);

    UFUNCTION()
    void OnTensionSliderChanged(float Value);

    UFUNCTION()
    void OnEnergySliderChanged(float Value);

    UFUNCTION()
    void OnCombatModeCheckBoxChanged(bool bIsChecked);

    UFUNCTION()
    void OnCombatIntensitySliderChanged(float Value);

    UFUNCTION()
    void OnAlignmentSliderChanged(float Value);

    UFUNCTION()
    void OnTriggerEventButtonClicked();

    UFUNCTION()
    void OnClearLogButtonClicked();

    UFUNCTION()
    void OnExportLogButtonClicked();

    UFUNCTION()
    void OnAutoRefreshCheckBoxChanged(bool bIsChecked);

    UFUNCTION()
    void OnLayerItemSelected(UObject* Item);

    // Music system events
    UFUNCTION()
    void OnMusicMoodChanged(EMusicMood OldMood, EMusicMood NewMood);

    UFUNCTION()
    void OnMusicContextChanged(EMusicContext OldContext, EMusicContext NewContext);

    UFUNCTION()
    void OnMusicLayerChanged(const FString& LayerID, bool bIsActive);

    UFUNCTION()
    void OnLayerBlendStarted(const FString& LayerID, float BlendDuration);

    UFUNCTION()
    void OnLayerBlendCompleted(const FString& LayerID, float FinalVolume);

    UFUNCTION()
    void OnMusicTriggerActivated(EMusicTriggerType TriggerType, const FMusicTriggerData& TriggerData);

    // Timer callback
    UFUNCTION()
    void OnRefreshTimer();

public:
    /**
     * Blueprint implementable events for custom music debugger logic
     */

    /**
     * Called when debugger is initialized (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debugger Events")
    void OnMusicDebuggerInitializedEvent();

    /**
     * Called when layer state changes (for custom UI updates)
     * @param LayerID ID of changed layer
     * @param bIsActive Whether layer is active
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debugger Events")
    void OnLayerStateChangedEvent(const FString& LayerID, bool bIsActive);

    /**
     * Called when music event is triggered (for custom logging)
     * @param TriggerType Type of trigger
     * @param TriggerData The trigger data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debugger Events")
    void OnMusicEventTriggeredEvent(EMusicTriggerType TriggerType, const FMusicTriggerData& TriggerData);

    /**
     * Called to customize layer display (override in Blueprint)
     * @param LayerInfo Layer debug info
     * @return Customized display text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debugger Events")
    FString CustomizeLayerDisplay(const FMusicLayerDebugInfo& LayerInfo);

    /**
     * Called to validate debug command (override in Blueprint)
     * @param Command Command being executed
     * @param Parameters Command parameters
     * @return True if command should be executed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debugger Events")
    bool ValidateDebugCommand(const FString& Command, const TMap<FString, FString>& Parameters);
};
