// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Cloud/CloudSaveSubsystem.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "CampaignBrowserWidget.generated.h"

/**
 * Campaign filter types
 */
UENUM(BlueprintType)
enum class ECampaignFilter : uint8
{
    All             UMETA(DisplayName = "All Campaigns"),
    MyCreated       UMETA(DisplayName = "My Created"),
    MyPlayed        UMETA(DisplayName = "My Played"),
    Featured        UMETA(DisplayName = "Featured"),
    Popular         UMETA(DisplayName = "Popular"),
    Recent          UMETA(DisplayName = "Recent"),
    Multiplayer     UMETA(DisplayName = "Multiplayer"),
    SinglePlayer    UMETA(DisplayName = "Single Player"),
    Completed       UMETA(DisplayName = "Completed"),
    InProgress      UMETA(DisplayName = "In Progress")
};

/**
 * Campaign sort types
 */
UENUM(BlueprintType)
enum class ECampaignSort : uint8
{
    Name            UMETA(DisplayName = "Name"),
    DateCreated     UMETA(DisplayName = "Date Created"),
    DateModified    UMETA(DisplayName = "Date Modified"),
    PlayTime        UMETA(DisplayName = "Play Time"),
    Rating          UMETA(DisplayName = "Rating"),
    Downloads       UMETA(DisplayName = "Downloads"),
    Difficulty      UMETA(DisplayName = "Difficulty")
};

