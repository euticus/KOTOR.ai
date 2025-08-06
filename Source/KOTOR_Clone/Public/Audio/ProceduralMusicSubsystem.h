// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundMix.h"
#include "ProceduralMusicSubsystem.generated.h"

/**
 * Music layer types
 */
UENUM(BlueprintType)
enum class EMusicLayerType : uint8
{
    Ambient             UMETA(DisplayName = "Ambient"),
    Melody              UMETA(DisplayName = "Melody"),
    Harmony             UMETA(DisplayName = "Harmony"),
    Rhythm              UMETA(DisplayName = "Rhythm"),
    Bass                UMETA(DisplayName = "Bass"),
    Percussion          UMETA(DisplayName = "Percussion"),
    Strings             UMETA(DisplayName = "Strings"),
    Brass               UMETA(DisplayName = "Brass"),
    Woodwinds           UMETA(DisplayName = "Woodwinds"),
    Choir               UMETA(DisplayName = "Choir"),
    Synth               UMETA(DisplayName = "Synth"),
    Effects             UMETA(DisplayName = "Effects"),
    Tension             UMETA(DisplayName = "Tension"),
    Action              UMETA(DisplayName = "Action"),
    Emotional           UMETA(DisplayName = "Emotional"),
    Atmospheric         UMETA(DisplayName = "Atmospheric")
};

/**
 * Music mood types
 */
UENUM(BlueprintType)
enum class EMusicMood : uint8
{
    Neutral             UMETA(DisplayName = "Neutral"),
    Peaceful            UMETA(DisplayName = "Peaceful"),
    Tense               UMETA(DisplayName = "Tense"),
    Action              UMETA(DisplayName = "Action"),
    Dramatic            UMETA(DisplayName = "Dramatic"),
    Mysterious          UMETA(DisplayName = "Mysterious"),
    Heroic              UMETA(DisplayName = "Heroic"),
    Dark                UMETA(DisplayName = "Dark"),
    Romantic            UMETA(DisplayName = "Romantic"),
    Sad                 UMETA(DisplayName = "Sad"),
    Triumphant          UMETA(DisplayName = "Triumphant"),
    Suspenseful         UMETA(DisplayName = "Suspenseful"),
    Epic                UMETA(DisplayName = "Epic"),
    Melancholic         UMETA(DisplayName = "Melancholic"),
    Aggressive          UMETA(DisplayName = "Aggressive"),
    Serene              UMETA(DisplayName = "Serene")
};

/**
 * Music context types
 */
UENUM(BlueprintType)
enum class EMusicContext : uint8
{
    Exploration         UMETA(DisplayName = "Exploration"),
    Combat              UMETA(DisplayName = "Combat"),
    Dialogue            UMETA(DisplayName = "Dialogue"),
    Cutscene            UMETA(DisplayName = "Cutscene"),
    Menu                UMETA(DisplayName = "Menu"),
    Victory             UMETA(DisplayName = "Victory"),
    Defeat              UMETA(DisplayName = "Defeat"),
    Discovery           UMETA(DisplayName = "Discovery"),
    Stealth             UMETA(DisplayName = "Stealth"),
    Puzzle              UMETA(DisplayName = "Puzzle"),
    Shopping            UMETA(DisplayName = "Shopping"),
    Meditation          UMETA(DisplayName = "Meditation"),
    Travel              UMETA(DisplayName = "Travel"),
    Boss                UMETA(DisplayName = "Boss"),
    Ambient             UMETA(DisplayName = "Ambient"),
    Transition          UMETA(DisplayName = "Transition")
};

