// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Dom/JsonObject.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "CampaignJSONLoader.generated.h"

/**
 * JSON parsing result
 */
UENUM(BlueprintType)
enum class EJSONParseResult : uint8
{
    Success             UMETA(DisplayName = "Success"),
    FileNotFound        UMETA(DisplayName = "File Not Found"),
    InvalidJSON         UMETA(DisplayName = "Invalid JSON"),
    MissingRequiredData UMETA(DisplayName = "Missing Required Data"),
    VersionMismatch     UMETA(DisplayName = "Version Mismatch"),
    CorruptedData       UMETA(DisplayName = "Corrupted Data")
};

/**
 * Campaign file metadata
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCampaignFileMetadata
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString FileName;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString FilePath;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString CampaignID;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString CampaignName;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString Version;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString Author;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FDateTime CreatedDate;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FDateTime ModifiedDate;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    int64 FileSize; // File size in bytes

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Campaign Metadata")
    TArray<FString> Tags;

    FCampaignFileMetadata()
    {
        FileName = TEXT("");
        FilePath = TEXT("");
        CampaignID = TEXT("");
        CampaignName = TEXT("Unknown Campaign");
        Version = TEXT("1.0");
        Author = TEXT("Unknown");
        FileSize = 0;
        Description = TEXT("");
    }
};

/**
 * JSON validation result
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FJSONValidationResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "JSON Validation")
    bool bIsValid;

    UPROPERTY(BlueprintReadWrite, Category = "JSON Validation")
    TArray<FString> Errors;

    UPROPERTY(BlueprintReadWrite, Category = "JSON Validation")
    TArray<FString> Warnings;

    UPROPERTY(BlueprintReadWrite, Category = "JSON Validation")
    FString ValidationSummary;

    FJSONValidationResult()
    {
        bIsValid = false;
        ValidationSummary = TEXT("");
    }
};

/**
 * Campaign loading events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCampaignLoadStarted, const FString&, FilePath, const FString&, CampaignName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCampaignLoadCompleted, const FCampaignData&, CampaignData, EJSONParseResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCampaignLoadFailed, const FString&, FilePath, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCampaignListRefreshed, const TArray<FCampaignFileMetadata>&, AvailableCampaigns);

/**
 * Campaign JSON Loader - Loads AIDM campaigns from JSON files
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UCampaignJSONLoader : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Load campaign from JSON file
     * @param FilePath Path to the JSON campaign file
     * @param bValidateData Whether to validate the loaded data
     * @return Parse result and campaign data
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    EJSONParseResult LoadCampaignFromFile(const FString& FilePath, bool bValidateData = true);

    /**
     * Load campaign from JSON string
     * @param JSONString JSON string containing campaign data
     * @param bValidateData Whether to validate the loaded data
     * @return Parse result and campaign data
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    EJSONParseResult LoadCampaignFromString(const FString& JSONString, bool bValidateData = true);

    /**
     * Save campaign to JSON file
     * @param CampaignData Campaign data to save
     * @param FilePath Path where to save the file
     * @param bPrettyPrint Whether to format JSON for readability
     * @return True if save was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    bool SaveCampaignToFile(const FCampaignData& CampaignData, const FString& FilePath, bool bPrettyPrint = true);

    /**
     * Convert campaign to JSON string
     * @param CampaignData Campaign data to convert
     * @param bPrettyPrint Whether to format JSON for readability
     * @return JSON string representation
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    FString ConvertCampaignToJSON(const FCampaignData& CampaignData, bool bPrettyPrint = true);

    /**
     * Validate JSON campaign data
     * @param JSONObject JSON object to validate
     * @return Validation result
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    FJSONValidationResult ValidateJSONCampaign(const FString& JSONString);

    /**
     * Get campaign metadata from file
     * @param FilePath Path to the campaign file
     * @return Campaign metadata
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    FCampaignFileMetadata GetCampaignMetadata(const FString& FilePath);

    /**
     * Scan directory for campaign files
     * @param DirectoryPath Directory to scan
     * @param bRecursive Whether to scan subdirectories
     * @return Array of found campaign files
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    TArray<FCampaignFileMetadata> ScanForCampaignFiles(const FString& DirectoryPath, bool bRecursive = true);

    /**
     * Get available campaigns
     * @return Array of available campaign metadata
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign JSON Loader")
    TArray<FCampaignFileMetadata> GetAvailableCampaigns() const { return AvailableCampaigns; }

    /**
     * Refresh campaign list
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    void RefreshCampaignList();

    /**
     * Get last loaded campaign
     * @return Last successfully loaded campaign data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign JSON Loader")
    FCampaignData GetLastLoadedCampaign() const { return LastLoadedCampaign; }

    /**
     * Check if campaign file exists
     * @param FilePath Path to check
     * @return True if file exists and is readable
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign JSON Loader")
    bool DoesCampaignFileExist(const FString& FilePath) const;

    /**
     * Get default campaign directory
     * @return Default directory for campaign files
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign JSON Loader")
    FString GetDefaultCampaignDirectory() const;

    /**
     * Set campaign directory
     * @param NewDirectory New directory for campaigns
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign JSON Loader")
    void SetCampaignDirectory(const FString& NewDirectory);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Campaign Loading Events")
    FOnCampaignLoadStarted OnCampaignLoadStarted;

    UPROPERTY(BlueprintAssignable, Category = "Campaign Loading Events")
    FOnCampaignLoadCompleted OnCampaignLoadCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Campaign Loading Events")
    FOnCampaignLoadFailed OnCampaignLoadFailed;

    UPROPERTY(BlueprintAssignable, Category = "Campaign Loading Events")
    FOnCampaignListRefreshed OnCampaignListRefreshed;

protected:
    // Campaign data
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Data")
    FCampaignData LastLoadedCampaign;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Data")
    TArray<FCampaignFileMetadata> AvailableCampaigns;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loader Settings")
    FString CampaignDirectory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loader Settings")
    TArray<FString> SupportedFileExtensions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loader Settings")
    bool bAutoRefreshOnStartup;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loader Settings")
    float RefreshInterval; // Seconds between automatic refreshes

    // Timer handle
    FTimerHandle RefreshTimer;

private:
    // JSON parsing methods
    TSharedPtr<FJsonObject> ParseJSONString(const FString& JSONString);
    FCampaignData ParseCampaignFromJSON(TSharedPtr<FJsonObject> JSONObject);
    TSharedPtr<FJsonObject> ConvertCampaignToJSONObject(const FCampaignData& CampaignData);
    
    // Data parsing helpers
    FPlanetData ParsePlanetFromJSON(TSharedPtr<FJsonObject> PlanetObject);
    FNPCData ParseNPCFromJSON(TSharedPtr<FJsonObject> NPCObject);
    FQuestData ParseQuestFromJSON(TSharedPtr<FJsonObject> QuestObject);
    FLootData ParseLootFromJSON(TSharedPtr<FJsonObject> LootObject);
    
    // Data conversion helpers
    TSharedPtr<FJsonObject> ConvertPlanetToJSON(const FPlanetData& PlanetData);
    TSharedPtr<FJsonObject> ConvertNPCToJSON(const FNPCData& NPCData);
    TSharedPtr<FJsonObject> ConvertQuestToJSON(const FQuestData& QuestData);
    TSharedPtr<FJsonObject> ConvertLootToJSON(const FLootData& LootData);
    
    // Validation helpers
    bool ValidateRequiredFields(TSharedPtr<FJsonObject> JSONObject, TArray<FString>& Errors);
    bool ValidatePlanetData(TSharedPtr<FJsonObject> PlanetObject, TArray<FString>& Errors);
    bool ValidateNPCData(TSharedPtr<FJsonObject> NPCObject, TArray<FString>& Errors);
    bool ValidateQuestData(TSharedPtr<FJsonObject> QuestObject, TArray<FString>& Errors);
    
    // File operations
    FString LoadFileToString(const FString& FilePath);
    bool SaveStringToFile(const FString& Content, const FString& FilePath);
    FCampaignFileMetadata ExtractMetadataFromFile(const FString& FilePath);
    
    // Utility methods
    void LoadDefaultSettings();
    void StartAutoRefresh();
    void StopAutoRefresh();

    // Timer callback
    UFUNCTION()
    void OnRefreshTimer();

public:
    /**
     * Blueprint implementable events for custom loading logic
     */

    /**
     * Called when campaign loading starts (for custom progress UI)
     * @param FilePath Path of file being loaded
     * @param CampaignName Name of campaign being loaded
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Loading Events")
    void OnCampaignLoadStartedEvent(const FString& FilePath, const FString& CampaignName);

    /**
     * Called when campaign loading completes (for custom processing)
     * @param CampaignData The loaded campaign data
     * @param Result The parse result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Loading Events")
    void OnCampaignLoadCompletedEvent(const FCampaignData& CampaignData, EJSONParseResult Result);

    /**
     * Called to perform custom validation (override in Blueprint)
     * @param JSONString The JSON string to validate
     * @return Custom validation result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Loading Events")
    FJSONValidationResult PerformCustomValidation(const FString& JSONString);

    /**
     * Called to customize campaign metadata extraction (override in Blueprint)
     * @param FilePath Path to the campaign file
     * @param JSONObject Parsed JSON object
     * @return Custom metadata
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Campaign Loading Events")
    FCampaignFileMetadata ExtractCustomMetadata(const FString& FilePath, const FString& JSONString);
};
