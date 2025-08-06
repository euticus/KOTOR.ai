// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Companions/CompanionManagerComponent.h"
#include "LLMCharacterCreator.generated.h"

/**
 * Character class types
 */
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
    Jedi            UMETA(DisplayName = "Jedi"),
    Sith            UMETA(DisplayName = "Sith"),
    Soldier         UMETA(DisplayName = "Soldier"),
    Scoundrel       UMETA(DisplayName = "Scoundrel"),
    Scout           UMETA(DisplayName = "Scout"),
    Noble           UMETA(DisplayName = "Noble"),
    Technician      UMETA(DisplayName = "Technician"),
    Pilot           UMETA(DisplayName = "Pilot"),
    Medic           UMETA(DisplayName = "Medic"),
    Custom          UMETA(DisplayName = "Custom")
};

/**
 * Character background types
 */
UENUM(BlueprintType)
enum class ECharacterBackground : uint8
{
    Military        UMETA(DisplayName = "Military"),
    Criminal        UMETA(DisplayName = "Criminal"),
    Noble           UMETA(DisplayName = "Noble"),
    Merchant        UMETA(DisplayName = "Merchant"),
    Scholar         UMETA(DisplayName = "Scholar"),
    Exile           UMETA(DisplayName = "Exile"),
    Orphan          UMETA(DisplayName = "Orphan"),
    Survivor        UMETA(DisplayName = "Survivor"),
    Mystic          UMETA(DisplayName = "Mystic"),
    Custom          UMETA(DisplayName = "Custom")
};

/**
 * Generated character stats
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGeneratedCharacterStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Character Stats")
    int32 HitPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Character Stats")
    int32 ArmorClass;

    UPROPERTY(BlueprintReadWrite, Category = "Character Stats")
    int32 ForceAffinity; // 0-100, higher = more Force sensitive

    UPROPERTY(BlueprintReadWrite, Category = "Character Stats")
    TMap<FString, int32> Skills; // Skill name -> level

    UPROPERTY(BlueprintReadWrite, Category = "Character Stats")
    TArray<FString> Abilities; // Special abilities

    UPROPERTY(BlueprintReadWrite, Category = "Character Stats")
    TArray<FString> Weaknesses; // Character flaws/weaknesses

    FGeneratedCharacterStats()
    {
        HitPoints = 100;
        ArmorClass = 10;
        ForceAffinity = 50;
    }
};

/**
 * Character creation request
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCharacterCreationRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Character Creation")
    FString CharacterName;

    UPROPERTY(BlueprintReadWrite, Category = "Character Creation")
    FString NaturalLanguageDescription; // Player's description

    UPROPERTY(BlueprintReadWrite, Category = "Character Creation")
    ECharacterClass PreferredClass;

    UPROPERTY(BlueprintReadWrite, Category = "Character Creation")
    ECharacterBackground PreferredBackground;

    UPROPERTY(BlueprintReadWrite, Category = "Character Creation")
    FString PreferredAlignment; // "light", "dark", "neutral"

    UPROPERTY(BlueprintReadWrite, Category = "Character Creation")
    TArray<FString> KeywordTags; // Additional tags from description

    FCharacterCreationRequest()
    {
        CharacterName = TEXT("Unnamed Character");
        NaturalLanguageDescription = TEXT("");
        PreferredClass = ECharacterClass::Custom;
        PreferredBackground = ECharacterBackground::Custom;
        PreferredAlignment = TEXT("neutral");
    }
};

/**
 * Generated character profile
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGeneratedCharacterProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString CharacterID;

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString CharacterName;

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    ECharacterClass CharacterClass;

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    ECharacterBackground Background;

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString Alignment;

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString Backstory; // LLM-generated backstory

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString Personality; // Personality traits

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString Motivation; // Primary motivation

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FString OriginPlanet; // Home planet

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FGeneratedCharacterStats Stats;

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    TArray<FLootItem> StartingLoot; // Starting equipment

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    FCompanionData RivalOrBondedNPC; // Generated rival/ally

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    TArray<FString> PersonalGoals; // Personal quest hooks

    UPROPERTY(BlueprintReadWrite, Category = "Character Profile")
    TMap<FString, FString> BackstoryHooks; // NPC connection hooks

    FGeneratedCharacterProfile()
    {
        CharacterID = TEXT("");
        CharacterName = TEXT("Unnamed Character");
        CharacterClass = ECharacterClass::Custom;
        Background = ECharacterBackground::Custom;
        Alignment = TEXT("neutral");
        Backstory = TEXT("");
        Personality = TEXT("");
        Motivation = TEXT("");
        OriginPlanet = TEXT("");
    }
};

/**
 * Character creation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCreationStarted, const FCharacterCreationRequest&, Request);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCreationCompleted, const FGeneratedCharacterProfile&, Profile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterCreationFailed, const FString&, Reason, const FCharacterCreationRequest&, Request);

/**
 * LLM Character Creator - Creates characters from natural language descriptions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API ULLMCharacterCreator : public UActorComponent
{
    GENERATED_BODY()

public:
    ULLMCharacterCreator();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize character creator
     * @param CampaignLoader Campaign loader for world context
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    void InitializeCharacterCreator(UCampaignLoaderSubsystem* CampaignLoader);

    /**
     * Create character from natural language description
     * @param Request Character creation request
     * @return Generated character profile
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FGeneratedCharacterProfile CreateCharacterFromDescription(const FCharacterCreationRequest& Request);

    /**
     * Generate character stats based on class and background
     * @param CharacterClass Character class
     * @param Background Character background
     * @param Description Natural language description
     * @return Generated stats
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FGeneratedCharacterStats GenerateCharacterStats(ECharacterClass CharacterClass, 
                                                   ECharacterBackground Background, 
                                                   const FString& Description);

    /**
     * Generate starting loot for character
     * @param CharacterClass Character class
     * @param Background Character background
     * @param Alignment Character alignment
     * @return Array of starting items
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FLootItem> GenerateStartingLoot(ECharacterClass CharacterClass, 
                                          ECharacterBackground Background, 
                                          const FString& Alignment);

    /**
     * Generate rival or bonded NPC
     * @param CharacterProfile Player character profile
     * @return Generated companion/rival NPC
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FCompanionData GenerateRivalOrBondedNPC(const FGeneratedCharacterProfile& CharacterProfile);

    /**
     * Generate personal quest hooks
     * @param CharacterProfile Character profile
     * @param NumQuests Number of quest hooks to generate
     * @return Array of quest hook descriptions
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TArray<FString> GeneratePersonalQuestHooks(const FGeneratedCharacterProfile& CharacterProfile, int32 NumQuests = 2);

    /**
     * Parse natural language description
     * @param Description Player's description
     * @return Extracted keywords and traits
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TMap<FString, FString> ParseNaturalLanguageDescription(const FString& Description);

    /**
     * Get character class suggestions
     * @param Description Natural language description
     * @return Array of suggested classes with confidence scores
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TMap<ECharacterClass, float> GetClassSuggestions(const FString& Description);

    /**
     * Get background suggestions
     * @param Description Natural language description
     * @return Array of suggested backgrounds with confidence scores
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    TMap<ECharacterBackground, float> GetBackgroundSuggestions(const FString& Description);

    /**
     * Export character profile to JSON
     * @param Profile Character profile to export
     * @return JSON string
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FString ExportCharacterProfile(const FGeneratedCharacterProfile& Profile);

    /**
     * Import character profile from JSON
     * @param ProfileJSON JSON string
     * @return Imported character profile
     */
    UFUNCTION(BlueprintCallable, Category = "Character Creator")
    FGeneratedCharacterProfile ImportCharacterProfile(const FString& ProfileJSON);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Character Creation Events")
    FOnCharacterCreationStarted OnCharacterCreationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Character Creation Events")
    FOnCharacterCreationCompleted OnCharacterCreationCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Character Creation Events")
    FOnCharacterCreationFailed OnCharacterCreationFailed;

