// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Cinematics/CinematicCameraSplineActor.h"
#include "Enemies/BossIntroNarrationComponent.h"
#include "BossEntranceSequenceActor.generated.h"

/**
 * Boss entrance types
 */
UENUM(BlueprintType)
enum class EBossEntranceType : uint8
{
    Dramatic            UMETA(DisplayName = "Dramatic Reveal"),
    Explosive           UMETA(DisplayName = "Explosive Entry"),
    Mysterious          UMETA(DisplayName = "Mysterious Appearance"),
    Regal               UMETA(DisplayName = "Regal Procession"),
    Menacing            UMETA(DisplayName = "Menacing Approach"),
    Teleportation       UMETA(DisplayName = "Teleportation"),
    Transformation      UMETA(DisplayName = "Transformation"),
    Awakening           UMETA(DisplayName = "Ancient Awakening"),
    Descent             UMETA(DisplayName = "Descent from Above"),
    Emergence           UMETA(DisplayName = "Emergence from Below"),
    PhaseShift          UMETA(DisplayName = "Phase Shift"),
    Summoning           UMETA(DisplayName = "Summoning Ritual")
};

/**
 * Boss entrance phase
 */
UENUM(BlueprintType)
enum class EBossEntrancePhase : uint8
{
    PreEntrance         UMETA(DisplayName = "Pre-Entrance"),
    Entrance            UMETA(DisplayName = "Entrance"),
    Reveal              UMETA(DisplayName = "Reveal"),
    Intimidation        UMETA(DisplayName = "Intimidation"),
    PostEntrance        UMETA(DisplayName = "Post-Entrance"),
    Completed           UMETA(DisplayName = "Completed")
};

/**
 * Boss entrance effect data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossEntranceEffect
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    FString EffectID;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    EBossEntrancePhase TriggerPhase;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    float TriggerTime; // Time within phase to trigger

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    UParticleSystem* ParticleEffect;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    USoundBase* SoundEffect;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    FVector EffectLocation; // Relative to boss

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    FRotator EffectRotation;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    float EffectScale;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    float EffectDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Effect")
    bool bAttachToBoss; // Whether effect follows boss

    FBossEntranceEffect()
    {
        EffectID = TEXT("");
        TriggerPhase = EBossEntrancePhase::Entrance;
        TriggerTime = 0.0f;
        ParticleEffect = nullptr;
        SoundEffect = nullptr;
        EffectLocation = FVector::ZeroVector;
        EffectRotation = FRotator::ZeroRotator;
        EffectScale = 1.0f;
        EffectDuration = 3.0f;
        bAttachToBoss = false;
    }
};

/**
 * Boss entrance sequence data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossEntranceSequence
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    FString SequenceID;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    EBossEntranceType EntranceType;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    TArray<UAnimMontage*> AnimationSequence;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    TArray<float> AnimationTimings; // When each animation starts

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    TArray<FBossEntranceEffect> Effects;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    float TotalDuration;

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    FString CameraSequenceID; // Camera sequence to use

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    FString MusicTrack; // Music for the entrance

    UPROPERTY(BlueprintReadWrite, Category = "Entrance Sequence")
    bool bPauseGameplay; // Whether to pause gameplay during entrance

    FBossEntranceSequence()
    {
        SequenceID = TEXT("");
        EntranceType = EBossEntranceType::Dramatic;
        TotalDuration = 10.0f;
        CameraSequenceID = TEXT("");
        MusicTrack = TEXT("boss_entrance_theme");
        bPauseGameplay = true;
    }
};

/**
 * Boss entrance events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossEntranceStarted, const FBossEntranceSequence&, EntranceSequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossEntrancePhaseChanged, EBossEntrancePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossEntranceCompleted, const FString&, SequenceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossEntranceEffectTriggered, const FString&, EffectID, const FBossEntranceEffect&, Effect);

/**
 * Boss Entrance Sequence Actor - Make boss intros unforgettable with synchronized animations
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ABossEntranceSequenceActor : public AActor
{
    GENERATED_BODY()

public:
    ABossEntranceSequenceActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Start boss entrance sequence
     * @param BossActor The boss actor
     * @param EntranceType Type of entrance to perform
     * @param CustomSequence Custom sequence data (optional)
     * @return True if entrance started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    bool StartBossEntrance(AActor* BossActor, EBossEntranceType EntranceType, const FBossEntranceSequence& CustomSequence = FBossEntranceSequence());

    /**
     * Stop current boss entrance
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    void StopBossEntrance();

    /**
     * Skip to entrance phase
     * @param Phase Phase to skip to
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    void SkipToPhase(EBossEntrancePhase Phase);

    /**
     * Add entrance sequence
     * @param Sequence Entrance sequence to add
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    void AddEntranceSequence(const FBossEntranceSequence& Sequence);

    /**
     * Remove entrance sequence
     * @param SequenceID ID of sequence to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    void RemoveEntranceSequence(const FString& SequenceID);

    /**
     * Get entrance sequence for type
     * @param EntranceType Type of entrance
     * @return Entrance sequence data
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    FBossEntranceSequence GetEntranceSequenceForType(EBossEntranceType EntranceType) const;

    /**
     * Check if entrance is active
     * @return True if boss entrance is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Entrance")
    bool IsEntranceActive() const { return bEntranceActive; }

    /**
     * Get current entrance phase
     * @return Current entrance phase
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Entrance")
    EBossEntrancePhase GetCurrentPhase() const { return CurrentPhase; }

    /**
     * Get entrance progress
     * @return Progress from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss Entrance")
    float GetEntranceProgress() const;

    /**
     * Set camera spline actor
     * @param CameraSpline Camera spline for cinematics
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    void SetCameraSplineActor(ACinematicCameraSplineActor* CameraSpline);

    /**
     * Set boss intro narration component
     * @param IntroNarration Intro narration component
     */
    UFUNCTION(BlueprintCallable, Category = "Boss Entrance")
    void SetBossIntroNarration(UBossIntroNarrationComponent* IntroNarration);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Boss Entrance Events")
    FOnBossEntranceStarted OnBossEntranceStarted;

    UPROPERTY(BlueprintAssignable, Category = "Boss Entrance Events")
    FOnBossEntrancePhaseChanged OnBossEntrancePhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Boss Entrance Events")
    FOnBossEntranceCompleted OnBossEntranceCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Boss Entrance Events")
    FOnBossEntranceEffectTriggered OnBossEntranceEffectTriggered;

