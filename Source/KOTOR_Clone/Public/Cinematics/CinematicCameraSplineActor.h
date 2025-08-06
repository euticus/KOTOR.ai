// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/InterpCurveFloat.h"
#include "AIDM/AIDirectorComponent.h"
#include "CinematicCameraSplineActor.generated.h"

/**
 * Camera movement types
 */
UENUM(BlueprintType)
enum class ECameraMovementType : uint8
{
    Linear              UMETA(DisplayName = "Linear"),
    Smooth              UMETA(DisplayName = "Smooth"),
    EaseIn              UMETA(DisplayName = "Ease In"),
    EaseOut             UMETA(DisplayName = "Ease Out"),
    EaseInOut           UMETA(DisplayName = "Ease In Out"),
    Bounce              UMETA(DisplayName = "Bounce"),
    Elastic             UMETA(DisplayName = "Elastic"),
    Custom              UMETA(DisplayName = "Custom Curve")
};

/**
 * Camera focus types
 */
UENUM(BlueprintType)
enum class ECameraFocusType : uint8
{
    FixedTarget         UMETA(DisplayName = "Fixed Target"),
    MovingTarget        UMETA(DisplayName = "Moving Target"),
    SplineDirection     UMETA(DisplayName = "Spline Direction"),
    PlayerCharacter     UMETA(DisplayName = "Player Character"),
    CustomLookAt        UMETA(DisplayName = "Custom Look At"),
    NoFocus             UMETA(DisplayName = "No Focus")
};

