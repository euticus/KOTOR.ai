// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/ListView.h"
#include "Components/EditableTextBox.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Audio/RuntimeMusicLayeringComponent.h"
#include "Audio/AIDMNarrativeMusicLinker.h"
#include "MusicDebugPanel.generated.h"

/**
 * Music debug info for display
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicDebugInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    FString StemID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    EMusicStemType StemType;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    EMusicBiome Biome;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    EMusicTone Tone;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    float CurrentVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    float TargetVolume;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Info")
    EMusicLayerState LayerState;

    FMusicDebugInfo()
    {
        StemID = TEXT("");
        StemType = EMusicStemType::Ambient;
        Biome = EMusicBiome::Generic;
        Tone = EMusicTone::Neutral;
        bIsActive = false;
        CurrentVolume = 0.0f;
        TargetVolume = 0.0f;
        LayerState = EMusicLayerState::Inactive;
    }
};

/**
 * Music debug log entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicDebugLogEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Log")
    FString Timestamp;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Log")
    FString EventType;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Log")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Log")
    FString StemID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Debug Log")
    float Value;

    FMusicDebugLogEntry()
    {
        Timestamp = TEXT("");
        EventType = TEXT("");
        Description = TEXT("");
        StemID = TEXT("");
        Value = 0.0f;
    }
};

/**
 * Music Debug Panel - UMG debug panel with biome, tone, combat controls and volume sliders
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMusicDebugPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    UMusicDebugPanel(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize music debug panel
     * @param MusicSubsystem Procedural music subsystem
     * @param LayeringComponent Runtime music layering component
     * @param NarrativeLinker AIDM narrative music linker
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void InitializeMusicDebugPanel(UProceduralMusicSubsystemV2* MusicSubsystem,
                                  URuntimeMusicLayeringComponent* LayeringComponent,
                                  UAIDMNarrativeMusicLinker* NarrativeLinker);

    /**
     * Refresh debug panel data
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void RefreshDebugPanel();

    /**
     * Set biome
     * @param Biome Biome to set
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void SetBiome(EMusicBiome Biome);

    /**
     * Set tone
     * @param Tone Tone to set
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void SetTone(EMusicTone Tone);

    /**
     * Set combat mode
     * @param bCombatMode Whether combat mode is active
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void SetCombatMode(bool bCombatMode);

    /**
     * Set stem volume
     * @param StemID ID of stem
     * @param Volume Volume to set
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void SetStemVolume(const FString& StemID, float Volume);

    /**
     * Toggle stem active state
     * @param StemID ID of stem to toggle
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void ToggleStem(const FString& StemID);

    /**
     * Trigger narrative tag
     * @param NarrativeTag Tag to trigger
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void TriggerNarrativeTag(EAIDMNarrativeTag NarrativeTag);

    /**
     * Apply blend preset
     * @param PresetID ID of preset to apply
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void ApplyBlendPreset(const FString& PresetID);

    /**
     * Stop all music
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void StopAllMusic();

    /**
     * Reset music system
     */
    UFUNCTION(BlueprintCallable, Category = "Music Debug Panel")
    void ResetMusicSystem();

protected:
    // UI Components - Main Controls
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* BiomeComboBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* ToneComboBox;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* CombatModeCheckBox;

    UPROPERTY(meta = (BindWidget))
    USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    UButton* StopAllButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResetSystemButton;

    // UI Components - Stem Controls
    UPROPERTY(meta = (BindWidget))
    UListView* StemListView;

    UPROPERTY(meta = (BindWidget))
    UButton* RefreshStemsButton;

    // UI Components - Narrative Controls
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* NarrativeTagComboBox;

    UPROPERTY(meta = (BindWidget))
    UButton* TriggerNarrativeButton;

    UPROPERTY(meta = (BindWidget))
    USlider* PlayerMoralitySlider;

    UPROPERTY(meta = (BindWidget))
    USlider* StoryProgressionSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* EmotionalIntensitySlider;

    // UI Components - Blend Presets
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* BlendPresetComboBox;

    UPROPERTY(meta = (BindWidget))
    UButton* ApplyPresetButton;

    // UI Components - Status Display
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentBiomeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentToneText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CombatStatusText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ActiveStemsText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* MasterVolumeProgressBar;

    // UI Components - Debug Log
    UPROPERTY(meta = (BindWidget))
    UListView* DebugLogListView;

    UPROPERTY(meta = (BindWidget))
    UButton* ClearLogButton;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* AutoScrollLogCheckBox;

    // UI Components - Performance
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PerformanceStatsText;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* AutoRefreshCheckBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* RefreshIntervalTextBox;

    // Data
    UPROPERTY(BlueprintReadOnly, Category = "Debug Data")
    TArray<FMusicDebugInfo> StemDebugInfo;

    UPROPERTY(BlueprintReadOnly, Category = "Debug Data")
    TArray<FMusicDebugLogEntry> DebugLog;

    // Component references
    UPROPERTY()
    UProceduralMusicSubsystemV2* ProceduralMusicSubsystem;

    UPROPERTY()
    URuntimeMusicLayeringComponent* RuntimeMusicLayeringComponent;

    UPROPERTY()
    UAIDMNarrativeMusicLinker* AIDMNarrativeMusicLinker;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel Settings")
    bool bAutoRefresh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel Settings")
    float RefreshInterval; // Seconds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel Settings")
    int32 MaxLogEntries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel Settings")
    bool bAutoScrollLog;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Panel State")
    bool bPanelActive;

    UPROPERTY(BlueprintReadOnly, Category = "Panel State")
    float LastRefreshTime;

    // Timer handle
    FTimerHandle RefreshTimer;

