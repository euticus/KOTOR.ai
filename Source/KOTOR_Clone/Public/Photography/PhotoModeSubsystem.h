// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Camera/CameraActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Photography/CinematicScreenshotNarrator.h"
#include "Photography/ScreenshotCaptureComponent.h"
#include "PhotoModeSubsystem.generated.h"

/**
 * Photo mode states
 */
UENUM(BlueprintType)
enum class EPhotoModeState : uint8
{
    Inactive            UMETA(DisplayName = "Inactive"),
    Activating          UMETA(DisplayName = "Activating"),
    Active              UMETA(DisplayName = "Active"),
    Capturing           UMETA(DisplayName = "Capturing"),
    Deactivating        UMETA(DisplayName = "Deactivating")
};

/**
 * Photo mode camera settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPhotoModeCameraSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float MovementSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float RotationSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float ZoomSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float MinFOV;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float MaxFOV;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float DefaultFOV;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    bool bInvertYAxis;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    bool bSmoothMovement;

    UPROPERTY(BlueprintReadWrite, Category = "Camera Settings")
    float SmoothingFactor;

    FPhotoModeCameraSettings()
    {
        MovementSpeed = 500.0f;
        RotationSpeed = 90.0f;
        ZoomSpeed = 10.0f;
        MinFOV = 10.0f;
        MaxFOV = 120.0f;
        DefaultFOV = 90.0f;
        bInvertYAxis = false;
        bSmoothMovement = true;
        SmoothingFactor = 5.0f;
    }
};

/**
 * Photo mode visual settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPhotoModeVisualSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    bool bHideUI;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    bool bHidePlayer;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    bool bHideHUD;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    bool bPauseGame;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    bool bEnableDepthOfField;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    float DepthOfFieldDistance;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    float DepthOfFieldRadius;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    bool bEnableVignette;

    UPROPERTY(BlueprintReadWrite, Category = "Visual Settings")
    float VignetteIntensity;

    FPhotoModeVisualSettings()
    {
        bHideUI = true;
        bHidePlayer = false;
        bHideHUD = true;
        bPauseGame = true;
        bEnableDepthOfField = false;
        DepthOfFieldDistance = 1000.0f;
        DepthOfFieldRadius = 100.0f;
        bEnableVignette = false;
        VignetteIntensity = 0.5f;
    }
};

/**
 * Photo mode events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoModeActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoModeDeactivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhotoCapture, const FString&, FilePath, const FString&, Caption);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoCaptureCompleted, const FString&, FilePath);

/**
 * Photo Mode Subsystem - Manages cinematic photo mode with free camera
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UPhotoModeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Toggle photo mode
     * @return True if photo mode was toggled successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    bool TogglePhotoMode();

    /**
     * Activate photo mode
     * @return True if photo mode was activated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    bool ActivatePhotoMode();

    /**
     * Deactivate photo mode
     * @return True if photo mode was deactivated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    bool DeactivatePhotoMode();

    /**
     * Check if photo mode is active
     * @return True if photo mode is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Photo Mode")
    bool IsPhotoModeActive() const { return PhotoModeState == EPhotoModeState::Active; }

    /**
     * Get photo mode state
     * @return Current photo mode state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Photo Mode")
    EPhotoModeState GetPhotoModeState() const { return PhotoModeState; }

    /**
     * Capture photo
     * @param bAutoGenerateCaption Whether to auto-generate caption
     * @param CustomCaption Custom caption to use
     * @return True if photo capture was initiated
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    bool CapturePhoto(bool bAutoGenerateCaption = true, const FString& CustomCaption = TEXT(""));

    /**
     * Set camera settings
     * @param Settings New camera settings
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    void SetCameraSettings(const FPhotoModeCameraSettings& Settings);

    /**
     * Get camera settings
     * @return Current camera settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Photo Mode")
    FPhotoModeCameraSettings GetCameraSettings() const { return CameraSettings; }

    /**
     * Set visual settings
     * @param Settings New visual settings
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    void SetVisualSettings(const FPhotoModeVisualSettings& Settings);

    /**
     * Get visual settings
     * @return Current visual settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Photo Mode")
    FPhotoModeVisualSettings GetVisualSettings() const { return VisualSettings; }

    /**
     * Move photo camera
     * @param Direction Movement direction
     * @param DeltaTime Time delta
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    void MovePhotoCamera(const FVector& Direction, float DeltaTime);

    /**
     * Rotate photo camera
     * @param Rotation Rotation input
     * @param DeltaTime Time delta
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    void RotatePhotoCamera(const FVector2D& Rotation, float DeltaTime);

    /**
     * Zoom photo camera
     * @param ZoomDelta Zoom input
     * @param DeltaTime Time delta
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    void ZoomPhotoCamera(float ZoomDelta, float DeltaTime);

    /**
     * Reset photo camera
     */
    UFUNCTION(BlueprintCallable, Category = "Photo Mode")
    void ResetPhotoCamera();

    /**
     * Get photo camera
     * @return Current photo camera actor
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Photo Mode")
    ACameraActor* GetPhotoCamera() const { return PhotoCamera; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Photo Mode Events")
    FOnPhotoModeActivated OnPhotoModeActivated;

    UPROPERTY(BlueprintAssignable, Category = "Photo Mode Events")
    FOnPhotoModeDeactivated OnPhotoModeDeactivated;

    UPROPERTY(BlueprintAssignable, Category = "Photo Mode Events")
    FOnPhotoCapture OnPhotoCapture;

    UPROPERTY(BlueprintAssignable, Category = "Photo Mode Events")
    FOnPhotoCaptureCompleted OnPhotoCaptureCompleted;

protected:
    // Photo mode state
    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode State")
    EPhotoModeState PhotoModeState;

    // Settings
    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode Settings")
    FPhotoModeCameraSettings CameraSettings;

    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode Settings")
    FPhotoModeVisualSettings VisualSettings;

    // Camera
    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode Camera")
    ACameraActor* PhotoCamera;

    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode Camera")
    FVector OriginalCameraLocation;

    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode Camera")
    FRotator OriginalCameraRotation;

    UPROPERTY(BlueprintReadOnly, Category = "Photo Mode Camera")
    float OriginalCameraFOV;

    // Player state backup
    UPROPERTY()
    APlayerController* OriginalPlayerController;

    UPROPERTY()
    APawn* OriginalPlayerPawn;

    UPROPERTY()
    AActor* OriginalViewTarget;

    // Components
    UPROPERTY()
    UCinematicScreenshotNarrator* ScreenshotNarrator;

    UPROPERTY()
    UScreenshotCaptureComponent* CaptureComponent;

    // Game state backup
    bool bOriginalGamePaused;
    bool bOriginalUIVisible;
    bool bOriginalHUDVisible;

private:
    // Helper methods
    void CreatePhotoCamera();
    void DestroyPhotoCamera();
    void BackupGameState();
    void RestoreGameState();
    void ApplyVisualSettings();
    void RestoreVisualSettings();
    void SetupCameraInput();
    void CleanupCameraInput();

    // Caption generation
    FString GeneratePhotoCaption();

public:
    /**
     * Blueprint implementable events for custom photo mode logic
     */

    /**
     * Called when photo mode is activated (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Photo Mode Events")
    void OnPhotoModeActivatedEvent();

    /**
     * Called when photo mode is deactivated (for custom cleanup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Photo Mode Events")
    void OnPhotoModeDeactivatedEvent();

    /**
     * Called when photo is captured (for custom processing)
     * @param FilePath Path to captured photo
     * @param Caption Generated or custom caption
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Photo Mode Events")
    void OnPhotoCaptureEvent(const FString& FilePath, const FString& Caption);

    /**
     * Called to customize photo mode camera (override in Blueprint)
     * @param Camera The photo mode camera
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Photo Mode Events")
    void CustomizePhotoModeCamera(ACameraActor* Camera);

    /**
     * Called to generate custom photo caption (override in Blueprint)
     * @param CameraLocation Current camera location
     * @param CameraRotation Current camera rotation
     * @return Custom generated caption
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Photo Mode Events")
    FString GenerateCustomPhotoCaption(const FVector& CameraLocation, const FRotator& CameraRotation);

    /**
     * Called to apply custom visual effects (override in Blueprint)
     * @param Settings Current visual settings
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Photo Mode Events")
    void ApplyCustomVisualEffects(const FPhotoModeVisualSettings& Settings);
};
