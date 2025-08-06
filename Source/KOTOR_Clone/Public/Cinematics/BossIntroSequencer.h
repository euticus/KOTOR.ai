// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "CineCameraActor.h"
#include "Components/AudioComponent.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "Audio/ProceduralMusicSubsystem.h"
#include "BossIntroSequencer.generated.h"

/**
 * Boss intro sequence types
 */
UENUM(BlueprintType)
enum class EBossIntroSequenceType : uint8
{
    Dramatic            UMETA(DisplayName = "Dramatic Reveal"),
    Menacing            UMETA(DisplayName = "Menacing Approach"),
    Mysterious          UMETA(DisplayName = "Mysterious Entrance"),
    Explosive           UMETA(DisplayName = "Explosive Arrival"),
    Regal               UMETA(DisplayName = "Regal Presentation"),
    Corrupted           UMETA(DisplayName = "Corrupted Transformation"),
    Ancient             UMETA(DisplayName = "Ancient Awakening"),
    Betrayal            UMETA(DisplayName = "Betrayal Reveal"),
    Teleportation       UMETA(DisplayName = "Teleportation"),
    Emergence           UMETA(DisplayName = "Emergence"),
    Descent             UMETA(DisplayName = "Descent"),
    Summoning           UMETA(DisplayName = "Summoning")
};

/**
 * Boss intro camera shot types
 */
UENUM(BlueprintType)
enum class EBossIntroCameraShot : uint8
{
    WideEstablishing    UMETA(DisplayName = "Wide Establishing"),
    MediumShot          UMETA(DisplayName = "Medium Shot"),
    CloseUp             UMETA(DisplayName = "Close Up"),
    ExtremeCloseUp      UMETA(DisplayName = "Extreme Close Up"),
    LowAngle            UMETA(DisplayName = "Low Angle"),
    HighAngle           UMETA(DisplayName = "High Angle"),
    OverShoulder        UMETA(DisplayName = "Over Shoulder"),
    Tracking            UMETA(DisplayName = "Tracking"),
    Dolly               UMETA(DisplayName = "Dolly"),
    Crane               UMETA(DisplayName = "Crane"),
    Handheld            UMETA(DisplayName = "Handheld"),
    Steadicam           UMETA(DisplayName = "Steadicam")
};