/**
 * Campaign browser entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCampaignBrowserEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString CampaignID;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString CampaignName;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString AuthorName;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString AuthorID;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FDateTime CreatedDate;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FDateTime ModifiedDate;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    int32 EstimatedDuration; // Minutes

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString DifficultyLevel; // "easy", "medium", "hard"

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    int32 PlayerCount; // Number of players supported

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    float Rating; // 0.0 to 5.0

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    int32 Downloads;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    TArray<FString> Tags; // Campaign tags

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    FString ThumbnailURL; // URL to campaign thumbnail

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    bool bIsPublic;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    bool bIsDownloaded;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    bool bIsInProgress;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Entry")
    float ProgressPercentage; // 0.0 to 1.0

    FCampaignBrowserEntry()
    {
        CampaignID = TEXT("");
        CampaignName = TEXT("Untitled Campaign");
        Description = TEXT("");
        AuthorName = TEXT("Unknown");
        AuthorID = TEXT("");
        EstimatedDuration = 60;
        DifficultyLevel = TEXT("medium");
        PlayerCount = 1;
        Rating = 0.0f;
        Downloads = 0;
        ThumbnailURL = TEXT("");
        bIsPublic = false;
        bIsDownloaded = false;
        bIsInProgress = false;
        ProgressPercentage = 0.0f;
    }
};

/**
 * Campaign browser events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCampaignSelected, const FCampaignBrowserEntry&, Campaign);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCampaignDownloaded, const FCampaignBrowserEntry&, Campaign);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCampaignUploaded, const FCampaignBrowserEntry&, Campaign);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCampaignRated, const FString&, CampaignID, float, Rating);

/**
 * Campaign Browser Widget - UI for browsing and managing campaigns
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCampaignBrowserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UCampaignBrowserWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    /**
     * Initialize campaign browser
     * @param CloudSave Cloud save subsystem for campaign data
     * @param CampaignLoader Campaign loader for local campaigns
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void InitializeCampaignBrowser(UCloudSaveSubsystem* CloudSave, UCampaignLoaderSubsystem* CampaignLoader);

    /**
     * Refresh campaign list
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void RefreshCampaignList();

    /**
     * Apply filter to campaign list
     * @param Filter Filter type to apply
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void ApplyFilter(ECampaignFilter Filter);

    /**
     * Apply sort to campaign list
     * @param Sort Sort type to apply
     * @param bAscending Whether to sort in ascending order
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void ApplySort(ECampaignSort Sort, bool bAscending = true);

    /**
     * Search campaigns by text
     * @param SearchText Text to search for
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void SearchCampaigns(const FString& SearchText);

    /**
     * Download campaign
     * @param CampaignID ID of campaign to download
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void DownloadCampaign(const FString& CampaignID);

    /**
     * Upload campaign
     * @param CampaignPath Path to local campaign file
     * @param bMakePublic Whether to make campaign public
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void UploadCampaign(const FString& CampaignPath, bool bMakePublic = false);

    /**
     * Rate campaign
     * @param CampaignID ID of campaign to rate
     * @param Rating Rating value (0.0 to 5.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void RateCampaign(const FString& CampaignID, float Rating);

    /**
     * Delete campaign
     * @param CampaignID ID of campaign to delete
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void DeleteCampaign(const FString& CampaignID);

    /**
     * Get filtered campaigns
     * @return Array of campaigns matching current filter
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Browser")
    TArray<FCampaignBrowserEntry> GetFilteredCampaigns() const { return FilteredCampaigns; }

    /**
     * Get selected campaign
     * @return Currently selected campaign
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Browser")
    FCampaignBrowserEntry GetSelectedCampaign() const { return SelectedCampaign; }

    /**
     * Set selected campaign
     * @param Campaign Campaign to select
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Browser")
    void SetSelectedCampaign(const FCampaignBrowserEntry& Campaign);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Campaign Browser Events")
    FOnCampaignSelected OnCampaignSelected;

    UPROPERTY(BlueprintAssignable, Category = "Campaign Browser Events")
    FOnCampaignDownloaded OnCampaignDownloaded;

    UPROPERTY(BlueprintAssignable, Category = "Campaign Browser Events")
    FOnCampaignUploaded OnCampaignUploaded;

    UPROPERTY(BlueprintAssignable, Category = "Campaign Browser Events")
    FOnCampaignRated OnCampaignRated;

protected:
    // UI Components
    UPROPERTY(meta = (BindWidget))
    UScrollBox* CampaignListScrollBox;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StatusTextBlock;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* SearchTextBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* FilterComboBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* SortComboBox;

    UPROPERTY(meta = (BindWidget))
    UButton* RefreshButton;

    UPROPERTY(meta = (BindWidget))
    UButton* UploadButton;

    UPROPERTY(meta = (BindWidget))
    UButton* DownloadButton;

    UPROPERTY(meta = (BindWidget))
    UButton* DeleteButton;

    // Campaign data
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    TArray<FCampaignBrowserEntry> AllCampaigns;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    TArray<FCampaignBrowserEntry> FilteredCampaigns;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    FCampaignBrowserEntry SelectedCampaign;

    // Current filter/sort settings
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    ECampaignFilter CurrentFilter;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    ECampaignSort CurrentSort;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    bool bSortAscending;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Browser")
    FString CurrentSearchText;

    // Component references
    UPROPERTY()
    UCloudSaveSubsystem* CloudSaveRef;

    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    // UI state
    UPROPERTY()
    bool bIsLoading;

    UPROPERTY()
    TMap<FString, UWidget*> CampaignWidgets; // CampaignID -> Widget

private:
    // UI event handlers
    UFUNCTION()
    void OnRefreshButtonClicked();

    UFUNCTION()
    void OnUploadButtonClicked();

    UFUNCTION()
    void OnDownloadButtonClicked();

    UFUNCTION()
    void OnDeleteButtonClicked();

    UFUNCTION()
    void OnSearchTextChanged(const FText& Text);

    UFUNCTION()
    void OnFilterSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnSortSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    // Cloud save event handlers
    UFUNCTION()
    void OnCloudSaveCompleted(const FCloudSaveEntry& SaveEntry);

    UFUNCTION()
    void OnCloudLoadCompleted(const FCloudSaveEntry& SaveEntry);

    // Helper methods
    void PopulateFilterComboBox();
    void PopulateSortComboBox();
    void UpdateCampaignList();
    void CreateCampaignWidget(const FCampaignBrowserEntry& Campaign);
    void RemoveCampaignWidget(const FString& CampaignID);
    void UpdateStatusText(const FString& Status);
    bool PassesFilter(const FCampaignBrowserEntry& Campaign) const;
    void SortCampaigns();
    FCampaignBrowserEntry ConvertCloudSaveToEntry(const FCloudSaveEntry& SaveEntry) const;
    FCloudSaveEntry ConvertEntryToCloudSave(const FCampaignBrowserEntry& Entry) const;

public:
    /**
     * Blueprint implementable events for custom browser logic
     */

    /**
     * Called when campaign is selected (for custom UI updates)
     * @param Campaign The selected campaign
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Browser Events")
    void OnCampaignSelectedEvent(const FCampaignBrowserEntry& Campaign);

    /**
     * Called when campaign list is updated (for custom UI updates)
     * @param CampaignCount Number of campaigns in list
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Browser Events")
    void OnCampaignListUpdated(int32 CampaignCount);

    /**
     * Called to create custom campaign widget (override in Blueprint)
     * @param Campaign Campaign data for the widget
     * @return Custom campaign widget
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Browser Events")
    UWidget* CreateCustomCampaignWidget(const FCampaignBrowserEntry& Campaign);

    /**
     * Called to validate campaign upload (override in Blueprint)
     * @param CampaignPath Path to campaign file
     * @return True if campaign is valid for upload
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Browser Events")
    bool ValidateCampaignUpload(const FString& CampaignPath);

    /**
     * Called when campaign download starts (for custom progress UI)
     * @param Campaign Campaign being downloaded
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Browser Events")
    void OnCampaignDownloadStarted(const FCampaignBrowserEntry& Campaign);

    /**
     * Called when campaign upload starts (for custom progress UI)
     * @param Campaign Campaign being uploaded
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Browser Events")
    void OnCampaignUploadStarted(const FCampaignBrowserEntry& Campaign);
};
