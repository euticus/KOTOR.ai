// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "ProceduralPlanetGenerator.generated.h"

/**
 * Biome types for procedural generation
 */
UENUM(BlueprintType)
enum class EPlanetBiome : uint8
{
    Urban           UMETA(DisplayName = "Urban"),
    Desert          UMETA(DisplayName = "Desert"),
    Forest          UMETA(DisplayName = "Forest"),
    Ocean           UMETA(DisplayName = "Ocean"),
    Mountain        UMETA(DisplayName = "Mountain"),
    Volcanic        UMETA(DisplayName = "Volcanic"),
    Ice             UMETA(DisplayName = "Ice"),
    Swamp           UMETA(DisplayName = "Swamp"),
    Asteroid        UMETA(DisplayName = "Asteroid"),
    SpaceStation    UMETA(DisplayName = "Space Station")
};

/**
 * Planet generation parameters
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlanetGenerationParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    FString PlanetName;

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    EPlanetBiome PrimaryBiome;

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    TArray<EPlanetBiome> SecondaryBiomes;

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    FString DominantFaction; // "republic", "sith", "neutral", "criminal"

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    FString PopulationSize; // "sparse", "small", "medium", "large", "massive"

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    FString TechLevel; // "primitive", "standard", "advanced", "cutting_edge"

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    float DangerLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    int32 NumLayouts; // Number of layouts to generate

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    int32 Seed; // Random seed for generation

    UPROPERTY(BlueprintReadWrite, Category = "Planet Generation")
    TMap<FString, FString> CustomProperties; // Additional generation parameters

    FPlanetGenerationParams()
    {
        PlanetName = TEXT("Generated Planet");
        PrimaryBiome = EPlanetBiome::Urban;
        DominantFaction = TEXT("neutral");
        PopulationSize = TEXT("medium");
        TechLevel = TEXT("standard");
        DangerLevel = 0.5f;
        NumLayouts = 3;
        Seed = 0;
    }
};

/**
 * Tile template for modular generation
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FTileTemplate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    FString TileID;

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    FString TileName;

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    EPlanetBiome BiomeType;

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    FString TileType; // "entrance", "combat", "exploration", "social", "boss"

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    FVector TileSize; // Size in Unreal units

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    TArray<FString> ConnectionPoints; // Where this tile can connect to others

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    TArray<FVector> SpawnPoints; // Relative spawn point locations

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    TSoftObjectPtr<UStaticMesh> TileMesh; // 3D mesh for the tile

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    TMap<FString, TSoftObjectPtr<UObject>> TileAssets; // Additional assets

    UPROPERTY(BlueprintReadWrite, Category = "Tile Template")
    float SpawnWeight; // Probability of this tile being selected

    FTileTemplate()
    {
        TileID = TEXT("");
        TileName = TEXT("Basic Tile");
        BiomeType = EPlanetBiome::Urban;
        TileType = TEXT("exploration");
        TileSize = FVector(1000, 1000, 500);
        SpawnWeight = 1.0f;
    }
};

/**
 * Generated layout data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGeneratedLayout
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    FString LayoutID;

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    FString LayoutName;

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    EPlanetBiome BiomeType;

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    TArray<FString> TileIDs; // IDs of tiles in this layout

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    TMap<FString, FTransform> TileTransforms; // TileID -> Transform

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    TArray<FVector> NPCSpawnPoints; // NPC spawn locations

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    TArray<FVector> EnemySpawnPoints; // Enemy spawn locations

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    TArray<FVector> LootSpawnPoints; // Loot spawn locations

    UPROPERTY(BlueprintReadWrite, Category = "Generated Layout")
    FString Description; // AI-generated description

    FGeneratedLayout()
    {
        LayoutID = TEXT("");
        LayoutName = TEXT("Generated Layout");
        BiomeType = EPlanetBiome::Urban;
        Description = TEXT("A procedurally generated area");
    }
};

/**
 * Planet generation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlanetGenerationStarted, const FPlanetGenerationParams&, Params);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlanetGenerationCompleted, const FPlanetData&, GeneratedPlanet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLayoutGenerated, const FGeneratedLayout&, Layout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGenerationProgress, const FString&, Stage, float, Progress);

/**
 * Procedural Planet Generator - Generates planets and layouts dynamically
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UProceduralPlanetGenerator : public UActorComponent
{
    GENERATED_BODY()

public:
    UProceduralPlanetGenerator();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize procedural generation system
     * @param CampaignLoader Campaign loader for faction data
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void InitializeGenerator(UCampaignLoaderSubsystem* CampaignLoader);

    /**
     * Generate a new planet
     * @param GenerationParams Parameters for planet generation
     * @return Generated planet data
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    FPlanetData GeneratePlanet(const FPlanetGenerationParams& GenerationParams);

    /**
     * Generate a layout for a planet
     * @param BiomeType Biome type for the layout
     * @param LayoutType Type of layout ("city", "wilderness", "dungeon", etc.)
     * @param Seed Random seed for generation
     * @return Generated layout
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    FGeneratedLayout GenerateLayout(EPlanetBiome BiomeType, const FString& LayoutType, int32 Seed);

    /**
     * Generate NPCs for a planet
     * @param PlanetData Planet data to generate NPCs for
     * @param NumNPCs Number of NPCs to generate
     * @return Array of generated NPCs
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    TArray<FNPCData> GenerateNPCs(const FPlanetData& PlanetData, int32 NumNPCs);

    /**
     * Generate enemies for a planet
     * @param PlanetData Planet data to generate enemies for
     * @param DifficultyTier Difficulty tier for enemies
     * @return Array of generated enemies
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    TArray<FEnemyData> GenerateEnemies(const FPlanetData& PlanetData, const FString& DifficultyTier);

    /**
     * Generate loot for a planet
     * @param PlanetData Planet data to generate loot for
     * @param NumItems Number of loot items to generate
     * @return Array of generated loot items
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    TArray<FLootItem> GenerateLoot(const FPlanetData& PlanetData, int32 NumItems);

    /**
     * Spawn generated layout in world
     * @param Layout Layout to spawn
     * @param SpawnLocation World location to spawn at
     * @return True if layout was spawned successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    bool SpawnLayoutInWorld(const FGeneratedLayout& Layout, const FVector& SpawnLocation);

    /**
     * Clear spawned layout
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void ClearSpawnedLayout();

    /**
     * Add custom tile template
     * @param TileTemplate Template to add
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void AddTileTemplate(const FTileTemplate& TileTemplate);

    /**
     * Remove tile template
     * @param TileID ID of template to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    void RemoveTileTemplate(const FString& TileID);

    /**
     * Get available tile templates for biome
     * @param BiomeType Biome to get templates for
     * @return Array of available templates
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Generation")
    TArray<FTileTemplate> GetTileTemplatesForBiome(EPlanetBiome BiomeType) const;

    /**
     * Generate planet name
     * @param BiomeType Primary biome
     * @param Faction Dominant faction
     * @param Seed Random seed
     * @return Generated planet name
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    FString GeneratePlanetName(EPlanetBiome BiomeType, const FString& Faction, int32 Seed);

    /**
     * Generate planet description
     * @param GenerationParams Planet generation parameters
     * @return AI-generated planet description
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Generation")
    FString GeneratePlanetDescription(const FPlanetGenerationParams& GenerationParams);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Generation Events")
    FOnPlanetGenerationStarted OnPlanetGenerationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Generation Events")
    FOnPlanetGenerationCompleted OnPlanetGenerationCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Generation Events")
    FOnLayoutGenerated OnLayoutGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Generation Events")
    FOnGenerationProgress OnGenerationProgress;

protected:
    // Tile templates
    UPROPERTY(BlueprintReadOnly, Category = "Procedural Generation")
    /*
     * NOTE:
     * UPROPERTY does not support nested containers such as
     * TMap<Key, TArray<Value>>.  Storing tile templates in that shape caused
     * a reflection/serialization error during compilation.
     *
     * Instead, we store all templates in a flat array and filter by biome at
     * runtime (e.g., inside GetTileTemplatesForBiome or helper methods).
     */
    TArray<FTileTemplate> TileTemplates;

    // Generation settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
    int32 MaxLayoutSize; // Maximum number of tiles per layout

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
    float TileSpacing; // Spacing between tiles

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
    bool bUseAIGeneration; // Whether to use AI for descriptions

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    // Spawned actors
    UPROPERTY()
    TArray<AActor*> SpawnedTileActors;

    // Name generation data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name Generation")
    /*
     * Prefix pool used for procedural name generation.
     * NOTE: UPROPERTY reflection does not support nested containers
     * such as `TMap<Key, TArray<Value>>`. We therefore store all
     * possible prefixes in a flat array and filter / weight them for
     * a given biome at runtime inside the generator.
     */
    TArray<FString> PlanetNamePrefixes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name Generation")
    /*
     * Suffix pool used for procedural name generation.
     * These are likewise stored as a flat array for the same
     * serialization-compatibility reasons explained above.
     */
    TArray<FString> PlanetNameSuffixes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name Generation")
    TArray<FString> GenericPlanetNames;

