// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AutoMapLoader.generated.h"

/**
 * Map loading mode
 */
UENUM(BlueprintType)
enum class EMapLoadingMode : uint8
{
    Immediate           UMETA(DisplayName = "Immediate"),
    Delayed             UMETA(DisplayName = "Delayed"),
    OnFirstPlay         UMETA(DisplayName = "On First Play"),
    Conditional         UMETA(DisplayName = "Conditional")
};

/**
 * Map loading configuration
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMapLoadingConfiguration
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    bool bEnableAutoMapLoading; // Whether auto map loading is enabled

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    FString DefaultStartupMap; // Default map to load

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    EMapLoadingMode LoadingMode; // How to load the map

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    float DelayTime; // Delay before loading (for Delayed mode)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    bool bLoadInEditor; // Whether to load map in editor

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    bool bLoadInShipping; // Whether to load map in shipping builds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    TArray<FString> ConditionalMaps; // Maps to choose from conditionally

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading")
    TMap<FString, FString> MapParameters; // Parameters to pass to map

    FMapLoadingConfiguration()
    {
        bEnableAutoMapLoading = true;
        DefaultStartupMap = TEXT("/Game/Maps/TestLevel");
        LoadingMode = EMapLoadingMode::OnFirstPlay;
        DelayTime = 1.0f;
        bLoadInEditor = false;
        bLoadInShipping = true;
    }
};

/**
 * Map loading events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadingStarted, const FString&, MapName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadingCompleted, const FString&, MapName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMapLoadingFailed, const FString&, MapName, const FString&, ErrorMessage);

/**
 * Auto Map Loader - On game startup (non-editor), read DefaultStartupMap.ini and load level
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAutoMapLoader : public UObject
{
    GENERATED_BODY()

public:
    UAutoMapLoader();

    /**
     * Initialize auto map loader
     * @param World World context
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    void InitializeAutoMapLoader(UWorld* World);

    /**
     * Load startup map
     * @param bForceLoad Whether to force load even if conditions aren't met
     * @return True if map loading started
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    bool LoadStartupMap(bool bForceLoad = false);

    /**
     * Load specific map
     * @param MapName Name of map to load
     * @param bAbsolute Whether map name is absolute path
     * @return True if map loading started
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    bool LoadSpecificMap(const FString& MapName, bool bAbsolute = false);

    /**
     * Load configuration from INI file
     * @param ConfigFilePath Path to configuration file
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    bool LoadConfigurationFromINI(const FString& ConfigFilePath);

    /**
     * Save configuration to INI file
     * @param ConfigFilePath Path to save configuration file
     * @return True if saved successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    bool SaveConfigurationToINI(const FString& ConfigFilePath) const;

    /**
     * Set map loading configuration
     * @param Configuration New configuration
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    void SetMapLoadingConfiguration(const FMapLoadingConfiguration& Configuration);

    /**
     * Get map loading configuration
     * @return Current configuration
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Map Loader")
    FMapLoadingConfiguration GetMapLoadingConfiguration() const { return LoadingConfiguration; }

    /**
     * Check if should load map
     * @return True if map should be loaded
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Map Loader")
    bool ShouldLoadMap() const;

    /**
     * Get default startup map
     * @return Default startup map name
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Map Loader")
    FString GetDefaultStartupMap() const { return LoadingConfiguration.DefaultStartupMap; }

    /**
     * Set default startup map
     * @param MapName New default startup map
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    void SetDefaultStartupMap(const FString& MapName);

    /**
     * Check if map loading is in progress
     * @return True if map is currently loading
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Map Loader")
    bool IsMapLoading() const { return bMapLoading; }

    /**
     * Get current map name
     * @return Name of currently loaded map
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Auto Map Loader")
    FString GetCurrentMapName() const;

    /**
     * Reload current map
     * @return True if reload started
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader")
    bool ReloadCurrentMap();

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Map Loading Events")
    FOnMapLoadingStarted OnMapLoadingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Map Loading Events")
    FOnMapLoadingCompleted OnMapLoadingCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Map Loading Events")
    FOnMapLoadingFailed OnMapLoadingFailed;

    // Static helper functions
    /**
     * Get auto map loader instance
     * @param WorldContext World context
     * @return Auto map loader instance
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader", meta = (WorldContext = "WorldContext"))
    static UAutoMapLoader* GetAutoMapLoader(const UObject* WorldContext);

    /**
     * Load map with options
     * @param WorldContext World context
     * @param MapName Map to load
     * @param Options Additional options
     * @return True if loading started
     */
    UFUNCTION(BlueprintCallable, Category = "Auto Map Loader", meta = (WorldContext = "WorldContext"))
    static bool LoadMapWithOptions(const UObject* WorldContext, const FString& MapName, const FString& Options = TEXT(""));

protected:
    // Map loading configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Loading Configuration")
    FMapLoadingConfiguration LoadingConfiguration;

    // World reference
    UPROPERTY()
    UWorld* WorldContext;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Loading State")
    bool bMapLoading;

    UPROPERTY(BlueprintReadOnly, Category = "Loading State")
    FString CurrentlyLoadingMap;

    UPROPERTY(BlueprintReadOnly, Category = "Loading State")
    float LoadingStartTime;

    // Timer handle
    FTimerHandle DelayedLoadingTimer;

private:
    // Helper methods
    void LoadDefaultConfiguration();
    bool ValidateMapPath(const FString& MapPath) const;
    FString ResolveMapPath(const FString& MapName) const;
    void StartMapLoading(const FString& MapName);
    void CompleteMapLoading(const FString& MapName);
    void FailMapLoading(const FString& MapName, const FString& ErrorMessage);
    bool IsInEditor() const;
    bool IsShippingBuild() const;
    FString SelectConditionalMap() const;

    // Timer callback
    UFUNCTION()
    void OnDelayedLoadingTimer();

    // Level streaming events
    UFUNCTION()
    void OnLevelLoaded();

    UFUNCTION()
    void OnLevelLoadFailed();

public:
    /**
     * Blueprint implementable events for custom map loading logic
     */

    /**
     * Called when map loading starts (for custom processing)
     * @param MapName Name of map being loaded
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Map Loading Events")
    void OnMapLoadingStartedEvent(const FString& MapName);

    /**
     * Called when map loading completes (for custom setup)
     * @param MapName Name of loaded map
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Map Loading Events")
    void OnMapLoadingCompletedEvent(const FString& MapName);

    /**
     * Called to customize map selection (override in Blueprint)
     * @param DefaultMap Default map to load
     * @return Custom map selection
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Map Loading Events")
    FString CustomizeMapSelection(const FString& DefaultMap);

    /**
     * Called to validate map loading (override in Blueprint)
     * @param MapName Map to validate
     * @return True if map should be loaded
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Map Loading Events")
    bool ValidateMapLoading(const FString& MapName);
};
