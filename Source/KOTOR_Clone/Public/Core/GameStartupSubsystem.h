// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "AI/AIDirectorSubsystem.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Animation/AnimationTagMap.h"
#include "GameStartupSubsystem.generated.h"

/**
 * Startup actor types
 */
UENUM(BlueprintType)
enum class EStartupActorType : uint8
{
    PlayerCharacter     UMETA(DisplayName = "Player Character"),
    AIDirector          UMETA(DisplayName = "AI Director"),
    TestEnemies         UMETA(DisplayName = "Test Enemies"),
    DebugTools          UMETA(DisplayName = "Debug Tools"),
    NavMeshVolume       UMETA(DisplayName = "Nav Mesh Volume"),
    Lighting            UMETA(DisplayName = "Lighting"),
    AudioSources        UMETA(DisplayName = "Audio Sources"),
    CameraSystems       UMETA(DisplayName = "Camera Systems")
};

/**
 * Startup actor data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FStartupActorData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    EStartupActorType ActorType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    TSoftClassPtr<AActor> ActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    FVector SpawnLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    FRotator SpawnRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    bool bSpawnInTestMode; // Only spawn in test mode

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    bool bSpawnInShippingBuild; // Spawn in shipping builds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    int32 Priority; // Spawn order priority

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Actor")
    TMap<FString, FString> SpawnParameters; // Additional spawn parameters

    FStartupActorData()
    {
        ActorType = EStartupActorType::PlayerCharacter;
        ActorClass = nullptr;
        SpawnLocation = FVector::ZeroVector;
        SpawnRotation = FRotator::ZeroRotator;
        bSpawnInTestMode = true;
        bSpawnInShippingBuild = false;
        Priority = 1;
    }
};

/**
 * Startup configuration
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGameStartupConfiguration
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    bool bEnableTestMode; // Whether test mode is enabled

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    bool bAutoSpawnActors; // Whether to auto-spawn startup actors

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    bool bLoadDefaultMap; // Whether to load default startup map

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    FString DefaultStartupMap; // Default map to load

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    TArray<FStartupActorData> StartupActors; // Actors to spawn on startup

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    bool bInitializeAIDirector; // Whether to initialize AI Director

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    bool bInitializeMusicSubsystem; // Whether to initialize music subsystem

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    bool bLoadAnimationTagMap; // Whether to load animation tag map

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    TSoftObjectPtr<UAnimationTagMap> DefaultAnimationTagMap; // Default animation tag map

    FGameStartupConfiguration()
    {
        bEnableTestMode = true;
        bAutoSpawnActors = true;
        bLoadDefaultMap = false;
        DefaultStartupMap = TEXT("/Game/Maps/TestLevel");
        bInitializeAIDirector = true;
        bInitializeMusicSubsystem = true;
        bLoadAnimationTagMap = true;
        DefaultAnimationTagMap = nullptr;
    }
};

/**
 * Startup events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStartupCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartupActorSpawned, AActor*, SpawnedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubsystemInitialized, const FString&, SubsystemName);

/**
 * Game Startup Subsystem - Run during GameInstance init phase with auto-loading
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UGameStartupSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UGameStartupSubsystem();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // UGameInstanceSubsystem interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
    /**
     * Initialize game startup
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    void InitializeGameStartup();

    /**
     * Spawn startup actors
     * @param bForceSpawn Whether to force spawn even if not in test mode
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    void SpawnStartupActors(bool bForceSpawn = false);

    /**
     * Initialize AI Director
     * @return True if initialized successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    bool InitializeAIDirector();

    /**
     * Initialize music subsystem
     * @return True if initialized successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    bool InitializeMusicSubsystem();

    /**
     * Load animation tag map
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    bool LoadAnimationTagMap();

    /**
     * Add startup actor
     * @param ActorData Actor data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    void AddStartupActor(const FStartupActorData& ActorData);

    /**
     * Remove startup actor
     * @param ActorType Type of actor to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    void RemoveStartupActor(EStartupActorType ActorType);

    /**
     * Set startup configuration
     * @param Configuration New startup configuration
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    void SetStartupConfiguration(const FGameStartupConfiguration& Configuration);

    /**
     * Get startup configuration
     * @return Current startup configuration
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Startup")
    FGameStartupConfiguration GetStartupConfiguration() const { return StartupConfiguration; }

    /**
     * Check if startup is complete
     * @return True if startup is complete
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Startup")
    bool IsStartupComplete() const { return bStartupComplete; }

    /**
     * Check if test mode is enabled
     * @return True if test mode is enabled
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Startup")
    bool IsTestModeEnabled() const { return StartupConfiguration.bEnableTestMode; }

    /**
     * Get spawned actors by type
     * @param ActorType Type of actors to get
     * @return Array of spawned actors
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Startup")
    TArray<AActor*> GetSpawnedActorsByType(EStartupActorType ActorType) const;

    /**
     * Load startup configuration from file
     * @param FilePath Path to configuration file
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    bool LoadStartupConfigurationFromFile(const FString& FilePath);

    /**
     * Save startup configuration to file
     * @param FilePath Path to save configuration file
     * @return True if saved successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Game Startup")
    bool SaveStartupConfigurationToFile(const FString& FilePath) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Startup Events")
    FOnGameStartupCompleted OnGameStartupCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Startup Events")
    FOnStartupActorSpawned OnStartupActorSpawned;

    UPROPERTY(BlueprintAssignable, Category = "Startup Events")
    FOnSubsystemInitialized OnSubsystemInitialized;

protected:
    // Startup configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Startup Configuration")
    FGameStartupConfiguration StartupConfiguration;

    // Spawned actors
    UPROPERTY(BlueprintReadOnly, Category = "Spawned Actors")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<EStartupActorType, TArray<AActor*>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by EStartupActorType when needed.
     */
    TArray<AActor*>SpawnedActors;

    // Subsystem references
    UPROPERTY()
    UAIDirectorSubsystem* AIDirectorSubsystem;

    UPROPERTY()
    UProceduralMusicSubsystemV2* MusicSubsystem;

    UPROPERTY()
    UAnimationTagMap* AnimationTagMap;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Startup State")
    bool bStartupComplete;

    UPROPERTY(BlueprintReadOnly, Category = "Startup State")
    bool bSubsystemsInitialized;

    UPROPERTY(BlueprintReadOnly, Category = "Startup State")
    float StartupStartTime;

    // Timer handle
    FTimerHandle StartupTimer;