private:
    // Helper methods
    void LoadDefaultTileTemplates();
    FTileTemplate SelectRandomTile(EPlanetBiome BiomeType, const FString& TileType, int32 Seed);
    TArray<FVector> GenerateLayoutGrid(int32 NumTiles, float Spacing);
    FString GenerateLayoutDescription(const FGeneratedLayout& Layout);
    void LoadNameGenerationData();
    FString GetRandomNameComponent(const TArray<FString>& Components, int32 Seed);

public:
    /**
     * Blueprint implementable events for custom generation logic
     */

    /**
     * Called when planet generation starts (for custom setup)
     * @param Params Generation parameters
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Generation Events")
    void OnPlanetGenerationStartedEvent(const FPlanetGenerationParams& Params);

    /**
     * Called when planet generation completes (for custom processing)
     * @param GeneratedPlanet The generated planet data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Generation Events")
    void OnPlanetGenerationCompletedEvent(const FPlanetData& GeneratedPlanet);

    /**
     * Called to generate custom planet description (override in Blueprint)
     * @param Params Generation parameters
     * @return Custom planet description
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Generation Events")
    FString GenerateCustomPlanetDescription(const FPlanetGenerationParams& Params);

    /**
     * Called to generate custom NPC (override in Blueprint)
     * @param PlanetData Planet context
     * @param NPCRole Role for the NPC
     * @param Seed Random seed
     * @return Custom generated NPC
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Generation Events")
    FNPCData GenerateCustomNPC(const FPlanetData& PlanetData, const FString& NPCRole, int32 Seed);

    /**
     * Called to spawn custom tile (override in Blueprint)
     * @param TileTemplate Template for the tile
     * @param SpawnTransform Transform to spawn at
     * @return Spawned tile actor
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Generation Events")
    AActor* SpawnCustomTile(const FTileTemplate& TileTemplate, const FTransform& SpawnTransform);
};
