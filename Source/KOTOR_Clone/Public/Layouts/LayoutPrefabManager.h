// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Layouts/PlayableLayoutActor.h"
#include "LayoutPrefabManager.generated.h"

/**
 * Layout prefab types
 */
UENUM(BlueprintType)
enum class ELayoutPrefabType : uint8
{
    TempleLayout        UMETA(DisplayName = "Temple Layout"),
    SpaceportLayout     UMETA(DisplayName = "Spaceport Layout"),
    CityLayout          UMETA(DisplayName = "City Layout"),
    CantinLayout        UMETA(DisplayName = "Cantina Layout"),
    AcademyLayout       UMETA(DisplayName = "Academy Layout"),
    TombLayout          UMETA(DisplayName = "Tomb Layout"),
    ShipLayout          UMETA(DisplayName = "Ship Layout"),
    OutpostLayout       UMETA(DisplayName = "Outpost Layout"),
    RuinsLayout         UMETA(DisplayName = "Ruins Layout"),
    CustomLayout        UMETA(DisplayName = "Custom Layout")
};

/**
 * Prefab spawn point data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPrefabSpawnPoint
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FString SpawnID;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FString SpawnTag;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FVector Location;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FRotator Rotation;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FVector Scale;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    TArray<FString> AllowedActorTypes;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    int32 Priority; // Higher priority spawn points are used first

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    bool bIsOccupied;

    FPrefabSpawnPoint()
    {
        SpawnID = TEXT("");
        SpawnTag = TEXT("generic");
        Location = FVector::ZeroVector;
        Rotation = FRotator::ZeroRotator;
        Scale = FVector::OneVector;
        Priority = 1;
        bIsOccupied = false;
    }
};

/**
 * Layout prefab data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FLayoutPrefabData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    FString PrefabID;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    FString PrefabName;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    ELayoutPrefabType PrefabType;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    TArray<FPrefabSpawnPoint> SpawnPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    TArray<UStaticMesh*> GeometryMeshes;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    TArray<FString> RequiredTags;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    FVector PrefabBounds;

    UPROPERTY(BlueprintReadWrite, Category = "Layout Prefab")
    TMap<FString, FString> PrefabMetadata;

    FLayoutPrefabData()
    {
        PrefabID = TEXT("");
        PrefabName = TEXT("Unnamed Prefab");
        PrefabType = ELayoutPrefabType::CustomLayout;
        Description = TEXT("");
        PrefabBounds = FVector(2000.0f, 2000.0f, 500.0f);
    }
};

/**
 * Layout prefab events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrefabLoaded, const FLayoutPrefabData&, PrefabData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPrefabSpawned, const FString&, PrefabID, APlayableLayoutActor*, LayoutActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrefabUnloaded, const FString&, PrefabID);

/**
 * Layout Prefab Actor - Individual prefab instance
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ALayoutPrefabActor : public AActor
{
    GENERATED_BODY()

public:
    ALayoutPrefabActor();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize prefab
     * @param PrefabData Data for this prefab
     */
    UFUNCTION(BlueprintCallable, Category = "Layout Prefab")
    void InitializePrefab(const FLayoutPrefabData& TargetPrefabData);

    /**
     * Get prefab data
     * @return Current prefab data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Layout Prefab")
    FLayoutPrefabData GetPrefabData() const { return PrefabData; }

    /**
     * Get spawn points by tag
     * @param Tag Tag to filter by
     * @return Array of spawn points with matching tag
     */
    UFUNCTION(BlueprintCallable, Category = "Layout Prefab")
    TArray<FPrefabSpawnPoint> GetSpawnPointsByTag(const FString& Tag) const;

    /**
     * Get available spawn points
     * @param ActorType Type of actor to spawn
     * @return Array of available spawn points
     */
    UFUNCTION(BlueprintCallable, Category = "Layout Prefab")
    TArray<FPrefabSpawnPoint> GetAvailableSpawnPoints(const FString& ActorType = TEXT("")) const;

    /**
     * Occupy spawn point
     * @param SpawnID ID of spawn point to occupy
     * @param bOccupied Whether spawn point should be occupied
     */
    UFUNCTION(BlueprintCallable, Category = "Layout Prefab")
    void SetSpawnPointOccupied(const FString& SpawnID, bool bOccupied);

    /**
     * Get spawn point transform
     * @param SpawnID ID of spawn point
     * @return Transform of spawn point
     */
    UFUNCTION(BlueprintCallable, Category = "Layout Prefab")
    FTransform GetSpawnPointTransform(const FString& SpawnID) const;

protected:
    // Prefab data
    UPROPERTY(BlueprintReadOnly, Category = "Prefab Data")
    FLayoutPrefabData PrefabData;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* PrefabRoot;

    UPROPERTY(BlueprintReadOnly, Category = "Components")
    TArray<UStaticMeshComponent*> GeometryComponents;

    UPROPERTY(BlueprintReadOnly, Category = "Components")
    TArray<USceneComponent*> SpawnPointComponents;

