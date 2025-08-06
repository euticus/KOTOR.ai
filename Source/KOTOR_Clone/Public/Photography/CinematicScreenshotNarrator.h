// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Camera/CameraActor.h"
#include "AIDM/QuestManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Timeline/CampaignTimelineComponent.h"
#include "CinematicScreenshotNarrator.generated.h"

/**
 * Photo mode states
 */
UENUM(BlueprintType)
enum class EPhotoModeState : uint8
{
    Inactive            UMETA(DisplayName = "Inactive"),
    Entering            UMETA(DisplayName = "Entering Photo Mode"),
    Active              UMETA(DisplayName = "Active"),
    Capturing           UMETA(DisplayName = "Capturing"),
    Processing          UMETA(DisplayName = "Processing"),
    Exiting             UMETA(DisplayName = "Exiting Photo Mode")
};

/**
 * Caption styles
 */
UENUM(BlueprintType)
enum class ECaptionStyle : uint8
{
    Poetic              UMETA(DisplayName = "Poetic"),
    Dramatic            UMETA(DisplayName = "Dramatic"),
    Philosophical       UMETA(DisplayName = "Philosophical"),
    Heroic              UMETA(DisplayName = "Heroic"),
    Melancholic         UMETA(DisplayName = "Melancholic"),
    Epic                UMETA(DisplayName = "Epic"),
    Intimate            UMETA(DisplayName = "Intimate"),
    Mysterious          UMETA(DisplayName = "Mysterious"),
    Triumphant          UMETA(DisplayName = "Triumphant"),
    Tragic              UMETA(DisplayName = "Tragic")
};

