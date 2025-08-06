// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundMix.h"
#include "ProceduralMusicSubsystemV2.generated.h"

/**
 * Music stem types
 */
UENUM(BlueprintType)
enum class EMusicStemType : uint8
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
 * Music biome types
 */
UENUM(BlueprintType)
enum class EMusicBiome : uint8
{
    Taris               UMETA(DisplayName = "Taris"),
    Dantooine           UMETA(DisplayName = "Dantooine"),
    Korriban            UMETA(DisplayName = "Korriban"),
    Kashyyyk            UMETA(DisplayName = "Kashyyyk"),
    Tatooine            UMETA(DisplayName = "Tatooine"),
    Manaan              UMETA(DisplayName = "Manaan"),
    StarForge           UMETA(DisplayName = "Star Forge"),
    Endar_Spire         UMETA(DisplayName = "Endar Spire"),
    Leviathan           UMETA(DisplayName = "Leviathan"),
    Unknown_World       UMETA(DisplayName = "Unknown World"),
    Space               UMETA(DisplayName = "Space"),
    Generic             UMETA(DisplayName = "Generic")
};

/**
 * Music tone types
 */
UENUM(BlueprintType)
enum class EMusicTone : uint8
{
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
    Serene              UMETA(DisplayName = "Serene"),
    Neutral             UMETA(DisplayName = "Neutral")
};