private:
    // Helper methods
    void LoadDefaultConfiguration();
    void InitializeSubsystems();
    void SpawnActorByData(const FStartupActorData& ActorData);
    bool ShouldSpawnActor(const FStartupActorData& ActorData) const;
    void CompleteStartup();

    // Timer callback
    UFUNCTION()
    void OnStartupTimer();

public:
    /**
     * Blueprint implementable events for custom startup logic
     */

    /**
     * Called when game startup begins (for custom initialization)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Startup Events")
    void OnGameStartupBeginEvent();

    /**
     * Called when startup actor is spawned (for custom setup)
     * @param SpawnedActor The spawned actor
     * @param ActorData The actor data used for spawning
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Startup Events")
    void OnStartupActorSpawnedEvent(AActor* SpawnedActor, const FStartupActorData& ActorData);

    /**
     * Called to customize actor spawn (override in Blueprint)
     * @param ActorData Actor data to customize
     * @return Customized actor data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Startup Events")
    FStartupActorData CustomizeActorSpawn(const FStartupActorData& ActorData);

    /**
     * Called to validate startup configuration (override in Blueprint)
     * @param Configuration Configuration to validate
     * @return True if configuration is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Startup Events")
    bool ValidateStartupConfiguration(const FGameStartupConfiguration& Configuration);
};