protected:
    // Entrance sequences
    UPROPERTY(BlueprintReadOnly, Category = "Entrance Sequences")
    TMap<EBossEntranceType, FBossEntranceSequence> EntranceSequences;

    // Current entrance state
    UPROPERTY(BlueprintReadOnly, Category = "Entrance State")
    FBossEntranceSequence CurrentSequence;

    UPROPERTY(BlueprintReadOnly, Category = "Entrance State")
    bool bEntranceActive;

    UPROPERTY(BlueprintReadOnly, Category = "Entrance State")
    EBossEntrancePhase CurrentPhase;

    UPROPERTY(BlueprintReadOnly, Category = "Entrance State")
    float EntranceStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Entrance State")
    float PhaseStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Entrance State")
    int32 CurrentAnimationIndex;

    // Boss reference
    UPROPERTY(BlueprintReadOnly, Category = "Boss Reference")
    AActor* BossActor;

    UPROPERTY(BlueprintReadOnly, Category = "Boss Reference")
    USkeletalMeshComponent* BossSkeletalMesh;

    // Component references
    UPROPERTY()
    ACinematicCameraSplineActor* CameraSplineActor;

    UPROPERTY()
    UBossIntroNarrationComponent* BossIntroNarrationComponent;

    // Effect components
    UPROPERTY(BlueprintReadOnly, Category = "Effect Components")
    TArray<UParticleSystemComponent*> ActiveParticleEffects;

    UPROPERTY(BlueprintReadOnly, Category = "Effect Components")
    TArray<UAudioComponent*> ActiveAudioEffects;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entrance Settings")
    bool bAllowSkipping; // Whether entrance can be skipped

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entrance Settings")
    float EffectVolume; // Volume for entrance effects

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entrance Settings")
    bool bPausePlayerInput; // Whether to pause player input

    // Timer handles
    FTimerHandle PhaseTimer;
    FTimerHandle EffectTimer;

private:
    // Helper methods
    void LoadDefaultEntranceSequences();
    void StartEntrancePhase(EBossEntrancePhase Phase);
    void UpdateEntranceSequence(float DeltaTime);
    void TriggerEntranceEffects();
    void PlayEntranceAnimation(UAnimMontage* Animation);
    void TriggerEntranceEffect(const FBossEntranceEffect& Effect);
    void CleanupEffects();

    // Phase management
    void StartPreEntrancePhase();
    void StartEntrancePhase();
    void StartRevealPhase();
    void StartIntimidationPhase();
    void StartPostEntrancePhase();
    void CompleteEntrance();

    // Effect management
    UParticleSystemComponent* SpawnParticleEffect(const FBossEntranceEffect& Effect);
    UAudioComponent* SpawnAudioEffect(const FBossEntranceEffect& Effect);

    // Timer callbacks
    UFUNCTION()
    void OnPhaseTimer();

    UFUNCTION()
    void OnEffectTimer();

public:
    /**
     * Blueprint implementable events for custom boss entrance logic
     */

    /**
     * Called when boss entrance starts (for custom setup)
     * @param EntranceSequence The entrance sequence data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Entrance Events")
    void OnBossEntranceStartedEvent(const FBossEntranceSequence& EntranceSequence);

    /**
     * Called when entrance phase changes (for custom effects)
     * @param NewPhase The new entrance phase
     * @param PreviousPhase The previous entrance phase
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Entrance Events")
    void OnBossEntrancePhaseChangedEvent(EBossEntrancePhase NewPhase, EBossEntrancePhase PreviousPhase);

    /**
     * Called when entrance effect is triggered (for custom processing)
     * @param EffectID ID of the triggered effect
     * @param Effect The effect data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Entrance Events")
    void OnBossEntranceEffectTriggeredEvent(const FString& EffectID, const FBossEntranceEffect& Effect);

    /**
     * Called to customize entrance sequence (override in Blueprint)
     * @param BossActor The boss actor
     * @param BaseSequence Base entrance sequence
     * @return Customized entrance sequence
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Entrance Events")
    FBossEntranceSequence CustomizeEntranceSequence(AActor* BossActor, const FBossEntranceSequence& BaseSequence);

    /**
     * Called to generate dynamic entrance effects (override in Blueprint)
     * @param EntranceType Type of entrance
     * @param BossActor The boss actor
     * @return Array of dynamic effects
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Entrance Events")
    TArray<FBossEntranceEffect> GenerateDynamicEntranceEffects(EBossEntranceType EntranceType, AActor* BossActor);
};
