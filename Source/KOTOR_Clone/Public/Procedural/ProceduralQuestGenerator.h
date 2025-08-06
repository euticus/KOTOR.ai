// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/QuestManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "ProceduralQuestGenerator.generated.h"

/**
 * Procedural quest types
 */
UENUM(BlueprintType)
enum class EProceduralQuestType : uint8
{
    Fetch           UMETA(DisplayName = "Fetch Quest"),
    Kill            UMETA(DisplayName = "Kill Target"),
    Escort          UMETA(DisplayName = "Escort Mission"),
    Delivery        UMETA(DisplayName = "Delivery"),
    Investigation   UMETA(DisplayName = "Investigation"),
    Diplomacy       UMETA(DisplayName = "Diplomacy"),
    Rescue          UMETA(DisplayName = "Rescue Mission"),
    Exploration     UMETA(DisplayName = "Exploration"),
    Collection      UMETA(DisplayName = "Collection"),
    Survival        UMETA(DisplayName = "Survival"),
    Mystery         UMETA(DisplayName = "Mystery"),
    Betrayal        UMETA(DisplayName = "Betrayal"),
    Romance         UMETA(DisplayName = "Romance"),
    Revenge         UMETA(DisplayName = "Revenge"),
    Custom          UMETA(DisplayName = "Custom Quest")
};

