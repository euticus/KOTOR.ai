// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "Camera/CameraActor.h"
#include "Photography/CinematicScreenshotNarrator.h"
#include "ScreenshotCaptureComponent.generated.h"

/**
 * Capture quality settings
 */
UENUM(BlueprintType)
enum class ECaptureQuality : uint8
{
    Low                 UMETA(DisplayName = "Low (720p)"),
    Medium              UMETA(DisplayName = "Medium (1080p)"),
    High                UMETA(DisplayName = "High (1440p)"),
    Ultra               UMETA(DisplayName = "Ultra (4K)"),
    Custom              UMETA(DisplayName = "Custom")
};

/**
 * Capture format types
 */
UENUM(BlueprintType)
enum class ECaptureFormat : uint8
{
    PNG                 UMETA(DisplayName = "PNG"),
    JPEG                UMETA(DisplayName = "JPEG"),
    BMP                 UMETA(DisplayName = "BMP"),
    EXR                 UMETA(DisplayName = "EXR")
};

/**
 * Screenshot capture settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FScreenshotCaptureSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    ECaptureQuality Quality;

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    ECaptureFormat Format;

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    FIntPoint CustomResolution; // Used when Quality is Custom

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    int32 JPEGQuality; // 1-100, used for JPEG format

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    bool bIncludeUI; // Whether to include UI in screenshot

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    bool bIncludeCaption; // Whether to overlay caption

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    bool bAutoGenerateCaption; // Whether to auto-generate caption

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    ECaptionStyle CaptionStyle; // Style for auto-generated caption

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    FString SaveDirectory; // Directory to save screenshots

    UPROPERTY(BlueprintReadWrite, Category = "Capture Settings")
    FString FileNamePrefix; // Prefix for screenshot files

    FScreenshotCaptureSettings()
    {
        Quality = ECaptureQuality::High;
        Format = ECaptureFormat::PNG;
        CustomResolution = FIntPoint(1920, 1080);
        JPEGQuality = 90;
        bIncludeUI = false;
        bIncludeCaption = true;
        bAutoGenerateCaption = true;
        CaptionStyle = ECaptionStyle::Poetic;
        SaveDirectory = TEXT("Screenshots");
        FileNamePrefix = TEXT("KOTOR_");
    }
};

/**
 * Screenshot metadata
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FScreenshotMetadata
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FString FileName;

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FString FilePath;

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FDateTime CaptureTime;

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FIntPoint Resolution;

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    ECaptureFormat Format;

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FString CameraLocation; // Location where screenshot was taken

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FString CurrentLevel; // Level where screenshot was taken

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    FString GameplayContext; // What was happening when screenshot was taken

    UPROPERTY(BlueprintReadWrite, Category = "Screenshot Metadata")
    TMap<FString, FString> CustomMetadata; // Additional metadata

    FScreenshotMetadata()
    {
        FileName = TEXT("");
        FilePath = TEXT("");
        Resolution = FIntPoint::ZeroValue;
        Format = ECaptureFormat::PNG;
        CameraLocation = TEXT("");
        CurrentLevel = TEXT("");
        GameplayContext = TEXT("");
    }
};

/**
 * Screenshot events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotCaptureStarted, const FScreenshotCaptureSettings&, Settings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenshotCaptureCompleted, const FString&, FilePath, const FScreenshotMetadata&, Metadata);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenshotCaptureFailed, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCaptionGenerated, const FString&, Caption, ECaptionStyle, Style);

/**
 * Screenshot Capture Component - Handles screenshot capture with AI caption generation
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UScreenshotCaptureComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UScreenshotCaptureComponent();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize screenshot capture component
     * @param ScreenshotNarrator Screenshot narrator for caption generation
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    void InitializeScreenshotCapture(UCinematicScreenshotNarrator* ScreenshotNarrator);

    /**
     * Capture screenshot with current settings
     * @param CustomCaption Custom caption to use (optional)
     * @return True if capture was initiated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    bool CaptureScreenshot(const FString& CustomCaption = TEXT(""));

    /**
     * Capture screenshot with custom settings
     * @param Settings Custom capture settings
     * @param CustomCaption Custom caption to use (optional)
     * @return True if capture was initiated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    bool CaptureScreenshotWithSettings(const FScreenshotCaptureSettings& Settings, const FString& CustomCaption = TEXT(""));

    /**
     * Capture screenshot from specific camera
     * @param Camera Camera to capture from
     * @param Settings Capture settings
     * @param CustomCaption Custom caption to use (optional)
     * @return True if capture was initiated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    bool CaptureScreenshotFromCamera(ACameraActor* Camera, const FScreenshotCaptureSettings& Settings, const FString& CustomCaption = TEXT(""));

    /**
     * Set capture settings
     * @param NewSettings New capture settings
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    void SetCaptureSettings(const FScreenshotCaptureSettings& NewSettings);

    /**
     * Get capture settings
     * @return Current capture settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Capture")
    FScreenshotCaptureSettings GetCaptureSettings() const { return CaptureSettings; }

    /**
     * Generate caption for current context
     * @param CaptionStyle Style for the caption
     * @return Generated caption
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    FString GenerateCaptionForCurrentContext(ECaptionStyle CaptionStyle);

    /**
     * Save texture to file
     * @param Texture Texture to save
     * @param FilePath Path to save file
     * @param Format Format to save in
     * @param Quality Quality for JPEG (1-100)
     * @return True if save was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    bool SaveTextureToFile(UTexture2D* Texture, const FString& FilePath, ECaptureFormat Format, int32 Quality = 90);

    /**
     * Get resolution for quality setting
     * @param Quality Quality setting
     * @return Resolution for the quality
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Capture")
    FIntPoint GetResolutionForQuality(ECaptureQuality Quality) const;

    /**
     * Get file extension for format
     * @param Format Capture format
     * @return File extension (without dot)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Capture")
    FString GetFileExtensionForFormat(ECaptureFormat Format) const;

    /**
     * Generate unique filename
     * @param Prefix Filename prefix
     * @param Extension File extension
     * @return Unique filename
     */
    UFUNCTION(BlueprintCallable, Category = "Screenshot Capture")
    FString GenerateUniqueFilename(const FString& Prefix, const FString& Extension);

    /**
     * Check if capture is in progress
     * @return True if capture is currently in progress
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Capture")
    bool IsCaptureInProgress() const { return bCaptureInProgress; }

    /**
     * Get last captured screenshot metadata
     * @return Metadata of last captured screenshot
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Screenshot Capture")
    FScreenshotMetadata GetLastCaptureMetadata() const { return LastCaptureMetadata; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnScreenshotCaptureStarted OnScreenshotCaptureStarted;

    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnScreenshotCaptureCompleted OnScreenshotCaptureCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnScreenshotCaptureFailed OnScreenshotCaptureFailed;

    UPROPERTY(BlueprintAssignable, Category = "Screenshot Events")
    FOnCaptionGenerated OnCaptionGenerated;

protected:
    // Capture settings
    UPROPERTY(BlueprintReadOnly, Category = "Capture Settings")
    FScreenshotCaptureSettings CaptureSettings;

    // Capture state
    UPROPERTY(BlueprintReadOnly, Category = "Capture State")
    bool bCaptureInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Capture State")
    FScreenshotMetadata LastCaptureMetadata;

    // Component references
    UPROPERTY()
    UCinematicScreenshotNarrator* ScreenshotNarratorRef;

    // Render targets
    UPROPERTY()
    UTextureRenderTarget2D* CaptureRenderTarget;

    // Quality resolution mappings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality Settings")
    TMap<ECaptureQuality, FIntPoint> QualityResolutions;

private:
    // Helper methods
    void SetupQualityResolutions();
    UTexture2D* CaptureToTexture(const FScreenshotCaptureSettings& Settings, ACameraActor* Camera = nullptr);
    void SetupRenderTarget(const FIntPoint& Resolution);
    FString BuildFilePath(const FScreenshotCaptureSettings& Settings, const FString& Filename);
    FScreenshotMetadata BuildMetadata(const FString& FilePath, const FScreenshotCaptureSettings& Settings);
    FString GatherGameplayContext();
    void OverlayCaptionOnTexture(UTexture2D* Texture, const FString& Caption);

    // Async capture handling
    void OnCaptureCompleted(const FString& FilePath, const FScreenshotMetadata& Metadata);
    void OnCaptureFailed(const FString& ErrorMessage);

public:
    /**
     * Blueprint implementable events for custom capture logic
     */

    /**
     * Called when screenshot capture starts (for custom setup)
     * @param Settings The capture settings being used
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    void OnScreenshotCaptureStartedEvent(const FScreenshotCaptureSettings& Settings);

    /**
     * Called when screenshot capture completes (for custom processing)
     * @param FilePath Path to the captured screenshot
     * @param Metadata Screenshot metadata
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    void OnScreenshotCaptureCompletedEvent(const FString& FilePath, const FScreenshotMetadata& Metadata);

    /**
     * Called to generate custom caption (override in Blueprint)
     * @param Context Current gameplay context
     * @param CaptionStyle Style for the caption
     * @return Custom generated caption
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    FString GenerateCustomCaption(const FString& Context, ECaptionStyle CaptionStyle);

    /**
     * Called to customize screenshot post-processing (override in Blueprint)
     * @param Texture The captured texture
     * @param Settings Capture settings used
     * @return Post-processed texture
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    UTexture2D* PostProcessScreenshot(UTexture2D* Texture, const FScreenshotCaptureSettings& Settings);

    /**
     * Called to gather custom metadata (override in Blueprint)
     * @param Settings Capture settings
     * @return Custom metadata map
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Screenshot Events")
    TMap<FString, FString> GatherCustomMetadata(const FScreenshotCaptureSettings& Settings);
};