/**
 * Screenshot data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCinematicScreenshot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString ScreenshotID;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString FileName;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString FilePath;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString Caption; // LLM-generated caption

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    ECaptionStyle CaptionStyle;

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString Context; // Context when screenshot was taken

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    TArray<FString> RecentEvents; // Events from last 30 seconds

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString CurrentQuest; // Active quest when captured

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    FString Location; // Where screenshot was taken

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    float Timestamp; // When screenshot was taken

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    TMap<FString, FString> Metadata; // Additional metadata

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    bool bIsFavorited; // Player favorited this screenshot

    UPROPERTY(BlueprintReadWrite, Category = "Cinematic Screenshot")
    TArray<FString> Tags; // User-added tags

    FCinematicScreenshot()
    {
        ScreenshotID = TEXT("");
        FileName = TEXT("");
        FilePath = TEXT("");
        Caption = TEXT("");
        CaptionStyle = ECaptionStyle::Poetic;
        Context = TEXT("");
        CurrentQuest = TEXT("");
        Location = TEXT("");
        Timestamp = 0.0f;
        bIsFavorited = false;
    }
};

/**
 * Photo mode settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPhotoModeSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    bool bFreeCameraEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    bool bHideUI;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    bool bPauseGame;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    bool bHidePlayer;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    float CameraSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    float FOV;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    FVector CameraOffset;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    ECaptionStyle DefaultCaptionStyle;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    bool bAutoGenerateCaption;

    UPROPERTY(BlueprintReadWrite, Category = "Photo Mode Settings")
    FString SaveDirectory;

    FPhotoModeSettings()
    {
        bFreeCameraEnabled = true;
        bHideUI = true;
        bPauseGame = true;
        bHidePlayer = false;
        CameraSpeed = 500.0f;
        FOV = 90.0f;
        CameraOffset = FVector::ZeroVector;
        DefaultCaptionStyle = ECaptionStyle::Poetic;
        bAutoGenerateCaption = true;
        SaveDirectory = TEXT("CampaignPhotos");
    }
};

/**
 * Screenshot events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoModeToggled, bool, bEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotCaptured, const FCinematicScreenshot&, Screenshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCaptionGenerated, const FString&, Caption);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotSaved, const FString&, FilePath);

/**
 * Cinematic Screenshot Narrator - Captures emotional moments with AI-generated captions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCinematicScreenshotNarrator : public UActorComponent
{
    GENERATED_BODY()

public:
    UCinematicScreenshotNarrator();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize screenshot narrator
     * @param QuestManager Quest manager for quest context
     * @param NarrativeMemory Narrative memory for recent events
     * @param Timeline Campaign timeline for context
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    void InitializeScreenshotNarrator(UQuestManagerComponent* QuestManager,
                                     UNarrativeMemoryComponent* NarrativeMemory,
                                     UCampaignTimelineComponent* Timeline);

    /**
     * Toggle photo mode
     * @param bEnable Whether to enable photo mode
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    void TogglePhotoMode(bool bEnable);

    /**
     * Capture cinematic screenshot
     * @param CaptionStyle Style for the caption
     * @param CustomCaption Custom caption (optional)
     * @return Captured screenshot data
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    FCinematicScreenshot CaptureScreenshot(ECaptionStyle CaptionStyle = ECaptionStyle::Poetic, 
                                          const FString& CustomCaption = TEXT(""));

    /**
     * Generate caption for screenshot
     * @param Context Context when screenshot was taken
     * @param CaptionStyle Style for the caption
     * @return Generated caption
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    FString GenerateScreenshotCaption(const FString& Context, ECaptionStyle CaptionStyle);

    /**
     * Save screenshot to file
     * @param Screenshot Screenshot data to save
     * @param bIncludeCaption Whether to overlay caption on image
     * @return True if save was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    bool SaveScreenshotToFile(const FCinematicScreenshot& Screenshot, bool bIncludeCaption = true);

    /**
     * Get current photo mode state
     * @return Current photo mode state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Narrator")
    EPhotoModeState GetPhotoModeState() const { return CurrentPhotoModeState; }

    /**
     * Get all captured screenshots
     * @return Array of all captured screenshots
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Narrator")
    TArray<FCinematicScreenshot> GetAllScreenshots() const { return CapturedScreenshots; }

    /**
     * Get favorited screenshots
     * @return Array of favorited screenshots
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Narrator")
    TArray<FCinematicScreenshot> GetFavoritedScreenshots() const;

    /**
     * Toggle screenshot favorite status
     * @param ScreenshotID ID of screenshot to toggle
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    void ToggleScreenshotFavorite(const FString& ScreenshotID);

    /**
     * Add tag to screenshot
     * @param ScreenshotID ID of screenshot
     * @param Tag Tag to add
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    void AddScreenshotTag(const FString& ScreenshotID, const FString& Tag);

    /**
     * Search screenshots by tag
     * @param Tag Tag to search for
     * @return Array of screenshots with the tag
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    TArray<FCinematicScreenshot> SearchScreenshotsByTag(const FString& Tag) const;

    /**
     * Set photo mode settings
     * @param Settings New photo mode settings
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    void SetPhotoModeSettings(const FPhotoModeSettings& Settings);

    /**
     * Get photo mode settings
     * @return Current photo mode settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Narrator")
    FPhotoModeSettings GetPhotoModeSettings() const { return PhotoModeSettings; }

    /**
     * Export screenshot gallery
     * @param ExportPath Path to export gallery
     * @param bIncludeFavoritesOnly Whether to export only favorited screenshots
     * @return True if export was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Narrator")
    bool ExportScreenshotGallery(const FString& ExportPath, bool bIncludeFavoritesOnly = false);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnPhotoModeToggled OnPhotoModeToggled;

    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnScreenshotCaptured OnScreenshotCaptured;

    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnCaptionGenerated OnCaptionGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnScreenshotSaved OnScreenshotSaved;

protected:
    // Screenshot data
    UPROPERTY(BlueprintReadOnly, Category = "Screenshot Narrator")
    TArray<FCinematicScreenshot> CapturedScreenshots;

    UPROPERTY(BlueprintReadOnly, Category = "Screenshot Narrator")
    EPhotoModeState CurrentPhotoModeState;

    // Photo mode settings
    UPROPERTY(BlueprintReadOnly, Category = "Screenshot Narrator")
    FPhotoModeSettings PhotoModeSettings;

    // Component references
    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UCampaignTimelineComponent* TimelineRef;

    // Camera management
    UPROPERTY()
    ACameraActor* PhotoModeCamera;

    UPROPERTY()
    AActor* OriginalViewTarget;

    UPROPERTY()
    bool bGameWasPaused;

    // Render target for screenshots
    UPROPERTY()
    UTextureRenderTarget2D* ScreenshotRenderTarget;

    // Caption templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Caption Templates")
    TMap<ECaptionStyle, FString> CaptionStyleTemplates;

    // Input handling
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
    FKey PhotoModeToggleKey; // Default: F11

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
    FKey CaptureKey; // Default: Space

private:
    // Helper methods
    FString GenerateScreenshotID();
    FString BuildCaptionPrompt(const FString& Context, ECaptionStyle Style);
    FString GatherCurrentContext();
    TArray<FString> GetRecentEvents(float TimeWindow = 30.0f);
    void SetupPhotoModeCamera();
    void RestoreOriginalCamera();
    void SetupRenderTarget();
    UTexture2D* CaptureScreenshotTexture();
    void OverlayCaptionOnTexture(UTexture2D* Texture, const FString& Caption);
    FCinematicScreenshot* FindScreenshot(const FString& ScreenshotID);

    // Input handling
    void SetupInputBindings();
    void OnPhotoModeTogglePressed();
    void OnCapturePressed();

public:
    /**
     * Blueprint implementable events for custom screenshot logic
     */

    /**
     * Called when photo mode is toggled (for custom setup)
     * @param bEnabled Whether photo mode was enabled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    void OnPhotoModeToggledEvent(bool bEnabled);

    /**
     * Called when screenshot is captured (for custom processing)
     * @param Screenshot The captured screenshot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    void OnScreenshotCapturedEvent(const FCinematicScreenshot& Screenshot);

    /**
     * Called to generate custom caption (override in Blueprint)
     * @param Context Current context
     * @param CaptionStyle Style for the caption
     * @return Custom generated caption
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    FString GenerateCustomCaption(const FString& Context, ECaptionStyle CaptionStyle);

    /**
     * Called to apply custom photo mode effects (override in Blueprint)
     * @param bEnabled Whether photo mode is being enabled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    void ApplyCustomPhotoModeEffects(bool bEnabled);

    /**
     * Called to customize screenshot overlay (override in Blueprint)
     * @param Screenshot Screenshot being processed
     * @return Custom overlay texture
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    UTexture2D* CreateCustomScreenshotOverlay(const FCinematicScreenshot& Screenshot);
};
