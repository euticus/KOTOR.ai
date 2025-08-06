// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Multiplayer/CampaignSessionManager.h"
#include "CloudSaveSubsystem.generated.h"

/**
 * Cloud save providers
 */
UENUM(BlueprintType)
enum class ECloudProvider : uint8
{
    Firebase        UMETA(DisplayName = "Firebase"),
    Supabase        UMETA(DisplayName = "Supabase"),
    AWS             UMETA(DisplayName = "AWS"),
    Custom          UMETA(DisplayName = "Custom API")
};

/**
 * Save data types
 */
UENUM(BlueprintType)
enum class ESaveDataType : uint8
{
    Campaign        UMETA(DisplayName = "Campaign Data"),
    PlayerProgress  UMETA(DisplayName = "Player Progress"),
    SessionState    UMETA(DisplayName = "Session State"),
    UserSettings    UMETA(DisplayName = "User Settings"),
    Custom          UMETA(DisplayName = "Custom Data")
};

/**
 * Cloud save entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCloudSaveEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FString SaveID;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FString UserID;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    ESaveDataType DataType;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FString SaveName;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FString SaveData; // JSON serialized data

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FDateTime CreatedAt;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    FDateTime UpdatedAt;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    int32 Version;

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    int64 DataSize; // Size in bytes

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    TMap<FString, FString> Metadata; // Additional metadata

    UPROPERTY(BlueprintReadWrite, Category = "Cloud Save")
    bool bIsPublic; // Whether save can be shared

    FCloudSaveEntry()
    {
        SaveID = TEXT("");
        UserID = TEXT("");
        DataType = ESaveDataType::Campaign;
        SaveName = TEXT("Untitled Save");
        Description = TEXT("");
        SaveData = TEXT("");
        Version = 1;
        DataSize = 0;
        bIsPublic = false;
    }
};

/**
 * User profile data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FUserProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    FString UserID;

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    FString Username;

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    FString Email;

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    FDateTime CreatedAt;

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    FDateTime LastLoginAt;

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    TMap<FString, FString> UserSettings;

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    TArray<FString> SavedCampaigns; // IDs of saved campaigns

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    int32 TotalPlayTime; // Total play time in seconds

    UPROPERTY(BlueprintReadWrite, Category = "User Profile")
    TMap<FString, int32> Achievements; // Achievement ID -> Progress

    FUserProfile()
    {
        UserID = TEXT("");
        Username = TEXT("");
        Email = TEXT("");
        TotalPlayTime = 0;
    }
};

/**
 * Cloud save events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloudSaveCompleted, const FCloudSaveEntry&, SaveEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloudLoadCompleted, const FCloudSaveEntry&, SaveEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCloudOperationFailed, const FString&, Operation, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserAuthenticated, const FUserProfile&, UserProfile);

/**
 * Cloud Save Subsystem - Manages cloud saves and persistent campaigns
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UCloudSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Initialize cloud save system
     * @param Provider Cloud provider to use
     * @param APIKey API key for the provider
     * @param ProjectID Project ID (for Firebase/Supabase)
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void InitializeCloudSave(ECloudProvider Provider, const FString& APIKey, const FString& ProjectID = TEXT(""));

    /**
     * Authenticate user
     * @param Email User email
     * @param Password User password
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void AuthenticateUser(const FString& Email, const FString& Password);

    /**
     * Create new user account
     * @param Email User email
     * @param Password User password
     * @param Username Display username
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void CreateUserAccount(const FString& Email, const FString& Password, const FString& Username);

    /**
     * Save data to cloud
     * @param DataType Type of data being saved
     * @param SaveName Name for the save
     * @param SaveData JSON serialized data
     * @param Description Optional description
     * @param bPublic Whether save should be public
     * @return Save ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    FString SaveToCloud(ESaveDataType DataType, const FString& SaveName, const FString& SaveData, 
                       const FString& Description = TEXT(""), bool bPublic = false);

    /**
     * Load data from cloud
     * @param SaveID ID of the save to load
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void LoadFromCloud(const FString& SaveID);

    /**
     * Delete save from cloud
     * @param SaveID ID of the save to delete
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void DeleteFromCloud(const FString& SaveID);

    /**
     * Get user's saves
     * @param DataType Type of saves to retrieve (optional)
     * @return Array of user's save entries
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void GetUserSaves(ESaveDataType DataType = ESaveDataType::Campaign);

    /**
     * Get public saves
     * @param DataType Type of saves to retrieve
     * @param MaxResults Maximum number of results
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void GetPublicSaves(ESaveDataType DataType, int32 MaxResults = 20);

    /**
     * Share save publicly
     * @param SaveID ID of the save to share
     * @param bMakePublic Whether to make save public
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void ShareSave(const FString& SaveID, bool bMakePublic);

    /**
     * Update user profile
     * @param UserProfile Updated profile data
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void UpdateUserProfile(const FUserProfile& UserProfile);

    /**
     * Sync local save with cloud
     * @param LocalSaveData Local save data to sync
     * @param SaveID Cloud save ID to sync with
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void SyncSaveData(const FString& LocalSaveData, const FString& SaveID);

    /**
     * Check if user is authenticated
     * @return True if user is logged in
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cloud Save")
    bool IsUserAuthenticated() const { return bIsAuthenticated; }

    /**
     * Get current user profile
     * @return Current user profile
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cloud Save")
    FUserProfile GetCurrentUserProfile() const { return CurrentUserProfile; }

    /**
     * Get cached saves
     * @return Array of cached save entries
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cloud Save")
    TArray<FCloudSaveEntry> GetCachedSaves() const { return CachedSaves; }

    /**
     * Enable auto-save to cloud
     * @param bEnable Whether to enable auto-save
     * @param SaveInterval Interval between auto-saves (seconds)
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void SetAutoSaveEnabled(bool bEnable, float SaveInterval = 300.0f);

    /**
     * Force auto-save now
     */
    UFUNCTION(BlueprintCallable, Category = "Cloud Save")
    void ForceAutoSave();

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Cloud Save Events")
    FOnCloudSaveCompleted OnCloudSaveCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Cloud Save Events")
    FOnCloudLoadCompleted OnCloudLoadCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Cloud Save Events")
    FOnCloudOperationFailed OnCloudOperationFailed;

    UPROPERTY(BlueprintAssignable, Category = "Cloud Save Events")
    FOnUserAuthenticated OnUserAuthenticated;

