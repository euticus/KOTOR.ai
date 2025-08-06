// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/AutoMapLoader.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"

UAutoMapLoader::UAutoMapLoader()
{
    WorldContext = nullptr;
    bMapLoading = false;
    CurrentlyLoadingMap = TEXT("");
    LoadingStartTime = 0.0f;
    
    LoadDefaultConfiguration();
}

void UAutoMapLoader::InitializeAutoMapLoader(UWorld* World)
{
    WorldContext = World;
    
    // Load configuration from default INI file
    FString DefaultConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultStartupMap.ini");
    LoadConfigurationFromINI(DefaultConfigPath);
    
    // Auto-load map if conditions are met
    if (ShouldLoadMap())
    {
        if (LoadingConfiguration.LoadingMode == EMapLoadingMode::Immediate)
        {
            LoadStartupMap();
        }
        else if (LoadingConfiguration.LoadingMode == EMapLoadingMode::Delayed)
        {
            // Set timer for delayed loading
            if (WorldContext)
            {
                WorldContext->GetTimerManager().SetTimer(
                    DelayedLoadingTimer,
                    this,
                    &UAutoMapLoader::OnDelayedLoadingTimer,
                    LoadingConfiguration.DelayTime,
                    false
                );
            }
        }
        else if (LoadingConfiguration.LoadingMode == EMapLoadingMode::OnFirstPlay)
        {
            // Load on first play (immediate for now, can be customized)
            LoadStartupMap();
        }
    }
}

bool UAutoMapLoader::LoadStartupMap(bool bForceLoad)
{
    if (!bForceLoad && !ShouldLoadMap())
    {
        return false;
    }
    
    FString MapToLoad = LoadingConfiguration.DefaultStartupMap;
    
    // Handle conditional map selection
    if (LoadingConfiguration.LoadingMode == EMapLoadingMode::Conditional && 
        LoadingConfiguration.ConditionalMaps.Num() > 0)
    {
        MapToLoad = SelectConditionalMap();
    }
    
    // Allow Blueprint customization
    MapToLoad = CustomizeMapSelection(MapToLoad);
    
    // Validate map before loading
    if (!ValidateMapLoading(MapToLoad))
    {
        FailMapLoading(MapToLoad, TEXT("Map validation failed"));
        return false;
    }
    
    return LoadSpecificMap(MapToLoad, false);
}

bool UAutoMapLoader::LoadSpecificMap(const FString& MapName, bool bAbsolute)
{
    if (bMapLoading)
    {
        UE_LOG(LogTemp, Warning, TEXT("AutoMapLoader: Map loading already in progress"));
        return false;
    }
    
    if (!WorldContext)
    {
        UE_LOG(LogTemp, Error, TEXT("AutoMapLoader: No world context available"));
        return false;
    }
    
    FString ResolvedMapPath = bAbsolute ? MapName : ResolveMapPath(MapName);
    
    if (!ValidateMapPath(ResolvedMapPath))
    {
        FailMapLoading(MapName, TEXT("Invalid map path"));
        return false;
    }
    
    StartMapLoading(MapName);
    
    // Build options string
    FString Options = TEXT("");
    for (const auto& Param : LoadingConfiguration.MapParameters)
    {
        if (!Options.IsEmpty())
        {
            Options += TEXT("?");
        }
        Options += FString::Printf(TEXT("%s=%s"), *Param.Key, *Param.Value);
    }
    
    // Load the map
    UGameplayStatics::OpenLevel(WorldContext, FName(*ResolvedMapPath), true, Options);
    
    return true;
}

