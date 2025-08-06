// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Loaders/CampaignJSONLoader.h"
#include "KOTORGameInstance.generated.h"

/**
 * Auto-load configuration
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAutoLoadConfig
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    bool bAutoLoadEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    FString DefaultCampaignFile;

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    FString StartPlanet;

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    bool bSkipIntro;

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    bool bEnableDevMode;

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    bool bAutoGenerateTestCampaign;

    UPROPERTY(BlueprintReadWrite, Category = "Auto Load")
    TArray<FString> TestCampaignPlanets;

    FAutoLoadConfig()
    {
        bAutoLoadEnabled = false;
        DefaultCampaignFile = TEXT("TestCampaign.json");
        StartPlanet = TEXT("Taris");
        bSkipIntro = true;
        bEnableDevMode = false;
        bAutoGenerateTestCampaign = true;
        TestCampaignPlanets = {TEXT("Taris"), TEXT("Dantooine"), TEXT("Korriban")};
    }
};

/**
 * Game instance events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCampaignAutoLoaded, const FCampaignData&, CampaignData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoLoadFailed, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDevModeEnabled);

/**
 * KOTOR Game Instance - Enhanced game instance with auto-loading capabilities
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UKOTORGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UKOTORGameInstance();

protected:
    virtual void Init() override;
    virtual void Shutdown() override;

public:
    /**
     * Set auto-load configuration
     * @param Config New auto-load configuration
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    void SetAutoLoadConfig(const FAutoLoadConfig& Config);

    /**
     * Get auto-load configuration
     * @return Current auto-load configuration
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Game Instance")
    FAutoLoadConfig GetAutoLoadConfig() const { return AutoLoadConfig; }

    /**
     * Enable auto-load
     * @param bEnabled Whether to enable auto-load
     * @param CampaignFile Campaign file to auto-load
     * @param StartPlanet Planet to start on
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    void EnableAutoLoad(bool bEnabled, const FString& CampaignFile = TEXT(""), const FString& StartPlanet = TEXT(""));

    /**
     * Load campaign now
     * @param CampaignFile Campaign file to load
     * @return True if load was initiated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    bool LoadCampaignNow(const FString& CampaignFile);

    /**
     * Generate test campaign
     * @param CampaignName Name for the test campaign
     * @param PlanetNames Planets to include in test campaign
     * @return True if generation was successful
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    bool GenerateTestCampaign(const FString& CampaignName, const TArray<FString>& PlanetNames);

    /**
     * Enable development mode
     * @param bEnabled Whether to enable dev mode
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    void EnableDevelopmentMode(bool bEnabled);

    /**
     * Check if development mode is enabled
     * @return True if dev mode is enabled
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Game Instance")
    bool IsDevelopmentModeEnabled() const { return bDevelopmentModeEnabled; }

    /**
     * Get loaded campaign data
     * @return Currently loaded campaign data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Game Instance")
    FCampaignData GetLoadedCampaignData() const { return LoadedCampaignData; }

    /**
     * Check if campaign is loaded
     * @return True if a campaign is currently loaded
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Game Instance")
    bool IsCampaignLoaded() const { return bCampaignLoaded; }

    /**
     * Parse command line arguments
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    void ParseCommandLineArguments();

    /**
     * Apply command line overrides
     * @param CommandLine Command line string to parse
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Game Instance")
    void ApplyCommandLineOverrides(const FString& CommandLine);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Game Instance Events")
    FOnCampaignAutoLoaded OnCampaignAutoLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Game Instance Events")
    FOnAutoLoadFailed OnAutoLoadFailed;

    UPROPERTY(BlueprintAssignable, Category = "Game Instance Events")
    FOnDevModeEnabled OnDevModeEnabled;

protected:
    // Auto-load configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Load Settings")
    FAutoLoadConfig AutoLoadConfig;

    // Campaign data
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Data")
    FCampaignData LoadedCampaignData;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Data")
    bool bCampaignLoaded;

    // Development settings
    UPROPERTY(BlueprintReadOnly, Category = "Development")
    bool bDevelopmentModeEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Development")
    bool bEnableAutoLoadInEditor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Development")
    bool bEnableAutoLoadInGame;

    // Subsystem references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    UPROPERTY()
    UCampaignJSONLoader* JSONLoader;

    // Timer handles
    FTimerHandle AutoLoadTimer;

private:
    // Helper methods
    void InitializeSubsystems();
    void SetupAutoLoad();
    void PerformAutoLoad();
    void OnCampaignLoadCompleted(const FCampaignData& CampaignData, EJSONParseResult Result);
    void OnCampaignLoadFailed(const FString& FilePath, const FString& ErrorMessage);
    FString GetCommandLineValue(const FString& CommandLine, const FString& Parameter);
    bool HasCommandLineFlag(const FString& CommandLine, const FString& Flag);

    // Auto-load timer callback
    UFUNCTION()
    void OnAutoLoadTimer();

public:
    /**
     * Blueprint implementable events for custom game instance logic
     */

    /**
     * Called when campaign is auto-loaded (for custom setup)
     * @param CampaignData The loaded campaign data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Instance Events")
    void OnCampaignAutoLoadedEvent(const FCampaignData& CampaignData);

    /**
     * Called when auto-load fails (for custom error handling)
     * @param ErrorMessage Error message from auto-load failure
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Instance Events")
    void OnAutoLoadFailedEvent(const FString& ErrorMessage);

    /**
     * Called when development mode is enabled (for custom dev setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Instance Events")
    void OnDevModeEnabledEvent();

    /**
     * Called to generate custom test campaign (override in Blueprint)
     * @param CampaignName Name for the campaign
     * @param PlanetNames Planets to include
     * @return Custom generated campaign data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Instance Events")
    FCampaignData GenerateCustomTestCampaign(const FString& CampaignName, const TArray<FString>& PlanetNames);

    /**
     * Called to customize auto-load behavior (override in Blueprint)
     * @param Config Current auto-load configuration
     * @return Modified auto-load configuration
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Instance Events")
    FAutoLoadConfig CustomizeAutoLoadConfig(const FAutoLoadConfig& Config);
};
