// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIDM/CampaignLoaderSubsystem.h"
#include "Engine/Engine.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Paths.h"

void UCampaignLoaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    bCampaignLoaded = false;
    
    UE_LOG(LogTemp, Log, TEXT("CampaignLoaderSubsystem initialized"));
}

void UCampaignLoaderSubsystem::Deinitialize()
{
    // Clear cached data
    CachedNPCData.Empty();
    CachedEnemyData.Empty();
    bCampaignLoaded = false;
    
    Super::Deinitialize();
}

bool UCampaignLoaderSubsystem::LoadCampaign(const FString& JsonFilePath)
{
    // Construct full path
    FString FullPath = FPaths::ProjectContentDir() + JsonFilePath;
    
    // Check if file exists
    if (!FPaths::FileExists(FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Campaign file not found: %s"), *FullPath);
        return false;
    }
    
    // Load file content
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load campaign file: %s"), *FullPath);
        return false;
    }
    
    // Parse JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON from campaign file: %s"), *FullPath);
        return false;
    }
    
    // Parse campaign data
    if (!ParseCampaignFromJson(JsonObject))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse campaign data from: %s"), *FullPath);
        return false;
    }
    
    bCampaignLoaded = true;
    UE_LOG(LogTemp, Log, TEXT("Successfully loaded campaign: %s"), *CurrentCampaign.Config.StorySeed);
    
    return true;
}

FPlanetData UCampaignLoaderSubsystem::GetPlanetData(int32 PlanetIndex) const
{
    if (!bCampaignLoaded || PlanetIndex < 0 || PlanetIndex >= CurrentCampaign.Planets.Num())
    {
        return FPlanetData();
    }
    
    return CurrentCampaign.Planets[PlanetIndex];
}

TArray<FNPCData> UCampaignLoaderSubsystem::GetNPCsForLocation(int32 PlanetIndex, const FString& LayoutName) const
{
    FString LocationKey = FString::Printf(TEXT("%d_%s"), PlanetIndex, *LayoutName);
    
    if (const TArray<FNPCData>* FoundNPCs = CachedNPCData.Find(LocationKey))
    {
        return *FoundNPCs;
    }
    
    return TArray<FNPCData>();
}

TArray<FEnemyData> UCampaignLoaderSubsystem::GetEnemiesForPlanet(int32 PlanetIndex) const
{
    if (const TArray<FEnemyData>* FoundEnemies = CachedEnemyData.Find(PlanetIndex))
    {
        return *FoundEnemies;
    }
    
    return TArray<FEnemyData>();
}

bool UCampaignLoaderSubsystem::ParseCampaignFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
    // Parse campaign config
    const TSharedPtr<FJsonObject>* ConfigObject;
    if (!JsonObject->TryGetObjectField(TEXT("config"), ConfigObject) || !ConfigObject->IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Missing or invalid 'config' field in campaign JSON"));
        return false;
    }
    
    if (!ParseCampaignConfig(*ConfigObject, CurrentCampaign.Config))
    {
        return false;
    }
    
    // Parse planets
    const TArray<TSharedPtr<FJsonValue>>* PlanetsArray;
    if (!JsonObject->TryGetArrayField(TEXT("planets"), PlanetsArray))
    {
        UE_LOG(LogTemp, Error, TEXT("Missing 'planets' array in campaign JSON"));
        return false;
    }
    
    CurrentCampaign.Planets.Empty();
    for (const auto& PlanetValue : *PlanetsArray)
    {
        const TSharedPtr<FJsonObject>* PlanetObject = nullptr;
        if (PlanetValue->TryGetObject(PlanetObject) && PlanetObject->IsValid())
        {
            FPlanetData Planet;
            if (ParsePlanetData(*PlanetObject, Planet))
            {
                CurrentCampaign.Planets.Add(Planet);
            }
        }
    }
    
    // Parse final boss
    const TSharedPtr<FJsonObject>* BossObject;
    if (JsonObject->TryGetObjectField(TEXT("final_boss"), BossObject) && BossObject->IsValid())
    {
        ParseBossData(*BossObject, CurrentCampaign.FinalBoss);
    }
    
    // Parse main quest outline
    JsonObject->TryGetStringField(TEXT("main_quest_outline"), CurrentCampaign.MainQuestOutline);
    
    // Parse campaign summary
    JsonObject->TryGetStringField(TEXT("campaign_summary"), CurrentCampaign.CampaignSummary);
    
    UE_LOG(LogTemp, Log, TEXT("Parsed campaign with %d planets"), CurrentCampaign.Planets.Num());
    
    return true;
}