private:
    // Helper methods
    void PopulateComboBoxes();
    void UpdateStemList();
    void UpdateStatusDisplay();
    void UpdatePerformanceStats();
    void AddDebugLogEntry(const FString& EventType, const FString& Description, const FString& StemID = TEXT(""), float Value = 0.0f);
    void TrimDebugLog();

    // UI event handlers
    UFUNCTION()
    void OnBiomeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnToneComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnCombatModeCheckBoxChanged(bool bIsChecked);

    UFUNCTION()
    void OnMasterVolumeSliderChanged(float Value);

    UFUNCTION()
    void OnStopAllButtonClicked();

    UFUNCTION()
    void OnResetSystemButtonClicked();

    UFUNCTION()
    void OnRefreshStemsButtonClicked();

    UFUNCTION()
    void OnTriggerNarrativeButtonClicked();

    UFUNCTION()
    void OnPlayerMoralitySliderChanged(float Value);

    UFUNCTION()
    void OnStoryProgressionSliderChanged(float Value);

    UFUNCTION()
    void OnEmotionalIntensitySliderChanged(float Value);

    UFUNCTION()
    void OnApplyPresetButtonClicked();

    UFUNCTION()
    void OnClearLogButtonClicked();

    UFUNCTION()
    void OnAutoScrollLogCheckBoxChanged(bool bIsChecked);

    UFUNCTION()
    void OnAutoRefreshCheckBoxChanged(bool bIsChecked);

    UFUNCTION()
    void OnStemItemSelected(UObject* Item);

    // Music system events
    UFUNCTION()
    void OnMusicBiomeChanged(EMusicBiome OldBiome, EMusicBiome NewBiome);

    UFUNCTION()
    void OnMusicToneChanged(EMusicTone OldTone, EMusicTone NewTone);

    UFUNCTION()
    void OnMusicStemChanged(const FString& StemID, bool bIsActive);

    UFUNCTION()
    void OnMusicLayerStateChanged(const FString& LayerID, EMusicLayerState NewState);

    UFUNCTION()
    void OnNarrativeTagTriggered(EAIDMNarrativeTag NarrativeTag, const FNarrativeContextData& ContextData);

    // Timer callback
    UFUNCTION()
    void OnRefreshTimer();

public:
    /**
     * Blueprint implementable events for custom debug panel logic
     */

    /**
     * Called when debug panel is initialized (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debug Panel Events")
    void OnMusicDebugPanelInitializedEvent();

    /**
     * Called when stem state changes (for custom UI updates)
     * @param StemID ID of changed stem
     * @param bIsActive Whether stem is active
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debug Panel Events")
    void OnStemStateChangedEvent(const FString& StemID, bool bIsActive);

    /**
     * Called when music event is triggered (for custom logging)
     * @param EventType Type of event
     * @param Description Event description
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debug Panel Events")
    void OnMusicEventTriggeredEvent(const FString& EventType, const FString& Description);

    /**
     * Called to customize stem display (override in Blueprint)
     * @param StemInfo Stem debug info
     * @return Customized display text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debug Panel Events")
    FString CustomizeStemDisplay(const FMusicDebugInfo& StemInfo);

    /**
     * Called to validate debug command (override in Blueprint)
     * @param Command Command being executed
     * @param Parameters Command parameters
     * @return True if command should be executed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Debug Panel Events")
    bool ValidateDebugCommand(const FString& Command, const TMap<FString, FString>& Parameters);
};