/**
 * Music layer data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicLayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    FString LayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    EMusicLayerType LayerType;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    USoundWave* AudioStem;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float Volume; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float TargetVolume; // Target volume for blending

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    bool bIsLooping;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float FadeInTime;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float FadeOutTime;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    int32 Priority; // Higher priority layers take precedence

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    TArray<EMusicMood> SupportedMoods;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    TArray<EMusicContext> SupportedContexts;

    FMusicLayerData()
    {
        LayerID = TEXT("");
        LayerType = EMusicLayerType::Ambient;
        AudioStem = nullptr;
        Volume = 1.0f;
        TargetVolume = 1.0f;
        bIsActive = false;
        bIsLooping = true;
        FadeInTime = 2.0f;
        FadeOutTime = 2.0f;
        Priority = 1;
    }
};

/**
 * Music composition data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicComposition
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    FString CompositionID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    FString CompositionName;

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    TArray<FMusicLayerData> Layers;

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    EMusicMood PrimaryMood;

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    EMusicContext PrimaryContext;

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    float DefaultTempo; // BPM

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    FString Key; // Musical key

    UPROPERTY(BlueprintReadWrite, Category = "Music Composition")
    float Duration; // Duration in seconds

    FMusicComposition()
    {
        CompositionID = TEXT("");
        CompositionName = TEXT("Untitled Composition");
        PrimaryMood = EMusicMood::Neutral;
        PrimaryContext = EMusicContext::Exploration;
        DefaultTempo = 120.0f;
        Key = TEXT("C Major");
        Duration = 0.0f;
    }
};

/**
 * Music state data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    EMusicMood CurrentMood;

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    EMusicContext CurrentContext;

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    float Intensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    float Tension; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    float Energy; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    float PlayerAlignment; // -1.0 (dark) to 1.0 (light)

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    bool bInCombat;

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    bool bInDialogue;

    UPROPERTY(BlueprintReadWrite, Category = "Music State")
    bool bInCutscene;

    FMusicState()
    {
        CurrentMood = EMusicMood::Neutral;
        CurrentContext = EMusicContext::Exploration;
        Intensity = 0.5f;
        Tension = 0.0f;
        Energy = 0.5f;
        PlayerAlignment = 0.0f;
        bInCombat = false;
        bInDialogue = false;
        bInCutscene = false;
    }
};

/**
 * Music events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicMoodChanged, EMusicMood, OldMood, EMusicMood, NewMood);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicContextChanged, EMusicContext, OldContext, EMusicContext, NewContext);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicLayerChanged, const FString&, LayerID, bool, bIsActive);

/**
 * Procedural Music Subsystem - Master controller for all soundtrack blending
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UProceduralMusicSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UProceduralMusicSubsystem();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // UGameInstanceSubsystem interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
    /**
     * Set music mood
     * @param NewMood New music mood
     * @param BlendTime Time to blend to new mood
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicMood(EMusicMood NewMood, float BlendTime = 3.0f);

    /**
     * Set music context
     * @param NewContext New music context
     * @param BlendTime Time to blend to new context
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicContext(EMusicContext NewContext, float BlendTime = 2.0f);

    /**
     * Set music intensity
     * @param Intensity Intensity from 0.0 to 1.0
     * @param BlendTime Time to blend to new intensity
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicIntensity(float Intensity, float BlendTime = 1.0f);

    /**
     * Set music tension
     * @param Tension Tension from 0.0 to 1.0
     * @param BlendTime Time to blend to new tension
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicTension(float Tension, float BlendTime = 1.5f);

    /**
     * Set music energy
     * @param Energy Energy from 0.0 to 1.0
     * @param BlendTime Time to blend to new energy
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicEnergy(float Energy, float BlendTime = 2.0f);

    /**
     * Update music for player alignment
     * @param Alignment Player alignment (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void UpdateMusicForAlignment(float Alignment);

    /**
     * Add music composition
     * @param Composition Composition to add
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void AddMusicComposition(const FMusicComposition& Composition);

    /**
     * Remove music composition
     * @param CompositionID ID of composition to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void RemoveMusicComposition(const FString& CompositionID);

    /**
     * Load music compositions from JSON
     * @param FilePath Path to JSON file
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    bool LoadMusicCompositions(const FString& FilePath);

    /**
     * Get current music state
     * @return Current music state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    FMusicState GetCurrentMusicState() const { return CurrentMusicState; }

    /**
     * Get active layers
     * @return Array of currently active layer IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    TArray<FString> GetActiveLayers() const;

    /**
     * Set layer volume
     * @param LayerID ID of layer
     * @param Volume Volume from 0.0 to 1.0
     * @param BlendTime Time to blend to new volume
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetLayerVolume(const FString& LayerID, float Volume, float BlendTime = 1.0f);

    /**
     * Enable/disable music layer
     * @param LayerID ID of layer
     * @param bEnabled Whether layer should be enabled
     * @param BlendTime Time to fade in/out
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetLayerEnabled(const FString& LayerID, bool bEnabled, float BlendTime = 2.0f);

    /**
     * Stop all music
     * @param FadeOutTime Time to fade out
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void StopAllMusic(float FadeOutTime = 3.0f);

    /**
     * Pause/unpause music
     * @param bPaused Whether music should be paused
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicPaused(bool bPaused);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicMoodChanged OnMusicMoodChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicContextChanged OnMusicContextChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicLayerChanged OnMusicLayerChanged;

protected:
    // Music compositions
    UPROPERTY(BlueprintReadOnly, Category = "Music Compositions")
    TMap<FString, FMusicComposition> MusicCompositions;

    // Active music layers
    UPROPERTY(BlueprintReadOnly, Category = "Active Layers")
    TMap<FString, UAudioComponent*> ActiveAudioComponents;

    // Current music state
    UPROPERTY(BlueprintReadOnly, Category = "Music State")
    FMusicState CurrentMusicState;

    UPROPERTY(BlueprintReadOnly, Category = "Music State")
    FMusicState TargetMusicState;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Settings")
    float MasterVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Settings")
    bool bMusicEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Settings")
    float DefaultBlendTime;

    // Timer handle
    FTimerHandle MusicUpdateTimer;

private:
    // Helper methods
    void LoadDefaultCompositions();
    void UpdateMusicLayers();
    void BlendToTargetState(float DeltaTime);
    void UpdateLayerVolumes();
    FMusicComposition* FindBestComposition(const FMusicState& State);
    void ActivateComposition(const FMusicComposition& Composition);
    void DeactivateAllLayers();
    UAudioComponent* CreateAudioComponent(const FMusicLayerData& LayerData);

    // Timer callback
    UFUNCTION()
    void OnMusicUpdateTimer();

public:
    /**
     * Blueprint implementable events for custom music logic
     */

    /**
     * Called when music mood changes (for custom processing)
     * @param OldMood Previous mood
     * @param NewMood New mood
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    void OnMusicMoodChangedEvent(EMusicMood OldMood, EMusicMood NewMood);

    /**
     * Called when music context changes (for custom processing)
     * @param OldContext Previous context
     * @param NewContext New context
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    void OnMusicContextChangedEvent(EMusicContext OldContext, EMusicContext NewContext);

    /**
     * Called to customize music selection (override in Blueprint)
     * @param MusicState Current music state
     * @return Custom composition ID
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    FString SelectCustomComposition(const FMusicState& MusicState);

    /**
     * Called to customize layer volumes (override in Blueprint)
     * @param LayerData Layer data
     * @param MusicState Current music state
     * @return Custom volume multiplier
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    float CustomizeLayerVolume(const FMusicLayerData& LayerData, const FMusicState& MusicState);
};