bool UCampaignLoaderSubsystem::ParseCampaignConfig(const TSharedPtr<FJsonObject>& JsonObject, FCampaignConfig& OutConfig)
{
    JsonObject->TryGetNumberField(TEXT("game_length_hours"), OutConfig.GameLengthHours);
    JsonObject->TryGetNumberField(TEXT("estimated_planets"), OutConfig.EstimatedPlanets);
    JsonObject->TryGetStringField(TEXT("story_seed"), OutConfig.StorySeed);
    JsonObject->TryGetStringField(TEXT("time_period"), OutConfig.TimePeriod);
    JsonObject->TryGetStringField(TEXT("alignment_focus"), OutConfig.AlignmentFocus);
    
    return true;
}

bool UCampaignLoaderSubsystem::ParsePlanetData(const TSharedPtr<FJsonObject>& JsonObject, FPlanetData& OutPlanet)
{
    JsonObject->TryGetStringField(TEXT("name"), OutPlanet.Name);
    JsonObject->TryGetStringField(TEXT("biome"), OutPlanet.Biome);
    JsonObject->TryGetStringField(TEXT("climate"), OutPlanet.Climate);
    JsonObject->TryGetStringField(TEXT("population"), OutPlanet.Population);
    JsonObject->TryGetStringField(TEXT("government"), OutPlanet.Government);
    JsonObject->TryGetStringField(TEXT("main_quest"), OutPlanet.MainQuest);
    JsonObject->TryGetStringField(TEXT("difficulty_tier"), OutPlanet.DifficultyTier);
    JsonObject->TryGetNumberField(TEXT("planet_index"), OutPlanet.PlanetIndex);
    JsonObject->TryGetStringField(TEXT("lore_description"), OutPlanet.LoreDescription);
    
    // Parse side quests array
    const TArray<TSharedPtr<FJsonValue>>* SideQuestsArray;
    if (JsonObject->TryGetArrayField(TEXT("side_quests"), SideQuestsArray))
    {
        OutPlanet.SideQuests = JsonArrayToStringArray(*SideQuestsArray);
    }
    
    // Parse layouts array
    const TArray<TSharedPtr<FJsonValue>>* LayoutsArray;
    if (JsonObject->TryGetArrayField(TEXT("layouts"), LayoutsArray))
    {
        OutPlanet.Layouts.Empty();
        for (const auto& LayoutValue : *LayoutsArray)
        {
            const TSharedPtr<FJsonObject>* LayoutObject = nullptr;
            if (LayoutValue->TryGetObject(LayoutObject) && LayoutObject->IsValid())
            {
                FMapLayout Layout;
                if (ParseMapLayout(*LayoutObject, Layout))
                {
                    OutPlanet.Layouts.Add(Layout);
                }
            }
        }
    }
    
    return true;
}

bool UCampaignLoaderSubsystem::ParseMapLayout(const TSharedPtr<FJsonObject>& JsonObject, FMapLayout& OutLayout)
{
    JsonObject->TryGetStringField(TEXT("name"), OutLayout.Name);
    JsonObject->TryGetStringField(TEXT("layout_type"), OutLayout.LayoutType);
    JsonObject->TryGetStringField(TEXT("description"), OutLayout.Description);
    
    double TimeHours;
    if (JsonObject->TryGetNumberField(TEXT("estimated_time_hours"), TimeHours))
    {
        OutLayout.EstimatedTimeHours = static_cast<float>(TimeHours);
    }
    
    // Parse key features array
    const TArray<TSharedPtr<FJsonValue>>* FeaturesArray;
    if (JsonObject->TryGetArrayField(TEXT("key_features"), FeaturesArray))
    {
        OutLayout.KeyFeatures = JsonArrayToStringArray(*FeaturesArray);
    }
    
    return true;
}

