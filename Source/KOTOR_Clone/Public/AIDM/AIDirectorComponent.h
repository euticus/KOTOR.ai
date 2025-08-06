// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDirectorComponent.generated.h"

// Forward declarations
class AActor;
class APawn;
class UWorld;

/**
 * Spawn point data for dynamic content
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSpawnPointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    FString SpawnType; // "NPC", "Enemy", "Loot", "Quest"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    FString LayoutName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    bool bIsOccupied;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    AActor* SpawnedActor;

    FSpawnPointData()
    {
        Location = FVector::ZeroVector;
        Rotation = FRotator::ZeroRotator;
        SpawnType = TEXT("NPC");
        LayoutName = TEXT("");
        bIsOccupied = false;
        SpawnedActor = nullptr;
    }
};

/**
 * Event delegates for AI Director
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCampaignLoaded, const FCampaignPlan&, Campaign);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlanetChanged, int32, OldPlanetIndex, int32, NewPlanetIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLayoutChanged, const FString&, OldLayout, const FString&, NewLayout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContentSpawned, AActor*, SpawnedActor);

/**
 * AI Director Component - Manages dynamic content spawning and campaign progression
 * This component acts as the runtime agent that coordinates between AIDM data and Unreal Engine
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAIDirectorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAIDirectorComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /**
     * Initialize the AI Director with a campaign
     * @param CampaignFilePath Path to the campaign JSON file
     * @return True if initialization was successful
     */
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    bool InitializeWithCampaign(const FString& CampaignFilePath);

    /**
     * Change to a specific planet
     * @param PlanetIndex Index of the planet to change to
     * @return True if planet change was successful
     */
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    bool ChangeToPlanet(int32 PlanetIndex);

    /**
     * Change to a specific layout within the current planet
     * @param LayoutName Name of the layout to change to
     * @return True if layout change was successful
     */
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    bool ChangeToLayout(const FString& LayoutName);

    /**
     * Register a spawn point for dynamic content
     * @param SpawnPoint The spawn point data to register
     */
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    void RegisterSpawnPoint(const FSpawnPointData& SpawnPoint);

    /**
     * Spawn content for the current layout
     * @param bForceRespawn If true, will respawn content even if already spawned
     */
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    void SpawnContentForCurrentLayout(bool bForceRespawn = false);

    /**
     * Clear all spawned content
     */
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    void ClearAllSpawnedContent();

    /**
     * Get the current planet data
     * @return Current planet data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI Director")
    FPlanetData GetCurrentPlanetData() const;

    /**
     * Get the current layout data
     * @return Current layout data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI Director")
    FMapLayout GetCurrentLayoutData() const;

    /**
     * Get all spawn points for a specific type
     * @param SpawnType Type of spawn points to get ("NPC", "Enemy", "Loot", "Quest")
     * @return Array of spawn points of the specified type
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI Director")
    TArray<FSpawnPointData> GetSpawnPointsByType(const FString& SpawnType) const;

    /**
     * Check if the AI Director is initialized
     * @return True if initialized with a campaign
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI Director")
    bool IsInitialized() const { return bIsInitialized; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "AI Director Events")
    FOnCampaignLoaded OnCampaignLoaded;

    UPROPERTY(BlueprintAssignable, Category = "AI Director Events")
    FOnPlanetChanged OnPlanetChanged;

    UPROPERTY(BlueprintAssignable, Category = "AI Director Events")
    FOnLayoutChanged OnLayoutChanged;

    UPROPERTY(BlueprintAssignable, Category = "AI Director Events")
    FOnContentSpawned OnContentSpawned;

protected:
    // Campaign loader subsystem reference
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "AI Director")
    bool bIsInitialized;

    UPROPERTY(BlueprintReadOnly, Category = "AI Director")
    int32 CurrentPlanetIndex;

    UPROPERTY(BlueprintReadOnly, Category = "AI Director")
    FString CurrentLayoutName;

    // Spawn point management
    UPROPERTY(BlueprintReadOnly, Category = "AI Director")
    TArray<FSpawnPointData> RegisteredSpawnPoints;

    UPROPERTY(BlueprintReadOnly, Category = "AI Director")
    TArray<AActor*> SpawnedActors;

    // Blueprint classes for spawning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Spawn Classes")
    TSubclassOf<APawn> DefaultNPCClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Spawn Classes")
    TSubclassOf<APawn> DefaultEnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Spawn Classes")
    TSubclassOf<AActor> DefaultLootClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Spawn Classes")
    TSubclassOf<AActor> DefaultQuestClass;

    // NPC class mapping by species
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|NPC Classes")
    TMap<FString, TSubclassOf<APawn>> NPCClassesBySpecies;

    // Enemy class mapping by species
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Enemy Classes")
    TMap<FString, TSubclassOf<APawn>> EnemyClassesBySpecies;

    // Auto-spawn settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Auto Spawn")
    bool bAutoSpawnOnLayoutChange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Auto Spawn")
    bool bAutoSpawnNPCs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Auto Spawn")
    bool bAutoSpawnEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Auto Spawn")
    bool bAutoSpawnLoot;

    // Debug settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Debug")
    bool bDebugMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Director|Debug")
    bool bShowSpawnPointDebug;

private:
    // Internal spawning methods
    void SpawnNPCsForLayout();
    void SpawnEnemiesForLayout();
    void SpawnLootForLayout();
    void SpawnQuestsForLayout();

    // Helper methods
    TSubclassOf<APawn> GetNPCClassForSpecies(const FString& Species) const;
    TSubclassOf<APawn> GetEnemyClassForSpecies(const FString& Species) const;
    FSpawnPointData* FindAvailableSpawnPoint(const FString& SpawnType, const FString& LayoutName);
    void SetupSpawnedActor(AActor* SpawnedActor, const FString& ActorType, const void* Data);

    // Debug helpers
    void DrawSpawnPointDebug() const;
    void LogDirectorState() const;

public:
    /**
     * Blueprint implementable events for custom spawning logic
     */
    
    /**
     * Called when an NPC should be spawned
     * @param NPCData The NPC data from AIDM
     * @param SpawnPoint The spawn point to use
     * @return The spawned NPC actor (can be null if spawn failed)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "AI Director|Spawning")
    APawn* OnSpawnNPC(const FNPCData& NPCData, const FSpawnPointData& SpawnPoint);

    /**
     * Called when an enemy should be spawned
     * @param EnemyData The enemy data from AIDM
     * @param SpawnPoint The spawn point to use
     * @return The spawned enemy actor (can be null if spawn failed)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "AI Director|Spawning")
    APawn* OnSpawnEnemy(const FEnemyData& EnemyData, const FSpawnPointData& SpawnPoint);

    /**
     * Called when loot should be spawned
     * @param LootData The loot data from AIDM
     * @param SpawnPoint The spawn point to use
     * @return The spawned loot actor (can be null if spawn failed)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "AI Director|Spawning")
    AActor* OnSpawnLoot(const FLootItem& LootData, const FSpawnPointData& SpawnPoint);

    /**
     * Called when the layout changes (for custom logic)
     * @param OldLayout The previous layout name
     * @param NewLayout The new layout name
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "AI Director|Events")
    void OnLayoutChangedEvent(const FString& OldLayout, const FString& NewLayout);

    /**
     * Called when the planet changes (for custom logic)
     * @param OldPlanetIndex The previous planet index
     * @param NewPlanetIndex The new planet index
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "AI Director|Events")
    void OnPlanetChangedEvent(int32 OldPlanetIndex, int32 NewPlanetIndex);
};
