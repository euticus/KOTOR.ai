// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/QuestManagerComponent.h"
#include "Procedural/ProceduralPlanetGenerator.h"
#include "Procedural/ProceduralQuestGenerator.h"
#include "Timeline/CampaignTimelineComponent.h"
#include "GalaxyCampaignExpander.generated.h"

/**
 * Expansion trigger types
 */
UENUM(BlueprintType)
enum class EExpansionTrigger : uint8
{
    MainQuestComplete   UMETA(DisplayName = "Main Quest Complete"),
    PlanetExplored      UMETA(DisplayName = "Planet Fully Explored"),
    TimeElapsed         UMETA(DisplayName = "Time Elapsed"),
    PlayerLevel         UMETA(DisplayName = "Player Level Reached"),
    StoryMilestone      UMETA(DisplayName = "Story Milestone"),
    PlayerChoice        UMETA(DisplayName = "Player Choice"),
    FactionEvent        UMETA(DisplayName = "Faction Event"),
    Manual              UMETA(DisplayName = "Manual Trigger")
};

/**
 * Story arc types
 */
UENUM(BlueprintType)
enum class EStoryArcType : uint8
{
    MainStory           UMETA(DisplayName = "Main Story"),
    SideStory           UMETA(DisplayName = "Side Story"),
    CharacterArc        UMETA(DisplayName = "Character Arc"),
    FactionConflict     UMETA(DisplayName = "Faction Conflict"),
    Mystery             UMETA(DisplayName = "Mystery"),
    Romance             UMETA(DisplayName = "Romance"),
    Redemption          UMETA(DisplayName = "Redemption"),
    Exploration         UMETA(DisplayName = "Exploration"),
    Political           UMETA(DisplayName = "Political"),
    Cosmic              UMETA(DisplayName = "Cosmic Event")
};