bool UCampaignLoaderSubsystem::ParseBossData(const TSharedPtr<FJsonObject>& JsonObject, FBossData& OutBoss)
{
    JsonObject->TryGetStringField(TEXT("name"), OutBoss.Name);
    JsonObject->TryGetStringField(TEXT("title"), OutBoss.Title);
    JsonObject->TryGetStringField(TEXT("species"), OutBoss.Species);
    JsonObject->TryGetStringField(TEXT("motivation"), OutBoss.Motivation);
    JsonObject->TryGetStringField(TEXT("backstory"), OutBoss.Backstory);
    JsonObject->TryGetStringField(TEXT("arena_description"), OutBoss.ArenaDescription);
    
    double ChallengeRating;
    if (JsonObject->TryGetNumberField(TEXT("challenge_rating"), ChallengeRating))
    {
        OutBoss.ChallengeRating = static_cast<float>(ChallengeRating);
    }
    
    JsonObject->TryGetNumberField(TEXT("hit_points"), OutBoss.HitPoints);
    JsonObject->TryGetNumberField(TEXT("armor_class"), OutBoss.ArmorClass);
    
    // Parse string arrays
    const TArray<TSharedPtr<FJsonValue>>* MechanicsArray;
    if (JsonObject->TryGetArrayField(TEXT("mechanics"), MechanicsArray))
    {
        OutBoss.Mechanics = JsonArrayToStringArray(*MechanicsArray);
    }
    
    const TArray<TSharedPtr<FJsonValue>>* AbilitiesArray;
    if (JsonObject->TryGetArrayField(TEXT("special_abilities"), AbilitiesArray))
    {
        OutBoss.SpecialAbilities = JsonArrayToStringArray(*AbilitiesArray);
    }
    
    const TArray<TSharedPtr<FJsonValue>>* WeaknessesArray;
    if (JsonObject->TryGetArrayField(TEXT("weaknesses"), WeaknessesArray))
    {
        OutBoss.Weaknesses = JsonArrayToStringArray(*WeaknessesArray);
    }
    
    const TArray<TSharedPtr<FJsonValue>>* MinionsArray;
    if (JsonObject->TryGetArrayField(TEXT("minions"), MinionsArray))
    {
        OutBoss.Minions = JsonArrayToStringArray(*MinionsArray);
    }
    
    // Parse loot drop
    const TSharedPtr<FJsonObject>* LootObject;
    if (JsonObject->TryGetObjectField(TEXT("loot_drop"), LootObject) && LootObject->IsValid())
    {
        ParseLootItem(*LootObject, OutBoss.LootDrop);
    }
    
    return true;
}

bool UCampaignLoaderSubsystem::ParseLootItem(const TSharedPtr<FJsonObject>& JsonObject, FLootItem& OutItem)
{
    JsonObject->TryGetStringField(TEXT("name"), OutItem.Name);
    JsonObject->TryGetStringField(TEXT("rarity"), OutItem.Rarity);
    JsonObject->TryGetStringField(TEXT("item_type"), OutItem.ItemType);
    JsonObject->TryGetStringField(TEXT("flavor_text"), OutItem.FlavorText);
    JsonObject->TryGetStringField(TEXT("faction_theme"), OutItem.FactionTheme);
    JsonObject->TryGetNumberField(TEXT("value_credits"), OutItem.ValueCredits);
    JsonObject->TryGetNumberField(TEXT("level_requirement"), OutItem.LevelRequirement);
    
    // Parse bonuses map
    const TSharedPtr<FJsonObject>* BonusesObject;
    if (JsonObject->TryGetObjectField(TEXT("bonuses"), BonusesObject) && BonusesObject->IsValid())
    {
        OutItem.Bonuses = JsonObjectToIntMap(*BonusesObject);
    }
    
    return true;
}

TArray<FString> UCampaignLoaderSubsystem::JsonArrayToStringArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
    TArray<FString> Result;
    
    for (const auto& Value : JsonArray)
    {
        FString StringValue;
        if (Value->TryGetString(StringValue))
        {
            Result.Add(StringValue);
        }
    }
    
    return Result;
}

TMap<FString, int32> UCampaignLoaderSubsystem::JsonObjectToIntMap(const TSharedPtr<FJsonObject>& JsonObject)
{
    TMap<FString, int32> Result;
    
    for (const auto& Pair : JsonObject->Values)
    {
        double NumberValue;
        if (Pair.Value->TryGetNumber(NumberValue))
        {
            Result.Add(Pair.Key, static_cast<int32>(NumberValue));
        }
    }
    
    return Result;
}