/**
 * Camera keyframe data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCameraKeyframe
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    float Time; // Time along spline (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    FVector Position; // World position override (optional)

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    FRotator Rotation; // World rotation override (optional)

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    float FOV; // Field of view

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    float FocusDistance; // Focus distance for depth of field

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    float Aperture; // Aperture for depth of field

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    AActor* LookAtTarget; // Target to look at

    UPROPERTY(BlueprintReadWrite, Category = "Camera Keyframe")
    FString EventTrigger; // Event to trigger at this keyframe

    FCameraKeyframe()
    {
        Time = 0.0f;
        Position = FVector::ZeroVector;
        Rotation = FRotator::ZeroRotator;
        FOV = 90.0f;
        FocusDistance = 1000.0f;
        Aperture = 2.8f;
        LookAtTarget = nullptr;
        EventTrigger = TEXT("");
    }
};

/**
 * Camera spline settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCameraSplineSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    float Duration; // Total duration of camera movement

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    ECameraMovementType MovementType;

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    ECameraFocusType FocusType;

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    bool bLooping; // Whether movement should loop

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    bool bAutoStart; // Start movement automatically

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    bool bSmoothRotation; // Smooth rotation interpolation

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    float RotationSpeed; // Speed of rotation changes

    UPROPERTY(BlueprintReadWrite, Category = "Spline Settings")
    UCurveFloat* CustomMovementCurve; // Custom movement curve

    FCameraSplineSettings()
    {
        Duration = 10.0f;
        MovementType = ECameraMovementType::Smooth;
        FocusType = ECameraFocusType::SplineDirection;
        bLooping = false;
        bAutoStart = false;
        bSmoothRotation = true;
        RotationSpeed = 1.0f;
        CustomMovementCurve = nullptr;
    }
};

/**
 * Camera spline events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraMovementStarted, ACinematicCameraSplineActor*, SplineActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraMovementCompleted, ACinematicCameraSplineActor*, SplineActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraKeyframeReached, ACinematicCameraSplineActor*, SplineActor, const FCameraKeyframe&, Keyframe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraEventTriggered, const FString&, EventName, float, CurrentTime);

/**
 * Cinematic Camera Spline Actor - AIDM-controlled camera paths for dramatic scenes
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ACinematicCameraSplineActor : public AActor
{
    GENERATED_BODY()

public:
    ACinematicCameraSplineActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Start camera movement
     * @param StartTime Time to start from (0.0 to 1.0)
     * @return True if movement started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    bool StartCameraMovement(float StartTime = 0.0f);

    /**
     * Stop camera movement
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void StopCameraMovement();

    /**
     * Pause/unpause camera movement
     * @param bPause Whether to pause or unpause
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void PauseCameraMovement(bool bPause);

    /**
     * Set camera movement time
     * @param Time Time along spline (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void SetCameraTime(float Time);

    /**
     * Get current camera time
     * @return Current time along spline (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera Spline")
    float GetCurrentCameraTime() const { return CurrentTime; }

    /**
     * Add camera keyframe
     * @param Keyframe Keyframe to add
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void AddCameraKeyframe(const FCameraKeyframe& Keyframe);

    /**
     * Remove camera keyframe
     * @param Index Index of keyframe to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void RemoveCameraKeyframe(int32 Index);

    /**
     * Clear all keyframes
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void ClearCameraKeyframes();

    /**
     * Set spline settings
     * @param Settings New spline settings
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void SetSplineSettings(const FCameraSplineSettings& Settings);

    /**
     * Get spline settings
     * @return Current spline settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera Spline")
    FCameraSplineSettings GetSplineSettings() const { return SplineSettings; }

    /**
     * Set look at target
     * @param Target Actor to look at
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    void SetLookAtTarget(AActor* Target);

    /**
     * Get camera at time
     * @param Time Time along spline (0.0 to 1.0)
     * @return Camera transform at specified time
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera Spline")
    FTransform GetCameraTransformAtTime(float Time) const;

    /**
     * Check if camera is moving
     * @return True if camera movement is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera Spline")
    bool IsCameraMoving() const { return bIsMoving; }

    /**
     * Generate spline from AIDM data
     * @param AIDirector AI Director component for context
     * @param SceneType Type of scene (dialogue, combat, exploration)
     * @param Participants Actors involved in the scene
     * @return True if spline was generated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Camera Spline")
    bool GenerateSplineFromAIDM(UAIDirectorComponent* AIDirector, const FString& SceneType, const TArray<AActor*>& Participants);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Camera Spline Events")
    FOnCameraMovementStarted OnCameraMovementStarted;

    UPROPERTY(BlueprintAssignable, Category = "Camera Spline Events")
    FOnCameraMovementCompleted OnCameraMovementCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Camera Spline Events")
    FOnCameraKeyframeReached OnCameraKeyframeReached;

    UPROPERTY(BlueprintAssignable, Category = "Camera Spline Events")
    FOnCameraEventTriggered OnCameraEventTriggered;

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USplineComponent* CameraSpline;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTimelineComponent* MovementTimeline;

    // Spline settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline Settings")
    FCameraSplineSettings SplineSettings;

    // Camera keyframes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Keyframes")
    TArray<FCameraKeyframe> CameraKeyframes;

    // Movement state
    UPROPERTY(BlueprintReadOnly, Category = "Movement State")
    bool bIsMoving;

    UPROPERTY(BlueprintReadOnly, Category = "Movement State")
    bool bIsPaused;

    UPROPERTY(BlueprintReadOnly, Category = "Movement State")
    float CurrentTime;

    UPROPERTY(BlueprintReadOnly, Category = "Movement State")
    float MovementStartTime;

    // Look at target
    UPROPERTY(BlueprintReadOnly, Category = "Look At")
    AActor* LookAtTarget;

    // Timeline curve
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    UCurveFloat* MovementCurve;

private:
    // Helper methods
    void InitializeTimeline();
    void UpdateCameraPosition(float Alpha);
    void UpdateCameraRotation(float Alpha);
    void UpdateCameraProperties(float Alpha);
    void CheckKeyframes(float Alpha);
    FVector GetSplinePositionAtTime(float Time) const;
    FRotator GetSplineRotationAtTime(float Time) const;
    float GetMovementAlpha(float Time) const;
    void TriggerKeyframeEvent(const FCameraKeyframe& Keyframe);

    // AIDM generation helpers
    void GenerateDialogueSpline(const TArray<AActor*>& Participants);
    void GenerateCombatSpline(const TArray<AActor*>& Participants);
    void GenerateExplorationSpline(const TArray<AActor*>& Participants);
    FVector CalculateOptimalCameraPosition(const TArray<AActor*>& Participants, float Angle);

    // Timeline callbacks
    UFUNCTION()
    void OnTimelineUpdate(float Value);

    UFUNCTION()
    void OnTimelineFinished();

    // Last triggered keyframe index
    int32 LastTriggeredKeyframeIndex;

public:
    /**
     * Blueprint implementable events for custom camera spline logic
     */

    /**
     * Called when camera movement starts (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera Spline Events")
    void OnCameraMovementStartedEvent();

    /**
     * Called when camera keyframe is reached (for custom effects)
     * @param Keyframe The reached keyframe
     * @param CurrentTime Current movement time
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera Spline Events")
    void OnCameraKeyframeReachedEvent(const FCameraKeyframe& Keyframe, float CurrentTime);

    /**
     * Called to customize camera position (override in Blueprint)
     * @param SplinePosition Position from spline
     * @param Time Current time (0.0 to 1.0)
     * @return Customized camera position
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera Spline Events")
    FVector CustomizeCameraPosition(const FVector& SplinePosition, float Time);

    /**
     * Called to customize camera rotation (override in Blueprint)
     * @param SplineRotation Rotation from spline
     * @param Time Current time (0.0 to 1.0)
     * @return Customized camera rotation
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera Spline Events")
    FRotator CustomizeCameraRotation(const FRotator& SplineRotation, float Time);

    /**
     * Called to generate custom spline points (override in Blueprint)
     * @param SceneType Type of scene
     * @param Participants Actors in the scene
     * @return Array of spline points
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera Spline Events")
    TArray<FVector> GenerateCustomSplinePoints(const FString& SceneType, const TArray<AActor*>& Participants);

    /**
     * Called when camera event is triggered (override in Blueprint)
     * @param EventName Name of the triggered event
     * @param CurrentTime Current movement time
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera Spline Events")
    void OnCameraEventTriggeredEvent(const FString& EventName, float CurrentTime);
};