/**
 * Generated story arc
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGeneratedStoryArc
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    FString ArcID;

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    FString ArcName;

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    EStoryArcType ArcType;

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    TArray<FString> RequiredPlanets; // Planets needed for this arc

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    TArray<FString> GeneratedQuests; // Quest IDs in this arc

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    TArray<FString> InvolvedNPCs; // NPCs involved in the arc

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    int32 EstimatedDuration; // Minutes to complete

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    int32 MinPlayerLevel; // Minimum player level

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    TMap<FString, FString> ArcVariables; // Story variables

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Story Arc")
    bool bIsCompleted;

    FGeneratedStoryArc()
    {
        ArcID = TEXT("");
        ArcName = TEXT("Generated Story Arc");
        ArcType = EStoryArcType::SideStory;
        Description = TEXT("");
        EstimatedDuration = 120; // 2 hours
        MinPlayerLevel = 1;
        bIsActive = false;
        bIsCompleted = false;
    }
};

/**
 * Galaxy expansion data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGalaxyExpansion
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    FString ExpansionID;

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    FString ExpansionName;

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    EExpansionTrigger TriggerType;

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    TArray<FPlanetData> NewPlanets; // Newly generated planets

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    TArray<FGeneratedStoryArc> NewStoryArcs; // New story arcs

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    TArray<FString> NewFactions; // New faction IDs

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    FString ExpansionTheme; // Overall theme of expansion

    UPROPERTY(BlueprintReadWrite, Category = "Galaxy Expansion")
    float ExpansionTimestamp; // When expansion was triggered

    FGalaxyExpansion()
    {
        ExpansionID = TEXT("");
        ExpansionName = TEXT("Galaxy Expansion");
        TriggerType = EExpansionTrigger::Manual;
        ExpansionTheme = TEXT("exploration");
        ExpansionTimestamp = 0.0f;
    }
};

/**
 * Infinite mode settings
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGalaxyInfiniteModeSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    bool bEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    float ExpansionInterval; // Seconds between expansions

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    int32 MaxActivePlanets; // Maximum planets in galaxy

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    int32 MaxActiveStoryArcs; // Maximum concurrent story arcs

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    TArray<EStoryArcType> AllowedArcTypes; // Story arc types to generate

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    bool bAdaptToPlayerChoices; // Whether to adapt based on player preferences

    UPROPERTY(BlueprintReadWrite, Category = "Infinite Mode")
    float DifficultyScaling; // How much to scale difficulty over time

    FInfiniteModeSettings()
    {
        bEnabled = false;
        ExpansionInterval = 3600.0f; // 1 hour
        MaxActivePlanets = 20;
        MaxActiveStoryArcs = 5;
        bAdaptToPlayerChoices = true;
        DifficultyScaling = 1.1f;
    }
};

/**
 * Galaxy expansion events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGalaxyExpanded, const FGalaxyExpansion&, Expansion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryArcGenerated, const FGeneratedStoryArc&, StoryArc);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryArcCompleted, const FGeneratedStoryArc&, StoryArc);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInfiniteModeToggled, bool, bEnabled);

/**
 * Galaxy Campaign Expander - Generates new planets and story arcs over time
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UGalaxyCampaignExpander : public UActorComponent
{
    GENERATED_BODY()

public:
    UGalaxyCampaignExpander();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize galaxy campaign expander
     * @param CampaignLoader Campaign loader for base data
     * @param QuestManager Quest manager for quest integration
     * @param PlanetGenerator Planet generator for new planets
     * @param QuestGenerator Quest generator for new quests
     * @param Timeline Timeline for tracking expansion history
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    void InitializeGalaxyExpander(UCampaignLoaderSubsystem* CampaignLoader,
                                 UQuestManagerComponent* QuestManager,
                                 UProceduralPlanetGenerator* PlanetGenerator,
                                 UProceduralQuestGenerator* QuestGenerator,
                                 UCampaignTimelineComponent* Timeline);

    /**
     * Trigger galaxy expansion
     * @param TriggerType What triggered the expansion
     * @param Context Additional context for expansion
     * @return Generated expansion data
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    FGalaxyExpansion TriggerGalaxyExpansion(EExpansionTrigger TriggerType, const FString& Context = TEXT(""));

    /**
     * Generate new story arc
     * @param ArcType Type of story arc to generate
     * @param Theme Theme for the story arc
     * @param PlayerLevel Current player level for scaling
     * @return Generated story arc
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    FGeneratedStoryArc GenerateStoryArc(EStoryArcType ArcType, const FString& Theme, int32 PlayerLevel);

    /**
     * Generate expansion planets
     * @param NumPlanets Number of planets to generate
     * @param Theme Theme for the planets
     * @return Array of generated planets
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    TArray<FPlanetData> GenerateExpansionPlanets(int32 NumPlanets, const FString& Theme);

    /**
     * Enable/disable infinite mode
     * @param bEnable Whether to enable infinite mode
     * @param Settings Infinite mode settings
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    void SetInfiniteModeEnabled(bool bEnable, const FInfiniteModeSettings& Settings);

    /**
     * Complete story arc
     * @param ArcID ID of the story arc to complete
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    void CompleteStoryArc(const FString& ArcID);

    /**
     * Get active story arcs
     * @return Array of active story arcs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Expansion")
    TArray<FGeneratedStoryArc> GetActiveStoryArcs() const;

    /**
     * Get expansion history
     * @return Array of all galaxy expansions
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Expansion")
    TArray<FGalaxyExpansion> GetExpansionHistory() const { return ExpansionHistory; }

    /**
     * Check if infinite mode is enabled
     * @return True if infinite mode is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Expansion")
    bool IsInfiniteModeEnabled() const { return InfiniteModeSettings.bEnabled; }

    /**
     * Get infinite mode settings
     * @return Current infinite mode settings
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Expansion")
    FInfiniteModeSettings GetInfiniteModeSettings() const { return InfiniteModeSettings; }

    /**
     * Force expansion trigger
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    void ForceExpansionTrigger();

    /**
     * Analyze player preferences for expansion
     * @return Map of preference scores for different content types
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Expansion")
    TMap<FString, float> AnalyzePlayerPreferences() const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Galaxy Expansion Events")
    FOnGalaxyExpanded OnGalaxyExpanded;

    UPROPERTY(BlueprintAssignable, Category = "Galaxy Expansion Events")
    FOnStoryArcGenerated OnStoryArcGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Galaxy Expansion Events")
    FOnStoryArcCompleted OnStoryArcCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Galaxy Expansion Events")
    FOnInfiniteModeToggled OnInfiniteModeToggled;

protected:
    // Expansion data
    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Expansion")
    TArray<FGalaxyExpansion> ExpansionHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Expansion")
    TArray<FGeneratedStoryArc> ActiveStoryArcs;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Expansion")
    FInfiniteModeSettings InfiniteModeSettings;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UProceduralPlanetGenerator* PlanetGeneratorRef;

    UPROPERTY()
    UProceduralQuestGenerator* QuestGeneratorRef;

    UPROPERTY()
    UCampaignTimelineComponent* TimelineRef;

    // Expansion tracking
    UPROPERTY()
    float LastExpansionTime;

    UPROPERTY()
    int32 TotalExpansions;

    // Timer handles
    FTimerHandle ExpansionTimer;

    // Story arc templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Arc Templates")
    /*
     * NOTE:
     * UPROPERTY reflection does not support nested containers such as
     * TMap<Key, TArray<Value>>.  Storing the story-arc templates that way
     * causes UHT compilation errors.
     *
     * Instead we keep all template strings in a single flat array and, when
     * needed, filter/group them by EStoryArcType inside helper functions
     * (e.g., LoadStoryArcTemplates or SelectTemplatesForType).
     */
    TArray<FString> ArcTemplates;

    // Expansion themes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Expansion Themes")
    TArray<FString> ExpansionThemes;

private:
    // Helper methods
    void LoadStoryArcTemplates();
    void ProcessInfiniteMode();
    bool ShouldTriggerExpansion(EExpansionTrigger TriggerType);
    FString DetermineExpansionTheme();
    TArray<FQuestData> GenerateArcQuests(const FGeneratedStoryArc& StoryArc);
    FString GenerateExpansionID();
    FString GenerateArcID();
    void CleanupCompletedArcs();

    // Event handlers
    UFUNCTION()
    void OnQuestCompleted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnTimelineEventAdded(const FTimelineEvent& Event);

public:
    /**
     * Blueprint implementable events for custom expansion logic
     */

    /**
     * Called when galaxy expands (for custom effects)
     * @param Expansion The galaxy expansion data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Expansion Events")
    void OnGalaxyExpandedEvent(const FGalaxyExpansion& Expansion);

    /**
     * Called when story arc is generated (for custom processing)
     * @param StoryArc The generated story arc
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Expansion Events")
    void OnStoryArcGeneratedEvent(const FGeneratedStoryArc& StoryArc);

    /**
     * Called to generate custom story arc (override in Blueprint)
     * @param ArcType Type of story arc
     * @param Theme Story theme
     * @param PlayerLevel Current player level
     * @return Custom generated story arc
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Expansion Events")
    FGeneratedStoryArc GenerateCustomStoryArc(EStoryArcType ArcType, const FString& Theme, int32 PlayerLevel);

    /**
     * Called to determine custom expansion trigger (override in Blueprint)
     * @param Context Current game context
     * @return True if expansion should be triggered
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Expansion Events")
    bool ShouldTriggerCustomExpansion(const FString& Context);
};
