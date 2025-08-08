// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "CinematicPlayerWidget.generated.h"

/**
 * Cinematic entry data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCinematicEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    FString CinematicID;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    FString CinematicName;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    TSoftObjectPtr<ULevelSequence> LevelSequence;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    UTexture2D* ThumbnailImage;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    float Duration; // Duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    FString Category; // Story, Character, Combat, etc.

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    bool bIsUnlocked; // Whether player has unlocked this cinematic

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    FDateTime UnlockDate; // When cinematic was unlocked

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    int32 ViewCount; // How many times player has watched

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Entry")
    TArray<FString> Tags; // Tags for filtering

    FCinematicEntry()
    {
        CinematicID = TEXT("");
        CinematicName = TEXT("Unknown Cinematic");
        Description = TEXT("");
        ThumbnailImage = nullptr;
        Duration = 0.0f;
        Category = TEXT("Story");
        bIsUnlocked = false;
        ViewCount = 0;
    }
};

/**
 * Cinematic category data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCinematicCategory
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Category")
    FString CategoryID;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Category")
    FString CategoryName;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Category")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Category")
    UTexture2D* CategoryIcon;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Category")
    TArray<FCinematicEntry> Cinematics;

    FCinematicCategory()
    {
        CategoryID = TEXT("");
        CategoryName = TEXT("Unknown Category");
        Description = TEXT("");
        CategoryIcon = nullptr;
    }
};

/**
 * Cinematic player events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCinematicSelected, const FCinematicEntry&, Cinematic);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCinematicPlaybackStarted, const FString&, CinematicID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCinematicPlaybackCompleted, const FString&, CinematicID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCinematicPlaybackPaused, bool, bIsPaused);

/**
 * Cinematic Player Widget - In-game rewatch menu for campaign cinematics
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCinematicPlayerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UCinematicPlayerWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize cinematic player
     * @param CinematicCategories Categories of cinematics to display
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void InitializeCinematicPlayer(const TArray<FCinematicCategory>& CinematicCategories);

    /**
     * Add cinematic entry
     * @param Cinematic Cinematic to add
     * @param CategoryID Category to add to
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void AddCinematicEntry(const FCinematicEntry& Cinematic, const FString& CategoryID);

    /**
     * Remove cinematic entry
     * @param CinematicID ID of cinematic to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void RemoveCinematicEntry(const FString& CinematicID);

    /**
     * Unlock cinematic
     * @param CinematicID ID of cinematic to unlock
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void UnlockCinematic(const FString& CinematicID);

    /**
     * Play selected cinematic
     * @param CinematicID ID of cinematic to play
     * @return True if playback started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    bool PlayCinematic(const FString& CinematicID);

    /**
     * Stop current cinematic
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void StopCinematic();

    /**
     * Pause/unpause current cinematic
     * @param bPause Whether to pause or unpause
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void PauseCinematic(bool bPause);

    /**
     * Seek to time in current cinematic
     * @param Time Time to seek to (seconds)
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void SeekToTime(float Time);

    /**
     * Set playback speed
     * @param Speed Playback speed (0.5 to 2.0)
     */
    void SetPlaybackSpeed(float Speed);

    /**
     * Filter cinematics by category
     * @param CategoryID Category to filter by (empty = show all)
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void FilterByCategory(const FString& CategoryID);

    /**
     * Search cinematics by text
     * @param SearchText Text to search for
     */
    UFUNCTION(BlueprintCallable, Category = "Cinematic Player")
    void SearchCinematics(const FString& SearchText);

    /**
     * Get current cinematic
     * @return Currently playing cinematic
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cinematic Player")
    FCinematicEntry GetCurrentCinematic() const { return CurrentCinematic; }

    /**
     * Check if cinematic is playing
     * @return True if a cinematic is currently playing
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cinematic Player")
    bool IsCinematicPlaying() const { return bIsPlaying; }

    /**
     * Get playback progress
     * @return Progress from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cinematic Player")
    float GetPlaybackProgress() const;

    /**
     * Get current playback time
     * @return Current time in seconds
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cinematic Player")
    float GetCurrentTime() const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Cinematic Player Events")
    FOnCinematicSelected OnCinematicSelected;

    UPROPERTY(BlueprintAssignable, Category = "Cinematic Player Events")
    FOnCinematicPlaybackStarted OnCinematicPlaybackStarted;

    UPROPERTY(BlueprintAssignable, Category = "Cinematic Player Events")
    FOnCinematicPlaybackCompleted OnCinematicPlaybackCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Cinematic Player Events")
    FOnCinematicPlaybackPaused OnCinematicPlaybackPaused;

protected:
    // UI Components
    UPROPERTY(meta = (BindWidget))
    UListView* CinematicListView;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* CategoryComboBox;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* SearchTextBox;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CinematicTitleText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CinematicDescriptionText;

    UPROPERTY(meta = (BindWidget))
    UImage* CinematicThumbnailImage;

    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UButton* PauseButton;

    UPROPERTY(meta = (BindWidget))
    UButton* StopButton;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* PlaybackProgressBar;

    UPROPERTY(meta = (BindWidget))
    USlider* SeekSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* SpeedSlider;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentTimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DurationText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ViewCountText;

    // Cinematic data
    UPROPERTY(BlueprintReadOnly, Category = "Cinematic Data")
    TArray<FCinematicCategory> CinematicCategories;

    UPROPERTY(BlueprintReadOnly, Category = "Cinematic Data")
    TArray<FCinematicEntry> FilteredCinematics;

    UPROPERTY(BlueprintReadOnly, Category = "Cinematic Data")
    FCinematicEntry CurrentCinematic;

    // Playback state
    UPROPERTY(BlueprintReadOnly, Category = "Playback State")
    bool bIsPlaying;

    UPROPERTY(BlueprintReadOnly, Category = "Playback State")
    bool bIsPaused;

    UPROPERTY(BlueprintReadOnly, Category = "Playback State")
    float CurrentPlaybackTime;

    UPROPERTY(BlueprintReadOnly, Category = "Playback State")
    float PlaybackSpeed;

    // Level sequence player
    UPROPERTY(BlueprintReadOnly, Category = "Sequence Player")
    ULevelSequencePlayer* SequencePlayer;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    bool bAutoPlayNext; // Auto-play next cinematic in category

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    bool bShowLockedCinematics; // Show locked cinematics in list

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    float DefaultPlaybackSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    bool bRememberLastPosition; // Remember playback position

private:
    // Helper methods
    void PopulateCinematicList();
    void PopulateCategoryComboBox();
    void UpdateCinematicDetails(const FCinematicEntry& Cinematic);
    void UpdatePlaybackUI();
    void ApplyFilters();
    FCinematicEntry* FindCinematic(const FString& CinematicID);
    void IncrementViewCount(const FString& CinematicID);

    // UI event handlers
    UFUNCTION()
    void OnPlayButtonClicked();

    UFUNCTION()
    void OnPauseButtonClicked();

    UFUNCTION()
    void OnStopButtonClicked();

    UFUNCTION()
    void OnSeekSliderValueChanged(float Value);

    UFUNCTION()
    void OnSpeedSliderValueChanged(float Value);

    UFUNCTION()
    void OnCategorySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnSearchTextChanged(const FText& Text);

    UFUNCTION()
    void OnCinematicListSelectionChanged(UObject* Item);

    // Sequence events
    UFUNCTION()
    void OnSequenceFinished();

public:
    /**
     * Blueprint implementable events for custom cinematic player logic
     */

    /**
     * Called when cinematic is selected (for custom UI updates)
     * @param Cinematic The selected cinematic
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic Player Events")
    void OnCinematicSelectedEvent(const FCinematicEntry& Cinematic);

    /**
     * Called when cinematic playback starts (for custom effects)
     * @param CinematicID ID of the cinematic
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic Player Events")
    void OnCinematicPlaybackStartedEvent(const FString& CinematicID);

    /**
     * Called when cinematic playback completes (for custom processing)
     * @param CinematicID ID of the completed cinematic
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic Player Events")
    void OnCinematicPlaybackCompletedEvent(const FString& CinematicID);

    /**
     * Called to customize cinematic list display (override in Blueprint)
     * @param Cinematic Cinematic to customize
     * @return Customized display data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic Player Events")
    FCinematicEntry CustomizeCinematicDisplay(const FCinematicEntry& Cinematic);

    /**
     * Called to validate cinematic access (override in Blueprint)
     * @param CinematicID ID of cinematic to check
     * @return True if player can access this cinematic
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic Player Events")
    bool ValidateCinematicAccess(const FString& CinematicID);

    /**
     * Called to get next cinematic for auto-play (override in Blueprint)
     * @param CurrentCinematicID ID of current cinematic
     * @return ID of next cinematic to play
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic Player Events")
    FString GetNextCinematicForAutoPlay(const FString& CurrentCinematicID);
};
