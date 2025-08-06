// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/TriggerVolume.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "PlayableLayoutActor.generated.h"

/**
 * Layout themes
 */
UENUM(BlueprintType)
enum class ELayoutTheme : uint8
{
    JediTemple          UMETA(DisplayName = "Jedi Temple"),
    SithAcademy         UMETA(DisplayName = "Sith Academy"),
    CityMarket          UMETA(DisplayName = "City Market"),
    SpaceportHub        UMETA(DisplayName = "Spaceport Hub"),
    BountyGuildHall     UMETA(DisplayName = "Bounty Guild Hall"),
    DesertTemple        UMETA(DisplayName = "Desert Temple"),
    ForestShrine        UMETA(DisplayName = "Forest Shrine"),
    UndergroundBase     UMETA(DisplayName = "Underground Base"),
    StarshipInterior    UMETA(DisplayName = "Starship Interior"),
    CantinaTavern       UMETA(DisplayName = "Cantina Tavern")
};

/**
 * AIDM map data structure
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAIDMMapData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    FString LayoutID;

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    FString LayoutName;

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    TArray<FString> NPCSpawnTags; // Tags for NPC spawn points

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    TArray<FString> EnemySpawnTags; // Tags for enemy spawn points

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    TArray<FString> LootSpawnTags; // Tags for loot spawn points

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    TArray<FString> InteractionTags; // Tags for interaction points

    UPROPERTY(BlueprintReadWrite, Category = "AIDM Map Data")
    TMap<FString, FString> LayoutProperties; // Custom properties

    FAIDMMapData()
    {
        LayoutID = TEXT("");
        LayoutName = TEXT("Unknown Layout");
        Description = TEXT("");
    }
};

/**
 * Spawn point data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSpawnPointData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FString SpawnTag;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FString SpawnType; // "NPC", "Enemy", "Loot", "Interaction"

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FVector Location;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    FRotator Rotation;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    bool bIsOccupied;

    UPROPERTY(BlueprintReadWrite, Category = "Spawn Point")
    AActor* SpawnedActor;

    FSpawnPointData()
    {
        SpawnTag = TEXT("");
        SpawnType = TEXT("NPC");
        Location = FVector::ZeroVector;
        Rotation = FRotator::ZeroRotator;
        bIsOccupied = false;
        SpawnedActor = nullptr;
    }
};

/**
 * Layout events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayoutLoaded, const FAIDMMapData&, MapData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorSpawned, const FString&, SpawnTag, AActor*, SpawnedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTriggerActivated, const FString&, TriggerTag);

/**
 * Spawn Point Actor - Marks locations for procedural spawning
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ASpawnPointActor : public AActor
{
    GENERATED_BODY()

public:
    ASpawnPointActor();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Get spawn point data
     * @return Spawn point data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Spawn Point")
    FSpawnPointData GetSpawnPointData() const { return SpawnData; }

    /**
     * Set spawn point data
     * @param NewSpawnData New spawn point data
     */
    UFUNCTION(BlueprintCallable, Category = "Spawn Point")
    void SetSpawnPointData(const FSpawnPointData& NewSpawnData);

    /**
     * Spawn actor at this point
     * @param ActorClass Class of actor to spawn
     * @return Spawned actor
     */
    UFUNCTION(BlueprintCallable, Category = "Spawn Point")
    AActor* SpawnActorAtPoint(TSubclassOf<AActor> ActorClass);

    /**
     * Clear spawned actor
     */
    UFUNCTION(BlueprintCallable, Category = "Spawn Point")
    void ClearSpawnedActor();

    /**
     * Check if spawn point is available
     * @return True if spawn point is not occupied
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Spawn Point")
    bool IsAvailable() const { return !SpawnData.bIsOccupied; }

protected:
    // Spawn point data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Point")
    FSpawnPointData SpawnData;

    // Visual components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* SpawnMarkerMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBillboardComponent* SpawnIcon;

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when actor is spawned at this point
     * @param SpawnedActor The spawned actor
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Spawn Point Events")
    void OnActorSpawnedAtPoint(AActor* SpawnedActor);

    /**
     * Called when spawned actor is cleared
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Spawn Point Events")
    void OnSpawnedActorCleared();
};

/**
 * Playable Layout Actor - Modular level template for AIDM integration
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API APlayableLayoutActor : public AActor
{
    GENERATED_BODY()

public:
    APlayableLayoutActor();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize layout with AIDM data
     * @param MapData AIDM map data to apply
     */
    UFUNCTION(BlueprintCallable, Category = "Playable Layout")
    void InitializeLayout(const FAIDMMapData& MapData);

    /**
     * Load layout from campaign loader
     * @param LayoutID ID of layout to load
     * @return True if layout was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Playable Layout")
    bool LoadLayoutFromCampaign(const FString& LayoutID);

    /**
     * Get spawn point by tag
     * @param SpawnTag Tag of spawn point to find
     * @return Spawn point actor (null if not found)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Playable Layout")
    ASpawnPointActor* GetSpawnPointByTag(const FString& SpawnTag) const;

    /**
     * Get all spawn points of type
     * @param SpawnType Type of spawn points to get
     * @return Array of spawn points of the specified type
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Playable Layout")
    TArray<ASpawnPointActor*> GetSpawnPointsByType(const FString& SpawnType) const;

    /**
     * Get available spawn points
     * @param SpawnType Type of spawn points (empty = all types)
     * @return Array of available spawn points
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Playable Layout")
    TArray<ASpawnPointActor*> GetAvailableSpawnPoints(const FString& SpawnType = TEXT("")) const;

    /**
     * Spawn actor at tagged location
     * @param SpawnTag Tag of spawn point
     * @param ActorClass Class of actor to spawn
     * @return Spawned actor
     */
    UFUNCTION(BlueprintCallable, Category = "Playable Layout")
    AActor* SpawnActorAtTag(const FString& SpawnTag, TSubclassOf<AActor> ActorClass);

    /**
     * Clear all spawned actors
     */
    UFUNCTION(BlueprintCallable, Category = "Playable Layout")
    void ClearAllSpawnedActors();

    /**
     * Set layout theme
     * @param NewTheme New theme for the layout
     */
    UFUNCTION(BlueprintCallable, Category = "Playable Layout")
    void SetLayoutTheme(ELayoutTheme NewTheme);

    /**
     * Get layout data
     * @return Current AIDM map data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Playable Layout")
    FAIDMMapData GetLayoutData() const { return AIDMMapData; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Layout Events")
    FOnLayoutLoaded OnLayoutLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Layout Events")
    FOnActorSpawned OnActorSpawned;

    UPROPERTY(BlueprintAssignable, Category = "Layout Events")
    FOnDialogueTriggerActivated OnDialogueTriggerActivated;

protected:
    // Layout data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout Settings")
    FString LayoutName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout Settings")
    ELayoutTheme Theme;

    UPROPERTY(BlueprintReadOnly, Category = "Layout Settings")
    FAIDMMapData AIDMMapData;

    // Core components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* LayoutMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAudioComponent* AmbientAudio;

    // Player start
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UPlayerStartComponent* PlayerStart;

    // Spawn points
    UPROPERTY(BlueprintReadOnly, Category = "Spawn Points")
    TArray<ASpawnPointActor*> SpawnPoints;

    // Dialogue trigger volumes
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue Triggers")
    TArray<ATriggerVolume*> DialogueTriggers;

    // Campaign loader reference
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

private:
    // Helper methods
    void CreateDefaultSpawnPoints();
    void SetupAmbientAudio();
    void ApplyThemeSettings();
    ASpawnPointActor* CreateSpawnPoint(const FString& Tag, const FString& Type, const FVector& Location);

public:
    /**
     * Blueprint implementable events for custom layout logic
     */

    /**
     * Called when layout is loaded (for custom setup)
     * @param MapData The loaded map data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Layout Events")
    void OnLayoutLoadedEvent(const FAIDMMapData& MapData);

    /**
     * Called when actor is spawned in layout (for custom processing)
     * @param SpawnTag Tag where actor was spawned
     * @param SpawnedActor The spawned actor
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Layout Events")
    void OnActorSpawnedEvent(const FString& SpawnTag, AActor* SpawnedActor);

    /**
     * Called to customize layout appearance based on theme (override in Blueprint)
     * @param LayoutTheme The layout theme
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Layout Events")
    void CustomizeLayoutAppearance(ELayoutTheme LayoutTheme);

    /**
     * Called to setup custom ambient effects (override in Blueprint)
     * @param MapData The map data for context
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Layout Events")
    void SetupCustomAmbientEffects(const FAIDMMapData& MapData);
};
