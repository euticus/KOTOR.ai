// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataAsset.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "ModdingSubsystem.generated.h"

/**
 * Mod types
 */
UENUM(BlueprintType)
enum class EModType : uint8
{
    Content         UMETA(DisplayName = "Content Mod"),
    Gameplay        UMETA(DisplayName = "Gameplay Mod"),
    UI              UMETA(DisplayName = "UI Mod"),
    Audio           UMETA(DisplayName = "Audio Mod"),
    Script          UMETA(DisplayName = "Script Mod"),
    Total           UMETA(DisplayName = "Total Conversion"),
    Patch           UMETA(DisplayName = "Patch/Fix")
};

/**
 * Mod data asset for custom content
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UModDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    FString ModID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    FString ModName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    FString ModVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    FString Author;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    EModType ModType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    TArray<FString> Dependencies; // Other mods this depends on

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod Info")
    TArray<FString> Conflicts; // Mods this conflicts with

    // Content overrides
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    TArray<FEnemyData> CustomEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    TArray<FNPCData> CustomNPCs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    TArray<FLootItem> CustomLoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    TArray<FQuestData> CustomQuests;

    // Asset overrides
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
    TMap<FString, TSoftObjectPtr<UObject>> AssetOverrides;

    // Script hooks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scripting")
    TArray<FString> ScriptFiles; // Paths to script files

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scripting")
    TMap<FString, FString> ScriptHooks; // Event -> Script function

    UModDataAsset()
    {
        ModID = TEXT("");
        ModName = TEXT("Unnamed Mod");
        ModVersion = TEXT("1.0.0");
        Author = TEXT("Unknown");
        Description = TEXT("");
        ModType = EModType::Content;
    }
};

/**
 * Mod information structure
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FModInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    FString ModID;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    FString ModName;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    FString ModVersion;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    FString Author;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    EModType ModType;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    bool bIsEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    bool bIsLoaded;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    FString ModPath;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    TArray<FString> Dependencies;

    UPROPERTY(BlueprintReadWrite, Category = "Mod Info")
    TArray<FString> Conflicts;

    FModInfo()
    {
        ModID = TEXT("");
        ModName = TEXT("");
        ModVersion = TEXT("1.0.0");
        Author = TEXT("");
        Description = TEXT("");
        ModType = EModType::Content;
        bIsEnabled = false;
        bIsLoaded = false;
        ModPath = TEXT("");
    }
};

/**
 * Script execution context
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FScriptContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Script Context")
    FString EventName;

    UPROPERTY(BlueprintReadWrite, Category = "Script Context")
    TMap<FString, FString> Parameters;

    UPROPERTY(BlueprintReadWrite, Category = "Script Context")
    UObject* SourceObject;

    UPROPERTY(BlueprintReadWrite, Category = "Script Context")
    float Timestamp;

    FScriptContext()
    {
        EventName = TEXT("");
        SourceObject = nullptr;
        Timestamp = 0.0f;
    }
};

/**
 * Modding events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModLoaded, const FModInfo&, ModInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModUnloaded, const FModInfo&, ModInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScriptHookExecuted, const FString&, HookName, const FScriptContext&, Context);

/**
 * Modding Subsystem - Manages mod loading, scripting, and content overrides
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UModdingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Scan for available mods
     * @param ModDirectory Directory to scan for mods
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    void ScanForMods(const FString& ModDirectory = TEXT("Mods"));

    /**
     * Load a mod by ID
     * @param ModID ID of the mod to load
     * @return True if mod was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool LoadMod(const FString& ModID);

    /**
     * Unload a mod by ID
     * @param ModID ID of the mod to unload
     * @return True if mod was unloaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool UnloadMod(const FString& ModID);

    /**
     * Enable or disable a mod
     * @param ModID ID of the mod
     * @param bEnable Whether to enable the mod
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    void SetModEnabled(const FString& ModID, bool bEnable);

    /**
     * Get all available mods
     * @return Array of mod information
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Modding")
    TArray<FModInfo> GetAvailableMods() const;

    /**
     * Get loaded mods
     * @return Array of loaded mod information
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Modding")
    TArray<FModInfo> GetLoadedMods() const;

    /**
     * Get mod information by ID
     * @param ModID ID of the mod
     * @return Mod information
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Modding")
    FModInfo GetModInfo(const FString& ModID) const;

    /**
     * Check if mod is loaded
     * @param ModID ID of the mod
     * @return True if mod is loaded
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Modding")
    bool IsModLoaded(const FString& ModID) const;

    /**
     * Execute script hook
     * @param HookName Name of the hook to execute
     * @param Context Script execution context
     * @return True if any scripts were executed
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool ExecuteScriptHook(const FString& HookName, const FScriptContext& Context);

    /**
     * Register script hook
     * @param HookName Name of the hook
     * @param ScriptFunction Function to call
     * @param ModID ID of the mod registering the hook
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    void RegisterScriptHook(const FString& HookName, const FString& ScriptFunction, const FString& ModID);

    /**
     * Get custom content from mods
     * @param ContentType Type of content to get ("enemies", "npcs", "loot", "quests")
     * @return Array of custom content as JSON strings
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    TArray<FString> GetCustomContent(const FString& ContentType) const;

    /**
     * Apply asset overrides from mods
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    void ApplyAssetOverrides();

    /**
     * Validate mod dependencies
     * @param ModID ID of the mod to validate
     * @return True if all dependencies are satisfied
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool ValidateModDependencies(const FString& ModID) const;

    /**
     * Check for mod conflicts
     * @param ModID ID of the mod to check
     * @return Array of conflicting mod IDs
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    TArray<FString> CheckModConflicts(const FString& ModID) const;

    /**
     * Save mod configuration
     * @return True if configuration was saved
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool SaveModConfiguration();

    /**
     * Load mod configuration
     * @return True if configuration was loaded
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool LoadModConfiguration();

    /**
     * Create mod template
     * @param ModName Name of the new mod
     * @param ModType Type of mod to create
     * @param OutputPath Path to create the mod template
     * @return True if template was created successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Modding")
    bool CreateModTemplate(const FString& ModName, EModType ModType, const FString& OutputPath);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Modding Events")
    FOnModLoaded OnModLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Modding Events")
    FOnModUnloaded OnModUnloaded;

    UPROPERTY(BlueprintAssignable, Category = "Modding Events")
    FOnScriptHookExecuted OnScriptHookExecuted;

protected:
    // Mod data
    UPROPERTY()
    TMap<FString, FModInfo> AvailableMods;

    UPROPERTY()
    TMap<FString, UModDataAsset*> LoadedModAssets;

    // Script hooks
    UPROPERTY()
    TMap<FString, TArray<FString>> ScriptHooks; // HookName -> Array of script functions

    // Asset overrides
    UPROPERTY()
    TMap<FString, TSoftObjectPtr<UObject>> ActiveAssetOverrides;

    // Settings
    UPROPERTY()
    FString ModDirectory;

    UPROPERTY()
    TArray<FString> EnabledMods;

private:
    // Helper methods
    FModInfo LoadModInfoFromFile(const FString& ModPath);
    bool LoadModAsset(const FString& ModID);
    void UnloadModAsset(const FString& ModID);
    void ApplyModContent(UModDataAsset* ModAsset);
    void RemoveModContent(UModDataAsset* ModAsset);
    bool ExecuteScript(const FString& ScriptFunction, const FScriptContext& Context);
    FString GenerateModTemplate(const FString& ModName, EModType ModType);
    bool ValidateModFile(const FString& ModPath);

public:
    /**
     * Blueprint implementable events for custom modding logic
     */

    /**
     * Called when a mod is loaded (for custom processing)
     * @param ModInfo Information about the loaded mod
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Modding Events")
    void OnModLoadedEvent(const FModInfo& ModInfo);

    /**
     * Called when a mod is unloaded (for custom cleanup)
     * @param ModInfo Information about the unloaded mod
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Modding Events")
    void OnModUnloadedEvent(const FModInfo& ModInfo);

    /**
     * Called to execute custom script (override in Blueprint)
     * @param ScriptFunction Name of the script function
     * @param Context Script execution context
     * @return True if script was executed successfully
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Modding Events")
    bool ExecuteCustomScript(const FString& ScriptFunction, const FScriptContext& Context);

    /**
     * Called to validate custom mod (override in Blueprint)
     * @param ModPath Path to the mod
     * @return True if mod is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Modding Events")
    bool ValidateCustomMod(const FString& ModPath);

    /**
     * Called when script hook is executed (for custom effects)
     * @param HookName Name of the executed hook
     * @param Context Script execution context
     * @param ExecutedScripts Array of scripts that were executed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Modding Events")
    void OnScriptHookExecutedEvent(const FString& HookName, const FScriptContext& Context, const TArray<FString>& ExecutedScripts);
};