protected:
    // Cloud provider settings
    UPROPERTY()
    ECloudProvider CurrentProvider;

    UPROPERTY()
    FString APIKey;

    UPROPERTY()
    FString ProjectID;

    UPROPERTY()
    FString BaseURL;

    // User authentication
    UPROPERTY()
    bool bIsAuthenticated;

    UPROPERTY()
    FString AuthToken;

    UPROPERTY()
    FUserProfile CurrentUserProfile;

    // Save data cache
    UPROPERTY()
    TArray<FCloudSaveEntry> CachedSaves;

    // Auto-save settings
    UPROPERTY()
    bool bAutoSaveEnabled;

    UPROPERTY()
    float AutoSaveInterval;

    UPROPERTY()
    FTimerHandle AutoSaveTimer;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    UPROPERTY()
    UCampaignSessionManager* SessionManager;

private:
    // HTTP request handling
    void SendHTTPRequest(const FString& Endpoint, const FString& Method, const FString& Data, 
                        const FString& Operation);
    void HandleHTTPResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, 
                           FString Operation);

    // Provider-specific implementations
    void InitializeFirebase();
    void InitializeSupabase();
    void InitializeAWS();
    void InitializeCustomAPI();

    // Authentication helpers
    void ProcessAuthResponse(const TSharedPtr<FJsonObject>& ResponseJson);
    void ProcessSaveResponse(const TSharedPtr<FJsonObject>& ResponseJson);
    void ProcessLoadResponse(const TSharedPtr<FJsonObject>& ResponseJson);

    // Data serialization
    FString SerializeSaveEntry(const FCloudSaveEntry& SaveEntry);
    FCloudSaveEntry DeserializeSaveEntry(const FString& JsonData);
    FString SerializeUserProfile(const FUserProfile& UserProfile);
    FUserProfile DeserializeUserProfile(const FString& JsonData);

    // Helper methods
    FString GenerateSaveID();
    FString GetProviderEndpoint(const FString& Operation);
    TMap<FString, FString> GetAuthHeaders();
    void CacheSaveEntry(const FCloudSaveEntry& SaveEntry);
    void PerformAutoSave();

public:
    /**
     * Blueprint implementable events for custom cloud logic
     */

    /**
     * Called when cloud save operation completes (for custom processing)
     * @param SaveEntry The saved entry
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cloud Save Events")
    void OnCloudSaveCompletedEvent(const FCloudSaveEntry& SaveEntry);

    /**
     * Called when cloud load operation completes (for custom processing)
     * @param SaveEntry The loaded entry
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cloud Save Events")
    void OnCloudLoadCompletedEvent(const FCloudSaveEntry& SaveEntry);

    /**
     * Called when user is authenticated (for custom setup)
     * @param UserProfile The authenticated user profile
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cloud Save Events")
    void OnUserAuthenticatedEvent(const FUserProfile& UserProfile);

    /**
     * Called to get custom save data (override in Blueprint)
     * @param DataType Type of data to save
     * @return Custom save data as JSON string
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cloud Save Events")
    FString GetCustomSaveData(ESaveDataType DataType);

    /**
     * Called to process custom loaded data (override in Blueprint)
     * @param SaveEntry The loaded save entry
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cloud Save Events")
    void ProcessCustomLoadedData(const FCloudSaveEntry& SaveEntry);
};
