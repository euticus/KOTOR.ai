// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Space/ShipPawn.h"
#include "SpaceEncounterManager.generated.h"

/**
 * Space encounter types
 */
UENUM(BlueprintType)
enum class ESpaceEncounterType : uint8
{
    Combat          UMETA(DisplayName = "Combat"),
    Ambush          UMETA(DisplayName = "Ambush"),
    Patrol          UMETA(DisplayName = "Patrol"),
    Minefield       UMETA(DisplayName = "Minefield"),
    Derelict        UMETA(DisplayName = "Derelict Ship"),
    Asteroid        UMETA(DisplayName = "Asteroid Field"),
    Station         UMETA(DisplayName = "Space Station"),
    Anomaly         UMETA(DisplayName = "Space Anomaly"),
    Rescue          UMETA(DisplayName = "Rescue Mission"),
    Blockade        UMETA(DisplayName = "Blockade")
};

/**
 * Space encounter data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSpaceEncounter
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    FString EncounterID;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    ESpaceEncounterType EncounterType;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    FString Title;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    EShipFaction HostileFaction;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    TArray<FString> EnemyShipTypes; // Types of enemy ships to spawn

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    int32 EnemyCount;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    float DifficultyRating; // 1.0 = normal, 2.0 = hard, etc.

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    FVector EncounterLocation;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    float EncounterRadius;

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    TArray<FString> Objectives; // Mission objectives

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    TMap<FString, int32> Rewards; // Reward type -> amount

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    TMap<FString, bool> StoryFlags; // Story flags set on completion

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    bool bIsOptional; // Whether encounter can be avoided

    UPROPERTY(BlueprintReadWrite, Category = "Space Encounter")
    float TimeLimit; // Time limit in seconds (0 = no limit)

    FSpaceEncounter()
    {
        EncounterID = TEXT("");
        EncounterType = ESpaceEncounterType::Combat;
        Title = TEXT("Space Encounter");
        Description = TEXT("A dangerous encounter in space");
        HostileFaction = EShipFaction::Unknown;
        EnemyCount = 1;
        DifficultyRating = 1.0f;
        EncounterLocation = FVector::ZeroVector;
        EncounterRadius = 5000.0f;
        bIsOptional = false;
        TimeLimit = 0.0f;
    }
};

/**
 * Space route data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSpaceRoute
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    FString RouteID;

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    int32 FromPlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    int32 ToPlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    float TravelTime; // Base travel time in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    float EncounterChance; // 0.0 to 1.0 chance of encounter

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    TArray<FSpaceEncounter> PossibleEncounters;

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    bool bIsUnlocked;

    UPROPERTY(BlueprintReadWrite, Category = "Space Route")
    FString UnlockRequirement; // Quest or story flag required

    FSpaceRoute()
    {
        RouteID = TEXT("");
        FromPlanetIndex = -1;
        ToPlanetIndex = -1;
        TravelTime = 60.0f;
        EncounterChance = 0.3f;
        bIsUnlocked = true;
        UnlockRequirement = TEXT("");
    }
};

/**
 * Space encounter events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpaceEncounterStarted, const FSpaceEncounter&, Encounter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpaceEncounterCompleted, const FSpaceEncounter&, Encounter, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpaceEncounterFailed, const FSpaceEncounter&, Encounter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpaceTravelStarted, int32, FromPlanet, int32, ToPlanet);

/**
 * Space Encounter Manager - Manages space travel and encounters
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API USpaceEncounterManager : public UActorComponent
{
    GENERATED_BODY()

public:
    USpaceEncounterManager();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize space encounter system
     * @param CampaignLoader Campaign loader for faction data
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    void InitializeSpaceEncounters(UCampaignLoaderSubsystem* CampaignLoader);

    /**
     * Start space travel between planets
     * @param FromPlanetIndex Source planet index
     * @param ToPlanetIndex Destination planet index
     * @param PlayerShip Player's ship
     * @return True if travel started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    bool StartSpaceTravel(int32 FromPlanetIndex, int32 ToPlanetIndex, AShipPawn* PlayerShip);

    /**
     * Generate random encounter for route
     * @param Route The space route
     * @param PlayerLevel Player level for scaling
     * @return Generated encounter
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    FSpaceEncounter GenerateRandomEncounter(const FSpaceRoute& Route, int32 PlayerLevel);

    /**
     * Start a specific encounter
     * @param Encounter The encounter to start
     * @param PlayerShip Player's ship
     * @return True if encounter started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    bool StartEncounter(const FSpaceEncounter& Encounter, AShipPawn* PlayerShip);

    /**
     * Complete current encounter
     * @param bSuccess Whether encounter was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    void CompleteEncounter(bool bSuccess);

    /**
     * Spawn enemy ships for encounter
     * @param Encounter The encounter data
     * @param SpawnLocation Center location for spawning
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    void SpawnEnemyShips(const FSpaceEncounter& Encounter, const FVector& SpawnLocation);

    /**
     * Get available routes from planet
     * @param PlanetIndex Index of the planet
     * @return Array of available routes
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Space Encounters")
    TArray<FSpaceRoute> GetAvailableRoutes(int32 PlanetIndex) const;

    /**
     * Check if route is unlocked
     * @param FromPlanet Source planet
     * @param ToPlanet Destination planet
     * @return True if route is unlocked
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Space Encounters")
    bool IsRouteUnlocked(int32 FromPlanet, int32 ToPlanet) const;

    /**
     * Unlock space route
     * @param FromPlanet Source planet
     * @param ToPlanet Destination planet
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    void UnlockRoute(int32 FromPlanet, int32 ToPlanet);

    /**
     * Get current encounter
     * @return Current active encounter
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Space Encounters")
    FSpaceEncounter GetCurrentEncounter() const { return CurrentEncounter; }

    /**
     * Check if encounter is active
     * @return True if an encounter is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Space Encounters")
    bool IsEncounterActive() const { return bEncounterActive; }

    /**
     * Get travel progress (0.0 to 1.0)
     * @return Current travel progress
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Space Encounters")
    float GetTravelProgress() const;

    /**
     * Add custom encounter to route
     * @param RouteID ID of the route
     * @param Encounter Encounter to add
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    void AddCustomEncounter(const FString& RouteID, const FSpaceEncounter& Encounter);

    /**
     * Remove encounter from route
     * @param RouteID ID of the route
     * @param EncounterID ID of the encounter to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Space Encounters")
    void RemoveEncounter(const FString& RouteID, const FString& EncounterID);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Space Events")
    FOnSpaceEncounterStarted OnSpaceEncounterStarted;

    UPROPERTY(BlueprintAssignable, Category = "Space Events")
    FOnSpaceEncounterCompleted OnSpaceEncounterCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Space Events")
    FOnSpaceEncounterFailed OnSpaceEncounterFailed;

    UPROPERTY(BlueprintAssignable, Category = "Space Events")
    FOnSpaceTravelStarted OnSpaceTravelStarted;

protected:
    // Space routes
    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    TArray<FSpaceRoute> SpaceRoutes;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    bool bEncounterActive;

    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    FSpaceEncounter CurrentEncounter;

    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    bool bTravelInProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    float TravelStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    float TravelDuration;

    UPROPERTY(BlueprintReadOnly, Category = "Space Encounters")
    int32 TravelDestination;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    // Spawned actors
    UPROPERTY()
    TArray<AShipPawn*> SpawnedEnemyShips;

    UPROPERTY()
    AShipPawn* PlayerShipRef;

    // Encounter templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter Templates")
    /*
     * NOTE:
     * UPROPERTY/UHT cannot serialise nested containers such as
     *     TMap<ESpaceEncounterType, TArray<FSpaceEncounter>>
     * We therefore store all items in a single flat array and, at runtime,
     * helper functions filter/group by ESpaceEncounterType when needed.
     */
    TArray<FSpaceEncounter>EncounterTemplates;

    // Ship spawn classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Spawning")
    TMap<FString, TSubclassOf<AShipPawn>> ShipClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Spawning")
    TSubclassOf<AShipPawn> DefaultEnemyShipClass;

