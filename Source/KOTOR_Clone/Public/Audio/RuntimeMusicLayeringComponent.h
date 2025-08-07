// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "RuntimeMusicLayeringComponent.generated.h"

/**
 * Music layer state
 */
UENUM(BlueprintType)
enum class EMusicLayerState : uint8
{
    Inactive            UMETA(DisplayName = "Inactive"),
    FadingIn            UMETA(DisplayName = "Fading In"),
    Active              UMETA(DisplayName = "Active"),
    FadingOut           UMETA(DisplayName = "Fading Out"),
    Paused              UMETA(DisplayName = "Paused")
};

/**
 * Music layer data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FRuntimeMusicLayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    FString LayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    EMusicStemType StemType;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    UAudioComponent* AudioComponent;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    EMusicLayerState LayerState;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float CurrentVolume; // Current volume (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float TargetVolume; // Target volume (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float BaseVolume; // Base volume for this layer

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float FadeStartTime; // When fade started

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    float FadeDuration; // How long fade takes

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    bool bIsLooping;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    int32 Priority; // Higher priority layers take precedence

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    EMusicBiome AssociatedBiome;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer")
    EMusicTone AssociatedTone;

    FMusicLayerData()
    {
        LayerID = TEXT("");
        StemType = EMusicStemType::Ambient;
        AudioComponent = nullptr;
        LayerState = EMusicLayerState::Inactive;
        CurrentVolume = 0.0f;
        TargetVolume = 0.0f;
        BaseVolume = 1.0f;
        FadeStartTime = 0.0f;
        FadeDuration = 2.0f;
        bIsLooping = true;
        Priority = 1;
        AssociatedBiome = EMusicBiome::Generic;
        AssociatedTone = EMusicTone::Neutral;
    }
};

/**
 * Music layer group
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMusicLayerGroup
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer Group")
    FString GroupID;

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer Group")
    TArray<FString> LayerIDs; // Layers in this group

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer Group")
    float GroupVolume; // Volume multiplier for entire group

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer Group")
    bool bMutuallyExclusive; // Only one layer in group can be active

    UPROPERTY(BlueprintReadWrite, Category = "Music Layer Group")
    bool bSynchronizedFading; // All layers fade together

    FMusicLayerGroup()
    {
        GroupID = TEXT("");
        GroupVolume = 1.0f;
        bMutuallyExclusive = false;
        bSynchronizedFading = false;
    }
};

/**
 * Music layering events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicLayerStateChanged, const FString&, LayerID, EMusicLayerState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicLayerVolumeChanged, const FString&, LayerID, float, NewVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicLayerGroupChanged, const FString&, GroupID);

/**
 * Runtime Music Layering Component - Runtime music layering with fade-in/out and active stem management
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API URuntimeMusicLayeringComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URuntimeMusicLayeringComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize music layering
     * @param MusicSubsystem Procedural music subsystem
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void InitializeMusicLayering(UProceduralMusicSubsystemV2* MusicSubsystem);

    /**
     * Add music layer
     * @param LayerData Layer data to add
     * @return True if layer was added successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    bool AddMusicLayer(const FMusicLayerData& LayerData);

    /**
     * Remove music layer
     * @param LayerID ID of layer to remove
     * @param FadeOutTime Time to fade out before removing
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void RemoveMusicLayer(const FString& LayerID, float FadeOutTime = 2.0f);

    /**
     * Activate music layer
     * @param LayerID ID of layer to activate
     * @param TargetVolume Target volume (0.0 to 1.0)
     * @param FadeInTime Time to fade in
     * @return True if layer was activated
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    bool ActivateMusicLayer(const FString& LayerID, float TargetVolume = 1.0f, float FadeInTime = 2.0f);

    /**
     * Deactivate music layer
     * @param LayerID ID of layer to deactivate
     * @param FadeOutTime Time to fade out
     * @return True if layer was deactivated
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    bool DeactivateMusicLayer(const FString& LayerID, float FadeOutTime = 2.0f);

    /**
     * Set layer volume
     * @param LayerID ID of layer
     * @param Volume Volume to set (0.0 to 1.0)
     * @param BlendTime Time to blend to new volume
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void SetLayerVolume(const FString& LayerID, float Volume, float BlendTime = 1.0f);

    /**
     * Crossfade between layers
     * @param FadeOutLayerID Layer to fade out
     * @param FadeInLayerID Layer to fade in
     * @param CrossfadeTime Duration of crossfade
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void CrossfadeLayers(const FString& FadeOutLayerID, const FString& FadeInLayerID, float CrossfadeTime = 3.0f);

    /**
     * Add layer group
     * @param LayerGroup Layer group to add
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void AddLayerGroup(const FMusicLayerGroup& LayerGroup);

    /**
     * Activate layer group
     * @param GroupID ID of group to activate
     * @param FadeTime Time to fade in group
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void ActivateLayerGroup(const FString& GroupID, float FadeTime = 2.0f);

    /**
     * Deactivate layer group
     * @param GroupID ID of group to deactivate
     * @param FadeTime Time to fade out group
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void DeactivateLayerGroup(const FString& GroupID, float FadeTime = 2.0f);

    /**
     * Set group volume
     * @param GroupID ID of group
     * @param Volume Volume multiplier for group
     * @param BlendTime Time to blend to new volume
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void SetGroupVolume(const FString& GroupID, float Volume, float BlendTime = 1.0f);

    /**
     * Get layer state
     * @param LayerID ID of layer
     * @return Current layer state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Music Layering")
    EMusicLayerState GetLayerState(const FString& LayerID) const;

    /**
     * Get layer volume
     * @param LayerID ID of layer
     * @return Current layer volume
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Music Layering")
    float GetLayerVolume(const FString& LayerID) const;

    /**
     * Get active layers
     * @return Array of currently active layer IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Music Layering")
    TArray<FString> GetActiveLayers() const;

    /**
     * Check if layer exists
     * @param LayerID ID of layer to check
     * @return True if layer exists
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Runtime Music Layering")
    bool HasLayer(const FString& LayerID) const;

    /**
     * Pause all layers
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void PauseAllLayers();

    /**
     * Resume all layers
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void ResumeAllLayers();

    /**
     * Stop all layers
     * @param FadeOutTime Time to fade out all layers
     */
    UFUNCTION(BlueprintCallable, Category = "Runtime Music Layering")
    void StopAllLayers(float FadeOutTime = 3.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Music Layering Events")
    FOnMusicLayerStateChanged OnMusicLayerStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Layering Events")
    FOnMusicLayerVolumeChanged OnMusicLayerVolumeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Music Layering Events")
    FOnMusicLayerGroupChanged OnMusicLayerGroupChanged;

protected:
    // Music layers
    UPROPERTY(BlueprintReadOnly, Category = "Music Layers")
    TMap<FString, FMusicLayerData> MusicLayers;

    // Layer groups
    UPROPERTY(BlueprintReadOnly, Category = "Layer Groups")
    TMap<FString, FMusicLayerGroup> LayerGroups;

    // Component reference
    UPROPERTY()
    UProceduralMusicSubsystemV2* ProceduralMusicSubsystem;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layering Settings")
    float MasterVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layering Settings")
    bool bLayeringEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layering Settings")
    float DefaultFadeTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layering Settings")
    int32 MaxActiveLayers; // Maximum number of active layers

private:
    // Helper methods
    void UpdateLayerFades(float DeltaTime);
    void SetLayerState(const FString& LayerID, EMusicLayerState NewState);
    void UpdateLayerVolume(const FString& LayerID, float NewVolume);
    bool CanActivateLayer(const FString& LayerID) const;
    void HandleMutuallyExclusiveGroups(const FString& LayerID);
    float CalculateEffectiveVolume(const FString& LayerID) const;
    UAudioComponent* CreateAudioComponentForLayer(const FMusicLayerData& LayerData);

public:
    /**
     * Blueprint implementable events for custom music layering logic
     */

    /**
     * Called when layer state changes (for custom processing)
     * @param LayerID ID of layer
     * @param OldState Previous state
     * @param NewState New state
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Layering Events")
    void OnMusicLayerStateChangedEvent(const FString& LayerID, EMusicLayerState OldState, EMusicLayerState NewState);

    /**
     * Called when layer volume changes (for custom effects)
     * @param LayerID ID of layer
     * @param OldVolume Previous volume
     * @param NewVolume New volume
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Layering Events")
    void OnMusicLayerVolumeChangedEvent(const FString& LayerID, float OldVolume, float NewVolume);

    /**
     * Called to customize layer activation (override in Blueprint)
     * @param LayerID ID of layer to activate
     * @param RequestedVolume Requested volume
     * @return True if layer should be activated
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Layering Events")
    bool ShouldActivateLayer(const FString& LayerID, float RequestedVolume);

    /**
     * Called to customize fade timing (override in Blueprint)
     * @param LayerID ID of layer
     * @param DefaultFadeTime Default fade time
     * @param bFadingIn Whether fading in or out
     * @return Custom fade time
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Music Layering Events")
    float CustomizeFadeTime(const FString& LayerID, float DefaultFadeTime, bool bFadingIn);
};
