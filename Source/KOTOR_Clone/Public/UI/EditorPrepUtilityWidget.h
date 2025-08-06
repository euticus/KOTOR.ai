// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/EditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "AI/NavMesh/RecastNavMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "EditorPrepUtilityWidget.generated.h"

/**
 * Editor prep action types
 */
UENUM(BlueprintType)
enum class EEditorPrepAction : uint8
{
    SpawnPlayer         UMETA(DisplayName = "Spawn Player"),
    SpawnEnemies        UMETA(DisplayName = "Spawn Enemies"),
    SpawnAIDirector     UMETA(DisplayName = "Spawn AI Director"),
    SetupNavMesh        UMETA(DisplayName = "Setup Nav Mesh"),
    SetupLighting       UMETA(DisplayName = "Setup Lighting"),
    SpawnDebugTools     UMETA(DisplayName = "Spawn Debug Tools"),
    SetupAudio          UMETA(DisplayName = "Setup Audio"),
    SetupCameras        UMETA(DisplayName = "Setup Cameras"),
    ClearLevel          UMETA(DisplayName = "Clear Level"),
    SavePreset          UMETA(DisplayName = "Save Preset")
};

/**
 * Spawn preset data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSpawnPresetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    FString PresetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    TSoftClassPtr<APawn> PlayerCharacterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    FVector PlayerSpawnLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    TArray<TSoftClassPtr<AActor>> EnemyClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    TArray<FVector> EnemySpawnLocations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    bool bSpawnAIDirector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    bool bSetupNavMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    bool bSetupLighting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Preset")
    bool bSpawnDebugTools;

    FSpawnPresetData()
    {
        PresetName = TEXT("Default");
        PlayerCharacterClass = nullptr;
        PlayerSpawnLocation = FVector(0, 0, 100);
        bSpawnAIDirector = true;
        bSetupNavMesh = true;
        bSetupLighting = true;
        bSpawnDebugTools = true;
    }
};

/**
 * Editor prep events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEditorPrepActionCompleted, EEditorPrepAction, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSpawned, AActor*, SpawnedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPresetLoaded, const FString&, PresetName);

/**
 * Editor Prep Utility Widget - Buttons for spawning BP_PlayerCharacter, enemies, AI Director HUD, nav mesh, lighting
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UEditorPrepUtilityWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UEditorPrepUtilityWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    /**
     * Initialize editor prep utility
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    void InitializeEditorPrepUtility();

    /**
     * Spawn player character
     * @param SpawnLocation Location to spawn player
     * @return Spawned player actor
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    APawn* SpawnPlayerCharacter(const FVector& SpawnLocation = FVector(0, 0, 100));

    /**
     * Spawn enemies
     * @param EnemyCount Number of enemies to spawn
     * @param SpawnRadius Radius around player to spawn enemies
     * @return Array of spawned enemy actors
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    TArray<AActor*> SpawnEnemies(int32 EnemyCount = 3, float SpawnRadius = 500.0f);

    /**
     * Spawn AI Director HUD
     * @return Spawned AI Director actor
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    AActor* SpawnAIDirectorHUD();

    /**
     * Setup nav mesh
     * @param NavMeshBounds Bounds for nav mesh generation
     * @return True if nav mesh was setup successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    bool SetupNavMesh(const FVector& NavMeshBounds = FVector(2000, 2000, 500));

    /**
     * Setup lighting
     * @param bCreateDirectionalLight Whether to create directional light
     * @param bCreateSkyLight Whether to create sky light
     * @return True if lighting was setup successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    bool SetupLighting(bool bCreateDirectionalLight = true, bool bCreateSkyLight = true);

    /**
     * Spawn debug tools
     * @return Array of spawned debug tool actors
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    TArray<AActor*> SpawnDebugTools();

    /**
     * Setup audio
     * @return True if audio was setup successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    bool SetupAudio();

    /**
     * Setup cameras
     * @return Array of spawned camera actors
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    TArray<AActor*> SetupCameras();

    /**
     * Clear level of spawned actors
     * @param bClearAll Whether to clear all actors or just spawned ones
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    void ClearLevel(bool bClearAll = false);

    /**
     * Load spawn preset
     * @param PresetName Name of preset to load
     * @return True if preset was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    bool LoadSpawnPreset(const FString& PresetName);

    /**
     * Save spawn preset
     * @param PresetName Name of preset to save
     * @return True if preset was saved successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    bool SaveSpawnPreset(const FString& PresetName);

    /**
     * Get available presets
     * @return Array of available preset names
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Editor Prep Utility")
    TArray<FString> GetAvailablePresets() const;

    /**
     * Execute prep action
     * @param Action Action to execute
     * @return True if action was executed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Editor Prep Utility")
    bool ExecutePrepAction(EEditorPrepAction Action);

    /**
     * Get spawned actors by type
     * @param ActorClass Class of actors to get
     * @return Array of spawned actors
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Editor Prep Utility")
    TArray<AActor*> GetSpawnedActorsByClass(UClass* ActorClass) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Editor Prep Events")
    FOnEditorPrepActionCompleted OnEditorPrepActionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Editor Prep Events")
    FOnActorSpawned OnActorSpawned;

    UPROPERTY(BlueprintAssignable, Category = "Editor Prep Events")
    FOnPresetLoaded OnPresetLoaded;

protected:
    // UI Components - Main Actions
    UPROPERTY(meta = (BindWidget))
    UButton* SpawnPlayerButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SpawnEnemiesButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SpawnAIDirectorButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SetupNavMeshButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SetupLightingButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SpawnDebugToolsButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SetupAudioButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SetupCamerasButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ClearLevelButton;

    // UI Components - Presets
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* PresetComboBox;

    UPROPERTY(meta = (BindWidget))
    UButton* LoadPresetButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SavePresetButton;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* PresetNameTextBox;

    // UI Components - Settings
    UPROPERTY(meta = (BindWidget))
    USlider* EnemyCountSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* SpawnRadiusSlider;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* AutoSetupCheckBox;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* ClearBeforeSpawnCheckBox;

    // UI Components - Status
    UPROPERTY(meta = (BindWidget))
    UTextBlock* StatusText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ActionProgressBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SpawnedActorsCountText;

    // Spawn presets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Presets")
    TMap<FString, FSpawnPresetData> SpawnPresets;

    // Spawned actors tracking
    UPROPERTY(BlueprintReadOnly, Category = "Spawned Actors")
    TArray<AActor*> SpawnedActors;

    // Actor classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Classes")
    TSoftClassPtr<APawn> DefaultPlayerCharacterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Classes")
    TArray<TSoftClassPtr<AActor>> DefaultEnemyClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Classes")
    TSoftClassPtr<AActor> AIDirectorHUDClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Classes")
    TArray<TSoftClassPtr<AActor>> DebugToolClasses;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prep Settings")
    bool bAutoSetup; // Whether to auto-setup when spawning

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prep Settings")
    bool bClearBeforeSpawn; // Whether to clear before spawning

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prep Settings")
    float DefaultSpawnHeight; // Default height for spawning

private:
    // Helper methods
    void LoadDefaultPresets();
    void UpdateUI();
    void UpdateSpawnedActorsCount();
    FVector GetRandomSpawnLocation(const FVector& Center, float Radius) const;
    bool IsValidSpawnLocation(const FVector& Location) const;
    void TrackSpawnedActor(AActor* Actor);

    // UI event handlers
    UFUNCTION()
    void OnSpawnPlayerButtonClicked();

    UFUNCTION()
    void OnSpawnEnemiesButtonClicked();

    UFUNCTION()
    void OnSpawnAIDirectorButtonClicked();

    UFUNCTION()
    void OnSetupNavMeshButtonClicked();

    UFUNCTION()
    void OnSetupLightingButtonClicked();

    UFUNCTION()
    void OnSpawnDebugToolsButtonClicked();

    UFUNCTION()
    void OnSetupAudioButtonClicked();

    UFUNCTION()
    void OnSetupCamerasButtonClicked();

    UFUNCTION()
    void OnClearLevelButtonClicked();

    UFUNCTION()
    void OnLoadPresetButtonClicked();

    UFUNCTION()
    void OnSavePresetButtonClicked();

    UFUNCTION()
    void OnPresetComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnEnemyCountSliderChanged(float Value);

    UFUNCTION()
    void OnSpawnRadiusSliderChanged(float Value);

    UFUNCTION()
    void OnAutoSetupCheckBoxChanged(bool bIsChecked);

    UFUNCTION()
    void OnClearBeforeSpawnCheckBoxChanged(bool bIsChecked);

public:
    /**
     * Blueprint implementable events for custom editor prep logic
     */

    /**
     * Called when prep action is completed (for custom processing)
     * @param Action The completed action
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Editor Prep Events")
    void OnEditorPrepActionCompletedEvent(EEditorPrepAction Action);

    /**
     * Called when actor is spawned (for custom setup)
     * @param SpawnedActor The spawned actor
     * @param ActorType Type of actor spawned
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Editor Prep Events")
    void OnActorSpawnedEvent(AActor* SpawnedActor, const FString& ActorType);

    /**
     * Called to customize actor spawn (override in Blueprint)
     * @param ActorClass Class of actor to spawn
     * @param SpawnLocation Location to spawn actor
     * @return Customized spawn parameters
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Editor Prep Events")
    FVector CustomizeActorSpawn(UClass* ActorClass, const FVector& SpawnLocation);

    /**
     * Called to validate spawn location (override in Blueprint)
     * @param Location Location to validate
     * @param ActorClass Class of actor being spawned
     * @return True if location is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Editor Prep Events")
    bool ValidateSpawnLocation(const FVector& Location, UClass* ActorClass);
};