private:
    // Helper methods
    void LoadSpaceRoutes();
    void CreateDefaultEncounters();
    FSpaceRoute* FindRoute(int32 FromPlanet, int32 ToPlanet);
    FString GenerateEncounterID();
    FVector GetRandomSpawnLocation(const FVector& Center, float Radius);
    void CleanupEncounter();
    void ProcessTravelProgress(float DeltaTime);

public:
    /**
     * Blueprint implementable events for custom space logic
     */

    /**
     * Called when space encounter starts (for custom setup)
     * @param Encounter The encounter that started
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Space Events")
    void OnSpaceEncounterStartedEvent(const FSpaceEncounter& Encounter);

    /**
     * Called when space encounter completes (for custom rewards)
     * @param Encounter The completed encounter
     * @param bSuccess Whether encounter was successful
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Space Events")
    void OnSpaceEncounterCompletedEvent(const FSpaceEncounter& Encounter, bool bSuccess);

    /**
     * Called to generate custom encounter (override in Blueprint)
     * @param Route The space route
     * @param PlayerLevel Player level for scaling
     * @return Custom generated encounter
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Space Events")
    FSpaceEncounter GenerateCustomEncounter(const FSpaceRoute& Route, int32 PlayerLevel);

    /**
     * Called to spawn custom enemy ship (override in Blueprint)
     * @param ShipType Type of ship to spawn
     * @param SpawnLocation Location to spawn at
     * @param Faction Ship faction
     * @return Spawned ship pawn
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Space Events")
    AShipPawn* SpawnCustomEnemyShip(const FString& ShipType, const FVector& SpawnLocation, EShipFaction Faction);
};