/**
 * Music stem data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicStemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    FString StemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    EMusicStemType StemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    USoundWave* AudioStem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    EMusicBiome Biome;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    EMusicTone Tone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    float Volume; // 0.0 to 1.0

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    float TargetVolume; // Target volume for blending

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    bool bIsLooping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    float FadeInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    float FadeOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    int32 Priority; // Higher priority stems take precedence

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    bool bCombatOnly; // Only plays during combat

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Stem")
    bool bDialogueOnly; // Only plays during dialogue

    FMusicStemData()
    {
        StemID = TEXT("");
        StemType = EMusicStemType::Ambient;
        AudioStem = nullptr;
        Biome = EMusicBiome::Generic;
        Tone = EMusicTone::Neutral;
        Volume = 1.0f;
        TargetVolume = 1.0f;
        bIsActive = false;
        bIsLooping = true;
        FadeInTime = 2.0f;
        FadeOutTime = 2.0f;
        Priority = 1;
        bCombatOnly = false;
        bDialogueOnly = false;
    }
};

/**
 * Music blend preset
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicBlendPreset
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    FString PresetID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    EMusicBiome Biome;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    EMusicTone Tone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    bool bCombatMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    TArray<FString> ActiveStems; // Stems to activate

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    TMap<FString, float> StemVolumes; // Volume overrides for specific stems

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Blend Preset")
    float BlendTime; // Time to transition to this preset

    FMusicBlendPreset()
    {
        PresetID = TEXT("");
        Biome = EMusicBiome::Generic;
        Tone = EMusicTone::Neutral;
        bCombatMode = false;
        BlendTime = 3.0f;
    }
};

/**
 * Music events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicBiomeChanged, EMusicBiome, OldBiome, EMusicBiome, NewBiome);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicToneChanged, EMusicTone, OldTone, EMusicTone, NewTone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicStemChanged, const FString&, StemID, bool, bIsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicBlendPresetChanged, const FString&, PresetID);

/**
 * Procedural Music Subsystem V2 - Music subsystem with stem loading and control API
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UProceduralMusicSubsystemV2 : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UProceduralMusicSubsystemV2();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // UGameInstanceSubsystem interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
    /**
     * Load music stems from directory
     * @param StemsDirectory Directory containing music stems
     * @return Number of stems loaded
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    int32 LoadMusicStems(const FString& StemsDirectory);

    /**
     * Add music stem
     * @param StemData Stem data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void AddMusicStem(const FMusicStemData& StemData);

    /**
     * Remove music stem
     * @param StemID ID of stem to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void RemoveMusicStem(const FString& StemID);

    /**
     * Set music biome
     * @param NewBiome New music biome
     * @param BlendTime Time to blend to new biome
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicBiome(EMusicBiome NewBiome, float BlendTime = 3.0f);

    /**
     * Set music tone
     * @param NewTone New music tone
     * @param BlendTime Time to blend to new tone
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMusicTone(EMusicTone NewTone, float BlendTime = 2.0f);

    /**
     * Set combat mode
     * @param bCombatMode Whether combat mode is active
     * @param BlendTime Time to transition
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetCombatMode(bool bCombatMode, float BlendTime = 1.5f);

    /**
     * Activate music stem
     * @param StemID ID of stem to activate
     * @param Volume Volume to set (0.0 to 1.0)
     * @param FadeTime Time to fade in
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void ActivateMusicStem(const FString& StemID, float Volume = 1.0f, float FadeTime = 2.0f);

    /**
     * Deactivate music stem
     * @param StemID ID of stem to deactivate
     * @param FadeTime Time to fade out
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void DeactivateMusicStem(const FString& StemID, float FadeTime = 2.0f);

    /**
     * Set stem volume
     * @param StemID ID of stem
     * @param Volume Volume to set (0.0 to 1.0)
     * @param BlendTime Time to blend to new volume
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetStemVolume(const FString& StemID, float Volume, float BlendTime = 1.0f);

    /**
     * Apply blend preset
     * @param PresetID ID of preset to apply
     * @return True if preset was applied successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    bool ApplyBlendPreset(const FString& PresetID);

    /**
     * Add blend preset
     * @param BlendPreset Blend preset to add
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void AddBlendPreset(const FMusicBlendPreset& BlendPreset);

    /**
     * Get current biome
     * @return Current music biome
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    EMusicBiome GetCurrentBiome() const { return CurrentBiome; }

    /**
     * Get current tone
     * @return Current music tone
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    EMusicTone GetCurrentTone() const { return CurrentTone; }

    /**
     * Check if in combat mode
     * @return True if in combat mode
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    bool IsInCombatMode() const { return bCombatMode; }

    /**
     * Get active stems
     * @return Array of currently active stem IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    TArray<FString> GetActiveStems() const;

    /**
     * Get stem data
     * @param StemID ID of stem
     * @return Stem data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Music")
    FMusicStemData GetStemData(const FString& StemID) const;

    /**
     * Stop all music
     * @param FadeOutTime Time to fade out
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void StopAllMusic(float FadeOutTime = 3.0f);

    /**
     * Set master volume
     * @param Volume Master volume (0.0 to 1.0)
     * @param BlendTime Time to blend to new volume
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Music")
    void SetMasterVolume(float Volume, float BlendTime = 1.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicBiomeChanged OnMusicBiomeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicToneChanged OnMusicToneChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicStemChanged OnMusicStemChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Events")
    FOnMusicBlendPresetChanged OnMusicBlendPresetChanged;

protected:
    // Music stems
    UPROPERTY(BlueprintReadOnly, Category = "Music Stems")
    TMap<FString, FMusicStemData> MusicStems;

    // Active audio components
    UPROPERTY(BlueprintReadOnly, Category = "Active Audio")
    TMap<FString, UAudioComponent*> ActiveAudioComponents;

    // Blend presets
    UPROPERTY(BlueprintReadOnly, Category = "Blend Presets")
    TMap<FString, FMusicBlendPreset> BlendPresets;

    // Current music state
    UPROPERTY(BlueprintReadOnly, Category = "Music State")
    EMusicBiome CurrentBiome;

    UPROPERTY(BlueprintReadOnly, Category = "Music State")
    EMusicTone CurrentTone;

    UPROPERTY(BlueprintReadOnly, Category = "Music State")
    bool bCombatMode;

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
    void LoadDefaultStems();
    void LoadDefaultBlendPresets();
    void UpdateMusicStems();
    UAudioComponent* CreateAudioComponent(const FMusicStemData& StemData);
    void UpdateStemForCurrentState(const FString& StemID);
    bool ShouldStemBeActive(const FMusicStemData& StemData) const;

    // Timer callback
    UFUNCTION()
    void OnMusicUpdateTimer();

public:
    /**
     * Blueprint implementable events for custom music logic
     */

    /**
     * Called when music biome changes (for custom processing)
     * @param OldBiome Previous biome
     * @param NewBiome New biome
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    void OnMusicBiomeChangedEvent(EMusicBiome OldBiome, EMusicBiome NewBiome);

    /**
     * Called when music tone changes (for custom processing)
     * @param OldTone Previous tone
     * @param NewTone New tone
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    void OnMusicToneChangedEvent(EMusicTone OldTone, EMusicTone NewTone);

    /**
     * Called to customize stem selection (override in Blueprint)
     * @param Biome Current biome
     * @param Tone Current tone
     * @param bCombat Whether in combat
     * @return Custom stem IDs to activate
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Events")
    TArray<FString> CustomizeStemSelection(EMusicBiome Biome, EMusicTone Tone, bool bCombat);
};