bool UAutoMapLoader::LoadConfigurationFromINI(const FString& ConfigFilePath)
{
    if (!FPaths::FileExists(ConfigFilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("AutoMapLoader: Configuration file not found: %s"), *ConfigFilePath);
        return false;
    }
    
    FConfigFile ConfigFile;
    ConfigFile.Read(ConfigFilePath);
    
    // Load configuration values
    FString SectionName = TEXT("MapLoading");
    
    ConfigFile.GetBool(*SectionName, TEXT("bEnableAutoMapLoading"), LoadingConfiguration.bEnableAutoMapLoading);
    ConfigFile.GetString(*SectionName, TEXT("DefaultStartupMap"), LoadingConfiguration.DefaultStartupMap);
    ConfigFile.GetFloat(*SectionName, TEXT("DelayTime"), LoadingConfiguration.DelayTime);
    ConfigFile.GetBool(*SectionName, TEXT("bLoadInEditor"), LoadingConfiguration.bLoadInEditor);
    ConfigFile.GetBool(*SectionName, TEXT("bLoadInShipping"), LoadingConfiguration.bLoadInShipping);
    
    // Load loading mode
    FString LoadingModeString;
    if (ConfigFile.GetString(*SectionName, TEXT("LoadingMode"), LoadingModeString))
    {
        if (LoadingModeString == TEXT("Immediate"))
        {
            LoadingConfiguration.LoadingMode = EMapLoadingMode::Immediate;
        }
        else if (LoadingModeString == TEXT("Delayed"))
        {
            LoadingConfiguration.LoadingMode = EMapLoadingMode::Delayed;
        }
        else if (LoadingModeString == TEXT("OnFirstPlay"))
        {
            LoadingConfiguration.LoadingMode = EMapLoadingMode::OnFirstPlay;
        }
        else if (LoadingModeString == TEXT("Conditional"))
        {
            LoadingConfiguration.LoadingMode = EMapLoadingMode::Conditional;
        }
    }
    
    // Load conditional maps
    TArray<FString> ConditionalMaps;
    ConfigFile.GetArray(*SectionName, TEXT("ConditionalMaps"), ConditionalMaps);
    LoadingConfiguration.ConditionalMaps = ConditionalMaps;
    
    UE_LOG(LogTemp, Log, TEXT("AutoMapLoader: Configuration loaded from %s"), *ConfigFilePath);
    return true;
}

bool UAutoMapLoader::SaveConfigurationToINI(const FString& ConfigFilePath) const
{
    FConfigFile ConfigFile;
    FString SectionName = TEXT("MapLoading");
    
    // Save configuration values
    ConfigFile.SetBool(*SectionName, TEXT("bEnableAutoMapLoading"), LoadingConfiguration.bEnableAutoMapLoading);
    ConfigFile.SetString(*SectionName, TEXT("DefaultStartupMap"), *LoadingConfiguration.DefaultStartupMap);
    ConfigFile.SetFloat(*SectionName, TEXT("DelayTime"), LoadingConfiguration.DelayTime);
    ConfigFile.SetBool(*SectionName, TEXT("bLoadInEditor"), LoadingConfiguration.bLoadInEditor);
    ConfigFile.SetBool(*SectionName, TEXT("bLoadInShipping"), LoadingConfiguration.bLoadInShipping);
    
    // Save loading mode
    FString LoadingModeString;
    switch (LoadingConfiguration.LoadingMode)
    {
        case EMapLoadingMode::Immediate:
            LoadingModeString = TEXT("Immediate");
            break;
        case EMapLoadingMode::Delayed:
            LoadingModeString = TEXT("Delayed");
            break;
        case EMapLoadingMode::OnFirstPlay:
            LoadingModeString = TEXT("OnFirstPlay");
            break;
        case EMapLoadingMode::Conditional:
            LoadingModeString = TEXT("Conditional");
            break;
    }
    ConfigFile.SetString(*SectionName, TEXT("LoadingMode"), *LoadingModeString);
    
    // Save conditional maps
    ConfigFile.SetArray(*SectionName, TEXT("ConditionalMaps"), LoadingConfiguration.ConditionalMaps);
    
    // Write to file
    bool bSuccess = ConfigFile.Write(ConfigFilePath);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("AutoMapLoader: Configuration saved to %s"), *ConfigFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AutoMapLoader: Failed to save configuration to %s"), *ConfigFilePath);
    }
    
    return bSuccess;
}

void UAutoMapLoader::SetMapLoadingConfiguration(const FMapLoadingConfiguration& Configuration)
{
    LoadingConfiguration = Configuration;
}

bool UAutoMapLoader::ShouldLoadMap() const
{
    if (!LoadingConfiguration.bEnableAutoMapLoading)
    {
        return false;
    }
    
    // Check editor condition
    if (IsInEditor() && !LoadingConfiguration.bLoadInEditor)
    {
        return false;
    }
    
    // Check shipping build condition
    if (IsShippingBuild() && !LoadingConfiguration.bLoadInShipping)
    {
        return false;
    }
    
    return true;
}

void UAutoMapLoader::SetDefaultStartupMap(const FString& MapName)
{
    LoadingConfiguration.DefaultStartupMap = MapName;
}

FString UAutoMapLoader::GetCurrentMapName() const
{
    if (WorldContext)
    {
        return WorldContext->GetMapName();
    }
    return TEXT("");
}

bool UAutoMapLoader::ReloadCurrentMap()
{
    FString CurrentMap = GetCurrentMapName();
    if (CurrentMap.IsEmpty())
    {
        return false;
    }
    
    return LoadSpecificMap(CurrentMap, true);
}