protected:
    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    // Character creation templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Templates")
    TMap<ECharacterClass, FGeneratedCharacterStats> ClassStatTemplates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Templates")
    TMap<ECharacterBackground, TArray<FString>> BackgroundTraits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Templates")
    TMap<ECharacterClass, TArray<FLootItem>> ClassStartingGear;

    // LLM prompts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Prompts")
    FString BackstoryPromptTemplate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Prompts")
    FString PersonalityPromptTemplate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Prompts")
    FString QuestHookPromptTemplate;

    // Name generation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name Generation")
    TMap<FString, TArray<FString>> SpeciesNamePools;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name Generation")
    TArray<FString> PlanetNames;

private:
    // Helper methods
    void LoadCharacterTemplates();
    void LoadLLMPrompts();
    ECharacterClass DetermineOptimalClass(const FString& Description);
    ECharacterBackground DetermineOptimalBackground(const FString& Description);
    FString GenerateBackstory(const FCharacterCreationRequest& Request);
    FString GeneratePersonality(const FCharacterCreationRequest& Request);
    FString GenerateMotivation(const FCharacterCreationRequest& Request);
    FString SelectOriginPlanet(const FCharacterCreationRequest& Request);
    TArray<FString> ExtractKeywords(const FString& Description);
    FString GenerateCharacterID();

public:
    /**
     * Blueprint implementable events for custom character creation
     */

    /**
     * Called when character creation starts (for custom setup)
     * @param Request The character creation request
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Character Creation Events")
    void OnCharacterCreationStartedEvent(const FCharacterCreationRequest& Request);

    /**
     * Called when character creation completes (for custom processing)
     * @param Profile The generated character profile
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Character Creation Events")
    void OnCharacterCreationCompletedEvent(const FGeneratedCharacterProfile& Profile);

    /**
     * Called to generate custom backstory (override in Blueprint)
     * @param Request Character creation request
     * @return Custom backstory
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Character Creation Events")
    FString GenerateCustomBackstory(const FCharacterCreationRequest& Request);

    /**
     * Called to generate custom stats (override in Blueprint)
     * @param Request Character creation request
     * @return Custom character stats
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Character Creation Events")
    FGeneratedCharacterStats GenerateCustomStats(const FCharacterCreationRequest& Request);
};
