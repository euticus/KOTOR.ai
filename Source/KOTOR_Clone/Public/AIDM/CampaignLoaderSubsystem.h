// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Dom/JsonObject.h"
#include "CampaignLoaderSubsystem.generated.h"

// Forward declarations
class UDataTable;
struct FJsonObject;

/**
 * Data structures for AIDM campaign data
 */

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMapLayout
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Layout")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Layout")
    FString LayoutType;

    UPROPERTY(BlueprintReadOnly, Category = "Layout")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Layout")
    float EstimatedTimeHours;

    UPROPERTY(BlueprintReadOnly, Category = "Layout")
    TArray<FString> KeyFeatures;

    FMapLayout()
    {
        Name = TEXT("");
        LayoutType = TEXT("");
        Description = TEXT("");
        EstimatedTimeHours = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlanetData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString Biome;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString Climate;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString Population;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString Government;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    TArray<FMapLayout> Layouts;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString MainQuest;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    TArray<FString> SideQuests;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString DifficultyTier;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    int32 PlanetIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Planet")
    FString LoreDescription;

    FPlanetData()
    {
        Name = TEXT("");
        Biome = TEXT("");
        Climate = TEXT("");
        Population = TEXT("");
        Government = TEXT("");
        MainQuest = TEXT("");
        DifficultyTier = TEXT("");
        PlanetIndex = 0;
        LoreDescription = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FQuestData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString Title;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString QuestType;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString RewardType;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString Difficulty;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    int32 EstimatedTimeMinutes;

    FQuestData()
    {
        Title = TEXT("");
        Description = TEXT("");
        QuestType = TEXT("");
        RewardType = TEXT("");
        Difficulty = TEXT("");
        EstimatedTimeMinutes = 0;
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNPCData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Species;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Faction;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Alignment;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    bool bLikesPlayer;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Role;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Backstory;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    TArray<FString> PersonalityTraits;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString Location;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FQuestData Quest;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    FString DialogueStyle;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    int32 ReputationStanding;

    FNPCData()
    {
        Name = TEXT("");
        Species = TEXT("");
        Faction = TEXT("");
        Alignment = TEXT("");
        bLikesPlayer = false;
        Role = TEXT("");
        Backstory = TEXT("");
        Location = TEXT("");
        DialogueStyle = TEXT("");
        ReputationStanding = 0;
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCampaignEnemyData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    FString Species;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    float ChallengeRating;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    int32 HitPoints;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    int32 ArmorClass;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    TArray<FString> Abilities;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    TArray<FString> LootTable;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    TArray<FString> BiomePreference;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    FString Faction;

    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
    FString Description;

    FEnemyData()
    {
        Name = TEXT("");
        Species = TEXT("");
        ChallengeRating = 0.0f;
        HitPoints = 0;
        ArmorClass = 0;
        Faction = TEXT("");
        Description = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FLootItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    FString Rarity;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    FString ItemType;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    TMap<FString, int32> Bonuses;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    FString FlavorText;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    int32 ValueCredits;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    int32 LevelRequirement;

    UPROPERTY(BlueprintReadOnly, Category = "Loot")
    FString FactionTheme;

    FLootItem()
    {
        Name = TEXT("");
        Rarity = TEXT("");
        ItemType = TEXT("");
        FlavorText = TEXT("");
        ValueCredits = 0;
        LevelRequirement = 0;
        FactionTheme = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FBossData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FString Title;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FString Species;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FString Motivation;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FString Backstory;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    TArray<FString> Mechanics;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FString ArenaDescription;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    FLootItem LootDrop;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    float ChallengeRating;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    int32 HitPoints;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    int32 ArmorClass;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    TArray<FString> SpecialAbilities;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    TArray<FString> Weaknesses;

    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    TArray<FString> Minions;

    FBossData()
    {
        Name = TEXT("");
        Title = TEXT("");
        Species = TEXT("");
        Motivation = TEXT("");
        Backstory = TEXT("");
        ArenaDescription = TEXT("");
        ChallengeRating = 0.0f;
        HitPoints = 0;
        ArmorClass = 0;
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCampaignConfig
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Config")
    int32 GameLengthHours;

    UPROPERTY(BlueprintReadOnly, Category = "Config")
    int32 EstimatedPlanets;

    UPROPERTY(BlueprintReadOnly, Category = "Config")
    FString StorySeed;

    UPROPERTY(BlueprintReadOnly, Category = "Config")
    FString TimePeriod;

    UPROPERTY(BlueprintReadOnly, Category = "Config")
    FString AlignmentFocus;

    FCampaignConfig()
    {
        GameLengthHours = 0;
        EstimatedPlanets = 0;
        StorySeed = TEXT("");
        TimePeriod = TEXT("");
        AlignmentFocus = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCampaignPlan
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Campaign")
    FCampaignConfig Config;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign")
    TArray<FPlanetData> Planets;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign")
    FBossData FinalBoss;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign")
    FString MainQuestOutline;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign")
    FString CampaignSummary;

    FCampaignPlan()
    {
        MainQuestOutline = TEXT("");
        CampaignSummary = TEXT("");
    }
};

/**
 * Subsystem for loading and managing AIDM campaign data
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCampaignLoaderSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Load a campaign from JSON file
     * @param JsonFilePath Path to the JSON file (relative to Content directory)
     * @return True if campaign was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    bool LoadCampaign(const FString& JsonFilePath);

    /**
     * Get the currently loaded campaign data
     * @return The loaded campaign plan
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM")
    const FCampaignPlan& GetCurrentCampaign() const { return CurrentCampaign; }

    /**
     * Check if a campaign is currently loaded
     * @return True if a campaign is loaded
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM")
    bool IsCampaignLoaded() const { return bCampaignLoaded; }

    /**
     * Get planet data by index
     * @param PlanetIndex Index of the planet
     * @return Planet data if valid index, empty struct otherwise
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM")
    FPlanetData GetPlanetData(int32 PlanetIndex) const;

    /**
     * Get NPCs for a specific planet and layout
     * @param PlanetIndex Index of the planet
     * @param LayoutName Name of the layout
     * @return Array of NPCs for that location
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    TArray<FNPCData> GetNPCsForLocation(int32 PlanetIndex, const FString& LayoutName) const;

    /**
     * Get enemies for a specific planet
     * @param PlanetIndex Index of the planet
     * @return Array of enemies for that planet
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    TArray<FEnemyData> GetEnemiesForPlanet(int32 PlanetIndex) const;

protected:
    // Current loaded campaign
    UPROPERTY(BlueprintReadOnly, Category = "AIDM")
    FCampaignPlan CurrentCampaign;

    // Campaign loaded state
    UPROPERTY(BlueprintReadOnly, Category = "AIDM")
    bool bCampaignLoaded;

    // Cached NPC data by location
    UPROPERTY()
    TMap<FString, TArray<FNPCData>> CachedNPCData;

    // Cached enemy data by planet
    UPROPERTY()
    TMap<int32, TArray<FEnemyData>> CachedEnemyData;

private:
    // JSON parsing helpers
    bool ParseCampaignFromJson(const TSharedPtr<FJsonObject>& JsonObject);
    bool ParsePlanetData(const TSharedPtr<FJsonObject>& JsonObject, FPlanetData& OutPlanet);
    bool ParseNPCData(const TSharedPtr<FJsonObject>& JsonObject, FNPCData& OutNPC);
    bool ParseEnemyData(const TSharedPtr<FJsonObject>& JsonObject, FEnemyData& OutEnemy);
    bool ParseLootItem(const TSharedPtr<FJsonObject>& JsonObject, FLootItem& OutItem);
    bool ParseBossData(const TSharedPtr<FJsonObject>& JsonObject, FBossData& OutBoss);
    bool ParseQuestData(const TSharedPtr<FJsonObject>& JsonObject, FQuestData& OutQuest);
    bool ParseMapLayout(const TSharedPtr<FJsonObject>& JsonObject, FMapLayout& OutLayout);
    bool ParseCampaignConfig(const TSharedPtr<FJsonObject>& JsonObject, FCampaignConfig& OutConfig);

    // Helper to convert JSON array to TArray<FString>
    TArray<FString> JsonArrayToStringArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray);
    
    // Helper to convert JSON object to TMap<FString, int32>
    TMap<FString, int32> JsonObjectToIntMap(const TSharedPtr<FJsonObject>& JsonObject);
};