UAutoMapLoader* UAutoMapLoader::GetAutoMapLoader(const UObject* WorldContext)
{
    if (!WorldContext)
    {
        return nullptr;
    }
    
    UWorld* World = WorldContext->GetWorld();
    if (!World)
    {
        return nullptr;
    }
    
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }
    
    // Try to find existing auto map loader
    // For now, create a new one (in a real implementation, this would be cached)
    UAutoMapLoader* AutoMapLoader = NewObject<UAutoMapLoader>(GameInstance);
    AutoMapLoader->InitializeAutoMapLoader(World);
    
    return AutoMapLoader;
}

bool UAutoMapLoader::LoadMapWithOptions(const UObject* WorldContext, const FString& MapName, const FString& Options)
{
    if (!WorldContext)
    {
        return false;
    }
    
    UWorld* World = WorldContext->GetWorld();
    if (!World)
    {
        return false;
    }
    
    UGameplayStatics::OpenLevel(World, FName(*MapName), true, Options);
    return true;
}

void UAutoMapLoader::LoadDefaultConfiguration()
{
    // Set default values
    LoadingConfiguration = FMapLoadingConfiguration();
}

bool UAutoMapLoader::ValidateMapPath(const FString& MapPath) const
{
    // Basic validation - check if path is not empty and has proper format
    if (MapPath.IsEmpty())
    {
        return false;
    }
    
    // Check if it's a valid Unreal map path
    if (!MapPath.StartsWith(TEXT("/Game/")) && !MapPath.StartsWith(TEXT("/Engine/")))
    {
        return false;
    }
    
    return true;
}

FString UAutoMapLoader::ResolveMapPath(const FString& MapName) const
{
    // If already a full path, return as-is
    if (MapName.StartsWith(TEXT("/Game/")) || MapName.StartsWith(TEXT("/Engine/")))
    {
        return MapName;
    }
    
    // Assume it's a map in the Maps folder
    return FString::Printf(TEXT("/Game/Maps/%s"), *MapName);
}

void UAutoMapLoader::StartMapLoading(const FString& MapName)
{
    bMapLoading = true;
    CurrentlyLoadingMap = MapName;
    LoadingStartTime = FPlatformTime::Seconds();
    
    OnMapLoadingStarted.Broadcast(MapName);
    OnMapLoadingStartedEvent(MapName);
    
    UE_LOG(LogTemp, Log, TEXT("AutoMapLoader: Started loading map: %s"), *MapName);
}

void UAutoMapLoader::CompleteMapLoading(const FString& MapName)
{
    bMapLoading = false;
    CurrentlyLoadingMap = TEXT("");
    
    float LoadingTime = FPlatformTime::Seconds() - LoadingStartTime;
    
    OnMapLoadingCompleted.Broadcast(MapName);
    OnMapLoadingCompletedEvent(MapName);
    
    UE_LOG(LogTemp, Log, TEXT("AutoMapLoader: Completed loading map: %s (%.2fs)"), *MapName, LoadingTime);
}

void UAutoMapLoader::FailMapLoading(const FString& MapName, const FString& ErrorMessage)
{
    bMapLoading = false;
    CurrentlyLoadingMap = TEXT("");
    
    OnMapLoadingFailed.Broadcast(MapName, ErrorMessage);
    
    UE_LOG(LogTemp, Error, TEXT("AutoMapLoader: Failed to load map: %s - %s"), *MapName, *ErrorMessage);
}

bool UAutoMapLoader::IsInEditor() const
{
#if WITH_EDITOR
    return true;
#else
    return false;
#endif
}

bool UAutoMapLoader::IsShippingBuild() const
{
#if UE_BUILD_SHIPPING
    return true;
#else
    return false;
#endif
}

FString UAutoMapLoader::SelectConditionalMap() const
{
    if (LoadingConfiguration.ConditionalMaps.Num() == 0)
    {
        return LoadingConfiguration.DefaultStartupMap;
    }
    
    // Simple selection - could be enhanced with more logic
    int32 SelectedIndex = FMath::RandRange(0, LoadingConfiguration.ConditionalMaps.Num() - 1);
    return LoadingConfiguration.ConditionalMaps[SelectedIndex];
}

void UAutoMapLoader::OnDelayedLoadingTimer()
{
    LoadStartupMap();
}

void UAutoMapLoader::OnLevelLoaded()
{
    if (bMapLoading)
    {
        CompleteMapLoading(CurrentlyLoadingMap);
    }
}

void UAutoMapLoader::OnLevelLoadFailed()
{
    if (bMapLoading)
    {
        FailMapLoading(CurrentlyLoadingMap, TEXT("Level loading failed"));
    }
}