/**
 * Quest generation parameters
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FQuestGenerationParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    EProceduralQuestType QuestType;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    FString PlanetName; // Planet where quest takes place

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    FString LayoutName; // Specific layout for quest

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    int32 PlayerLevel; // Player level for scaling

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    FString DifficultyTier; // "easy", "medium", "hard"

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    int32 EstimatedDuration; // Minutes

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    TArray<FString> RequiredFactions; // Factions that should be involved

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    TArray<FString> AvailableNPCs; // NPCs that can be used

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    TArray<FString> ContextTags; // Tags for context (e.g., "urgent", "personal", "political")

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    bool bUseNarrativeContext; // Whether to use player's narrative history

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    bool bAllowCompanionInvolvement; // Whether companions can be involved

    UPROPERTY(BlueprintReadWrite, Category = "Quest Generation")
    int32 Seed; // Random seed for generation

    FQuestGenerationParams()
    {
        QuestType = EProceduralQuestType::Fetch;
        PlanetName = TEXT("");
        LayoutName = TEXT("");
        PlayerLevel = 1;
        DifficultyTier = TEXT("medium");
        EstimatedDuration = 30;
        bUseNarrativeContext = true;
        bAllowCompanionInvolvement = true;
        Seed = 0;
    }
};

/**
 * Quest template for generation
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FQuestTemplate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    FString TemplateID;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    EProceduralQuestType QuestType;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    FString TitleTemplate; // Template with placeholders like {target}, {location}

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    FString DescriptionTemplate; // Description template

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    TArray<FString> ObjectiveTemplates; // Objective templates

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    TMap<FString, TArray<FString>> VariableOptions; // Variable -> Possible values

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    TMap<FString, int32> RewardRanges; // Reward type -> amount range

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    float GenerationWeight; // Probability of this template being selected

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    TArray<FString> RequiredTags; // Tags required for this template

    UPROPERTY(BlueprintReadWrite, Category = "Quest Template")
    TArray<FString> ConflictingTags; // Tags that prevent this template

    FQuestTemplate()
    {
        TemplateID = TEXT("");
        QuestType = EProceduralQuestType::Fetch;
        TitleTemplate = TEXT("Retrieve {item} from {location}");
        DescriptionTemplate = TEXT("A {npc} needs you to retrieve {item} from {location}.");
        GenerationWeight = 1.0f;
    }
};

/**
 * Infinite mode settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FInfiniteModeSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    bool bEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    int32 MaxActiveQuests; // Maximum number of active procedural quests

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    float QuestGenerationInterval; // Seconds between new quest generation

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    TArray<EProceduralQuestType> AllowedQuestTypes; // Quest types to generate

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    float DifficultyScaling; // How much to scale difficulty over time

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    bool bAdaptToPlayerChoices; // Whether to adapt based on player preferences

    FInfiniteModeSettings()
    {
        bEnabled = false;
        MaxActiveQuests = 5;
        QuestGenerationInterval = 300.0f; // 5 minutes
        DifficultyScaling = 1.1f;
        bAdaptToPlayerChoices = true;
    }
};

/**
 * Quest generation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestGenerated, const FQuestData&, GeneratedQuest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestGenerationFailed, EProceduralQuestType, QuestType, const FString&, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInfiniteModeToggled, bool, bEnabled);

/**
 * Procedural Quest Generator - Generates quests dynamically based on context
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UProceduralQuestGenerator : public UActorComponent
{
    GENERATED_BODY()

public:
    UProceduralQuestGenerator();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize procedural quest generator
     * @param CampaignLoader Campaign loader for content data
     * @param QuestManager Quest manager for quest integration
     * @param NarrativeMemory Narrative memory for context
     * @param CompanionManager Companion manager for companion involvement
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    void InitializeQuestGenerator(UCampaignLoaderSubsystem* CampaignLoader,
                                 UQuestManagerComponent* QuestManager,
                                 UNarrativeMemoryComponent* NarrativeMemory,
                                 UCompanionManagerComponent* CompanionManager);

    /**
     * Generate a quest based on parameters
     * @param GenerationParams Parameters for quest generation
     * @return Generated quest data
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    FQuestData GenerateQuest(const FQuestGenerationParams& GenerationParams);

    /**
     * Generate random quest for current context
     * @param QuestType Type of quest to generate (optional)
     * @return Generated quest data
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    FQuestData GenerateRandomQuest(EProceduralQuestType QuestType = EProceduralQuestType::Custom);

    /**
     * Generate quest chain (multiple related quests)
     * @param ChainLength Number of quests in chain
     * @param Theme Theme connecting the quests
     * @return Array of generated quests
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    TArray<FQuestData> GenerateQuestChain(int32 ChainLength, const FString& Theme);

    /**
     * Enable/disable infinite mode
     * @param bEnable Whether to enable infinite mode
     * @param Settings Infinite mode settings
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    void SetInfiniteModeEnabled(bool bEnable, const FInfiniteModeSettings& Settings);

    /**
     * Add custom quest template
     * @param Template Quest template to add
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    void AddQuestTemplate(const FQuestTemplate& Template);

    /**
     * Remove quest template
     * @param TemplateID ID of template to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    void RemoveQuestTemplate(const FString& TemplateID);

    /**
     * Get available quest templates
     * @param QuestType Type of quest templates to get
     * @return Array of available templates
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Quests")
    TArray<FQuestTemplate> GetQuestTemplates(EProceduralQuestType QuestType) const;

    /**
     * Analyze player preferences
     * @return Map of quest type preferences (type -> preference score)
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    TMap<FString, float> AnalyzePlayerPreferences() const;

    /**
     * Get quest generation statistics
     * @return Map of generation statistics
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Quests")
    TMap<FString, int32> GetGenerationStatistics() const;

    /**
     * Check if infinite mode is enabled
     * @return True if infinite mode is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Quests")
    bool IsInfiniteModeEnabled() const { return InfiniteModeSettings.bEnabled; }

    /**
     * Get infinite mode settings
     * @return Current infinite mode settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Quests")
    FInfiniteModeSettings GetInfiniteModeSettings() const { return InfiniteModeSettings; }

    /**
     * Force generate new quest in infinite mode
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural Quests")
    void ForceGenerateInfiniteQuest();

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Quest Generation Events")
    FOnQuestGenerated OnQuestGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Quest Generation Events")
    FOnQuestGenerationFailed OnQuestGenerationFailed;

    UPROPERTY(BlueprintAssignable, Category = "Quest Generation Events")
    FOnInfiniteModeToggled OnInfiniteModeToggled;

protected:
    // Quest templates
    UPROPERTY(BlueprintReadOnly, Category = "Procedural Quests")
    TMap<EProceduralQuestType, TArray<FQuestTemplate>> QuestTemplates;

    // Infinite mode
    UPROPERTY(BlueprintReadOnly, Category = "Procedural Quests")
    FInfiniteModeSettings InfiniteModeSettings;

    UPROPERTY(BlueprintReadOnly, Category = "Procedural Quests")
    TArray<FString> ActiveProceduralQuests;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    // Generation statistics
    UPROPERTY()
    TMap<EProceduralQuestType, int32> GenerationCounts;

    UPROPERTY()
    TMap<EProceduralQuestType, int32> CompletionCounts;

    // Timer handles
    FTimerHandle InfiniteQuestTimer;

private:
    // Helper methods
    void LoadDefaultQuestTemplates();
    FQuestTemplate SelectQuestTemplate(const FQuestGenerationParams& Params);
    FQuestData ProcessQuestTemplate(const FQuestTemplate& Template, const FQuestGenerationParams& Params);
    TMap<FString, FString> GenerateQuestVariables(const FQuestTemplate& Template, const FQuestGenerationParams& Params);
    FString ProcessTemplate(const FString& Template, const TMap<FString, FString>& Variables);
    TArray<FQuestObjective> GenerateObjectives(const FQuestTemplate& Template, const TMap<FString, FString>& Variables);
    TMap<FString, int32> GenerateRewards(const FQuestTemplate& Template, const FQuestGenerationParams& Params);
    void GenerateInfiniteQuest();
    FQuestGenerationParams CreateContextualParams();
    bool ValidateQuestGeneration(const FQuestData& Quest);

    // Context analysis
    FString GetCurrentPlanet() const;
    FString GetCurrentLayout() const;
    TArray<FString> GetAvailableNPCs() const;
    TArray<FString> GetRelevantFactions() const;
    float GetPlayerAlignmentBias() const;

public:
    /**
     * Blueprint implementable events for custom quest generation
     */

    /**
     * Called when quest is generated (for custom processing)
     * @param GeneratedQuest The generated quest
     * @param GenerationParams The parameters used for generation
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Generation Events")
    void OnQuestGeneratedEvent(const FQuestData& GeneratedQuest, const FQuestGenerationParams& GenerationParams);

    /**
     * Called to generate custom quest variables (override in Blueprint)
     * @param Template The quest template
     * @param Params Generation parameters
     * @return Custom quest variables
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Generation Events")
    TMap<FString, FString> GenerateCustomQuestVariables(const FQuestTemplate& Template, const FQuestGenerationParams& Params);

    /**
     * Called to validate custom quest (override in Blueprint)
     * @param Quest The generated quest
     * @param Params Generation parameters
     * @return True if quest is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Generation Events")
    bool ValidateCustomQuest(const FQuestData& Quest, const FQuestGenerationParams& Params);

    /**
     * Called to get custom generation parameters (override in Blueprint)
     * @return Custom generation parameters for infinite mode
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Generation Events")
    FQuestGenerationParams GetCustomGenerationParams();
};