/**
 * Boss intro camera data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossIntroCameraData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    EBossIntroCameraShot ShotType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    FVector CameraLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    FRotator CameraRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    float FOV; // Field of view

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    float Duration; // Duration of this shot

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    AActor* LookAtTarget; // Target to look at (optional)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    bool bUseSpline; // Whether to use spline movement

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Camera")
    class USplineComponent* CameraSpline; // Spline for camera movement

    FBossIntroCameraData()
    {
        ShotType = EBossIntroCameraShot::MediumShot;
        CameraLocation = FVector::ZeroVector;
        CameraRotation = FRotator::ZeroRotator;
        FOV = 90.0f;
        Duration = 3.0f;
        LookAtTarget = nullptr;
        bUseSpline = false;
        CameraSpline = nullptr;
    }
};

/**
 * Boss intro sequence data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossIntroSequenceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    FString SequenceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    EBossIntroSequenceType SequenceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    ULevelSequence* LevelSequence; // Main level sequence

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    TArray<FBossIntroCameraData> CameraShots; // Camera shots for the intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    FString BossDialogue; // What the boss says

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    EPerformanceEmotion BossEmotion; // Boss emotion during intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    EPerformanceTone BossTone; // Boss tone during intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    FString MusicTrack; // Music track for the intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    EMusicMood MusicMood; // Music mood for the intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    float TotalDuration; // Total duration of the intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    bool bPauseGameplay; // Whether to pause gameplay during intro

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Intro Sequence")
    bool bSkippable; // Whether intro can be skipped

    FBossIntroSequenceData()
    {
        SequenceID = TEXT("");
        SequenceType = EBossIntroSequenceType::Dramatic;
        LevelSequence = nullptr;
        BossDialogue = TEXT("You dare challenge me?");
        BossEmotion = EPerformanceEmotion::Angry;
        BossTone = EPerformanceTone::Threatening;
        MusicTrack = TEXT("boss_intro_theme");
        MusicMood = EMusicMood::Dramatic;
        TotalDuration = 10.0f;
        bPauseGameplay = true;
        bSkippable = true;
    }
};

/**
 * Boss intro events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossIntroStarted, const FBossIntroSequenceData&, SequenceData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossIntroCompleted, const FString&, SequenceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossIntroSkipped, const FString&, SequenceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossIntroCameraChanged, int32, ShotIndex, const FBossIntroCameraData&, CameraData);

/**
 * Boss Intro Sequencer - Cinematic boss introduction with camera and music
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ABossIntroSequencer : public AActor
{
    GENERATED_BODY()

public:
    ABossIntroSequencer();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Start boss intro sequence
     * @param BossActor The boss actor
     * @param SequenceType Type of intro sequence
     * @param CustomSequenceData Custom sequence data (optional)
     * @return True if intro started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    bool StartBossIntro(AActor* BossActor, EBossIntroSequenceType SequenceType, 
                       const FBossIntroSequenceData& CustomSequenceData = FBossIntroSequenceData());

    /**
     * Stop current boss intro
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    void StopBossIntro();

    /**
     * Skip current boss intro
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    void SkipBossIntro();

    /**
     * Add intro sequence
     * @param SequenceData Sequence data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    void AddIntroSequence(const FBossIntroSequenceData& SequenceData);

    /**
     * Remove intro sequence
     * @param SequenceID ID of sequence to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    void RemoveIntroSequence(const FString& SequenceID);

    /**
     * Get sequence for type
     * @param SequenceType Type of sequence
     * @return Sequence data for the type
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    FBossIntroSequenceData GetSequenceForType(EBossIntroSequenceType SequenceType) const;

    /**
     * Set cinema camera
     * @param CameraActor Cinema camera actor to use
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Intro Sequencer")
    void SetCinemaCamera(ACineCameraActor* CameraActor);

    /**
     * Check if intro is playing
     * @return True if boss intro is currently playing
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Intro Sequencer")
    bool IsIntroPlaying() const { return bIntroPlaying; }

    /**
     * Get current sequence data
     * @return Current sequence data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Intro Sequencer")
    FBossIntroSequenceData GetCurrentSequenceData() const { return CurrentSequenceData; }

    /**
     * Get intro progress
     * @return Progress from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Intro Sequencer")
    float GetIntroProgress() const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossIntroStarted OnBossIntroStarted;

    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossIntroCompleted OnBossIntroCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossIntroSkipped OnBossIntroSkipped;

    UPROPERTY(BlueprintAssignable, Category = "Boss Intro Events")
    FOnBossIntroCameraChanged OnBossIntroCameraChanged;

protected:
    // Intro sequences
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intro Sequences")
    TMap<EBossIntroSequenceType, FBossIntroSequenceData> IntroSequences;

    // Current sequence state
    UPROPERTY(BlueprintReadOnly, Category = "Sequence State")
    FBossIntroSequenceData CurrentSequenceData;

    UPROPERTY(BlueprintReadOnly, Category = "Sequence State")
    bool bIntroPlaying;

    UPROPERTY(BlueprintReadOnly, Category = "Sequence State")
    float IntroStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Sequence State")
    int32 CurrentCameraShot;

    // Boss reference
    UPROPERTY(BlueprintReadOnly, Category = "Boss Reference")
    AActor* CurrentBossActor;

    // Component references
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    ALevelSequenceActor* LevelSequenceActor;

    UPROPERTY()
    ULevelSequencePlayer* SequencePlayer;

    UPROPERTY()
    ACineCameraActor* CinemaCamera;

    UPROPERTY()
    UProceduralMusicSubsystem* MusicSubsystem;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequencer Settings")
    bool bAllowSkipping; // Whether intros can be skipped

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequencer Settings")
    bool bAutoStartMusic; // Whether to automatically start music

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequencer Settings")
    bool bPausePlayerInput; // Whether to pause player input

    // Timer handles
    FTimerHandle CameraShotTimer;
    FTimerHandle IntroTimer;

private:
    // Helper methods
    void LoadDefaultSequences();
    void SetupLevelSequence(const FBossIntroSequenceData& SequenceData);
    void StartCameraSequence();
    void PlayNextCameraShot();
    void SetupBossPerformance();
    void StartIntroMusic();
    void CompleteIntro();

    // Camera management
    void SetCameraShot(const FBossIntroCameraData& CameraData);
    void MoveCameraToPosition(const FVector& Location, const FRotator& Rotation, float Duration);
    void SetCameraLookAt(AActor* Target);

    // Timer callbacks
    UFUNCTION()
    void OnCameraShotTimer();

    UFUNCTION()
    void OnIntroTimer();

    // Sequence events
    UFUNCTION()
    void OnSequenceFinished();

public:
    /**
     * Blueprint implementable events for custom boss intro logic
     */

    /**
     * Called when boss intro starts (for custom setup)
     * @param SequenceData The sequence data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    void OnBossIntroStartedEvent(const FBossIntroSequenceData& SequenceData);

    /**
     * Called when camera shot changes (for custom effects)
     * @param ShotIndex Index of the new shot
     * @param CameraData Camera data for the shot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    void OnBossIntroCameraChangedEvent(int32 ShotIndex, const FBossIntroCameraData& CameraData);

    /**
     * Called to customize sequence (override in Blueprint)
     * @param BossActor The boss actor
     * @param BaseSequence Base sequence data
     * @return Customized sequence data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    FBossIntroSequenceData CustomizeBossIntroSequence(AActor* BossActor, const FBossIntroSequenceData& BaseSequence);

    /**
     * Called to generate dynamic camera shots (override in Blueprint)
     * @param SequenceType Type of sequence
     * @param BossActor The boss actor
     * @return Array of dynamic camera shots
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Intro Events")
    TArray<FBossIntroCameraData> GenerateDynamicCameraShots(EBossIntroSequenceType SequenceType, AActor* BossActor);

#if WITH_EDITOR
    // Editor-only functions
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