private:
    // Helper methods
    void CreateGeometry();
    void CreateSpawnPoints();
    void SetupPrefabBounds();

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when prefab is initialized
     * @param InitializedPrefabData The prefab data used for initialization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Prefab Events")
    void OnPrefabInitializedEvent(const FLayoutPrefabData& InitializedPrefabData);

    /**
     * Called to customize prefab geometry (override in Blueprint)
     * @param PrefabData The prefab data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Prefab Events")
    void CustomizePrefabGeometry(const FLayoutPrefabData& TargetPrefabData);
};

/**
 * Layout Prefab Manager - Manages layout prefabs for procedural testing
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ALayoutPrefabManager : public AActor
{
    GENERATED_BODY()

public:
    ALayoutPrefabManager();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Load prefab data
     * @param PrefabType Type of prefab to load
     * @return Loaded prefab data
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    FLayoutPrefabData LoadPrefabData(ELayoutPrefabType PrefabType);

    /**
     * Spawn prefab
     * @param PrefabType Type of prefab to spawn
     * @param Location Location to spawn prefab
     * @param Rotation Rotation for prefab
     * @return Spawned prefab actor
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    ALayoutPrefabActor* SpawnPrefab(ELayoutPrefabType PrefabType, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator);

    /**
     * Spawn prefab from data
     * @param PrefabData Prefab data to spawn
     * @param Location Location to spawn prefab
     * @param Rotation Rotation for prefab
     * @return Spawned prefab actor
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    ALayoutPrefabActor* SpawnPrefabFromData(const FLayoutPrefabData& TargetPrefabData, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator);

    /**
     * Get available prefab types
     * @return Array of available prefab types
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Prefab Manager")
    TArray<ELayoutPrefabType> GetAvailablePrefabTypes() const;

    /**
     * Register custom prefab
     * @param PrefabData Custom prefab data to register
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    void RegisterCustomPrefab(const FLayoutPrefabData& TargetPrefabData);

    /**
     * Unregister prefab
     * @param PrefabID ID of prefab to unregister
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    void UnregisterPrefab(const FString& PrefabID);

    /**
     * Get spawned prefabs
     * @return Array of currently spawned prefabs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Prefab Manager")
    TArray<ALayoutPrefabActor*> GetSpawnedPrefabs() const { return SpawnedPrefabs; }

    /**
     * Clear all spawned prefabs
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    void ClearAllPrefabs();

    /**
     * Populate prefab with AIDM data
     * @param PrefabActor Prefab to populate
     * @param LayoutData AIDM layout data
     * @return True if population was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Prefab Manager")
    bool PopulatePrefabWithAIDM(ALayoutPrefabActor* PrefabActor, const FString& LayoutData);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Prefab Manager Events")
    FOnPrefabLoaded OnPrefabLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Prefab Manager Events")
    FOnPrefabSpawned OnPrefabSpawned;

    UPROPERTY(BlueprintAssignable, Category = "Prefab Manager Events")
    FOnPrefabUnloaded OnPrefabUnloaded;

protected:
    // Prefab registry
    UPROPERTY(BlueprintReadOnly, Category = "Prefab Registry")
    TMap<ELayoutPrefabType, FLayoutPrefabData> PrefabRegistry;

    UPROPERTY(BlueprintReadOnly, Category = "Prefab Registry")
    TMap<FString, FLayoutPrefabData> CustomPrefabs;

    // Spawned prefabs
    UPROPERTY(BlueprintReadOnly, Category = "Spawned Prefabs")
    TArray<ALayoutPrefabActor*> SpawnedPrefabs;

    // Prefab actor class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefab Settings")
    TSubclassOf<ALayoutPrefabActor> PrefabActorClass;

private:
    // Helper methods
    void InitializePrefabRegistry();
    FLayoutPrefabData CreateTemplePrefab();
    FLayoutPrefabData CreateSpaceportPrefab();
    FLayoutPrefabData CreateCityPrefab();
    FLayoutPrefabData CreateCantinaPrefab();
    FLayoutPrefabData CreateAcademyPrefab();
    FLayoutPrefabData CreateTombPrefab();
    FLayoutPrefabData CreateShipPrefab();
    FLayoutPrefabData CreateOutpostPrefab();
    FLayoutPrefabData CreateRuinsPrefab();

    // Spawn point generation helpers
    TArray<FPrefabSpawnPoint> GenerateSpawnPoints(const FString& LayoutType, const FVector& Bounds);
    FPrefabSpawnPoint CreateSpawnPoint(const FString& Tag, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator);

public:
    /**
     * Blueprint implementable events for custom prefab logic
     */

    /**
     * Called when prefab is loaded (for custom processing)
     * @param PrefabData The loaded prefab data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Prefab Manager Events")
    void OnPrefabLoadedEvent(const FLayoutPrefabData& TargetPrefabData);

    /**
     * Called when prefab is spawned (for custom setup)
     * @param PrefabID ID of spawned prefab
     * @param PrefabActor The spawned prefab actor
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Prefab Manager Events")
    void OnPrefabSpawnedEvent(const FString& PrefabID, ALayoutPrefabActor* PrefabActor);

    /**
     * Called to create custom prefab data (override in Blueprint)
     * @param PrefabType Type of prefab to create
     * @return Custom prefab data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Prefab Manager Events")
    FLayoutPrefabData CreateCustomPrefabData(ELayoutPrefabType PrefabType);

    /**
     * Called to customize spawn points (override in Blueprint)
     * @param PrefabType Type of prefab
     * @param Bounds Prefab bounds
     * @return Custom spawn points
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Prefab Manager Events")
    TArray<FPrefabSpawnPoint> GenerateCustomSpawnPoints(ELayoutPrefabType PrefabType, const FVector& Bounds);
};