bool UCampaignLoaderSubsystem::ParseNPCData(const TSharedPtr<FJsonObject>& JsonObject, FNPCData& OutNPC)
{
    JsonObject->TryGetStringField(TEXT("name"), OutNPC.Name);
    JsonObject->TryGetStringField(TEXT("species"), OutNPC.Species);
    JsonObject->TryGetStringField(TEXT("faction"), OutNPC.Faction);
    JsonObject->TryGetStringField(TEXT("alignment"), OutNPC.Alignment);
    JsonObject->TryGetStringField(TEXT("role"), OutNPC.Role);
    JsonObject->TryGetStringField(TEXT("backstory"), OutNPC.Backstory);
    JsonObject->TryGetStringField(TEXT("location"), OutNPC.Location);
    JsonObject->TryGetStringField(TEXT("dialogue_style"), OutNPC.DialogueStyle);
    JsonObject->TryGetBoolField(TEXT("likes_player"), OutNPC.bLikesPlayer);
    JsonObject->TryGetNumberField(TEXT("reputation_standing"), OutNPC.ReputationStanding);

    // Parse personality traits array
    const TArray<TSharedPtr<FJsonValue>>* TraitsArray;
    if (JsonObject->TryGetArrayField(TEXT("personality_traits"), TraitsArray))
    {
        OutNPC.PersonalityTraits = JsonArrayToStringArray(*TraitsArray);
    }

    // Parse quest data if present
    const TSharedPtr<FJsonObject>* QuestObject;
    if (JsonObject->TryGetObjectField(TEXT("quest"), QuestObject) && QuestObject->IsValid())
    {
        ParseQuestData(*QuestObject, OutNPC.Quest);
    }

    return true;
}

bool UCampaignLoaderSubsystem::ParseEnemyData(const TSharedPtr<FJsonObject>& JsonObject, FEnemyData& OutEnemy)
{
    JsonObject->TryGetStringField(TEXT("name"), OutEnemy.Name);
    JsonObject->TryGetStringField(TEXT("species"), OutEnemy.Species);
    JsonObject->TryGetStringField(TEXT("faction"), OutEnemy.Faction);
    JsonObject->TryGetStringField(TEXT("description"), OutEnemy.Description);
    JsonObject->TryGetNumberField(TEXT("hit_points"), OutEnemy.HitPoints);
    JsonObject->TryGetNumberField(TEXT("armor_class"), OutEnemy.ArmorClass);

    double ChallengeRating;
    if (JsonObject->TryGetNumberField(TEXT("cr_rating"), ChallengeRating))
    {
        OutEnemy.ChallengeRating = static_cast<float>(ChallengeRating);
    }

    // Parse abilities array
    const TArray<TSharedPtr<FJsonValue>>* AbilitiesArray;
    if (JsonObject->TryGetArrayField(TEXT("abilities"), AbilitiesArray))
    {
        OutEnemy.Abilities = JsonArrayToStringArray(*AbilitiesArray);
    }

    // Parse loot table array
    const TArray<TSharedPtr<FJsonValue>>* LootArray;
    if (JsonObject->TryGetArrayField(TEXT("loot_table"), LootArray))
    {
        OutEnemy.LootTable = JsonArrayToStringArray(*LootArray);
    }

    // Parse biome preference array
    const TArray<TSharedPtr<FJsonValue>>* BiomeArray;
    if (JsonObject->TryGetArrayField(TEXT("biome_preference"), BiomeArray))
    {
        OutEnemy.BiomePreference = JsonArrayToStringArray(*BiomeArray);
    }

    return true;
}

bool UCampaignLoaderSubsystem::ParseQuestData(const TSharedPtr<FJsonObject>& JsonObject, FQuestData& OutQuest)
{
    JsonObject->TryGetStringField(TEXT("title"), OutQuest.Title);
    JsonObject->TryGetStringField(TEXT("description"), OutQuest.Description);
    JsonObject->TryGetStringField(TEXT("quest_type"), OutQuest.QuestType);
    JsonObject->TryGetStringField(TEXT("reward_type"), OutQuest.RewardType);
    JsonObject->TryGetStringField(TEXT("difficulty"), OutQuest.Difficulty);
    JsonObject->TryGetNumberField(TEXT("estimated_time_minutes"), OutQuest.EstimatedTimeMinutes);
    
    return true;
}
