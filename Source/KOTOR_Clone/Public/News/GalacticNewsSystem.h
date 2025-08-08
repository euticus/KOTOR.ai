// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Simulation/WorldStateSimulator.h"
#include "Politics/FactionDiplomacySystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Audio/VoiceSynthesisComponent.h"
#include "GalacticNewsSystem.generated.h"

/**
 * News categories
 */
UENUM(BlueprintType)
enum class ENewsCategory : uint8
{
    Politics            UMETA(DisplayName = "Politics"),
    Military            UMETA(DisplayName = "Military"),
    Economy             UMETA(DisplayName = "Economy"),
    Crime               UMETA(DisplayName = "Crime"),
    Technology          UMETA(DisplayName = "Technology"),
    Culture             UMETA(DisplayName = "Culture"),
    Sports              UMETA(DisplayName = "Sports"),
    Weather             UMETA(DisplayName = "Weather"),
    Disaster            UMETA(DisplayName = "Disaster"),
    Discovery           UMETA(DisplayName = "Discovery"),
    Obituary            UMETA(DisplayName = "Obituary"),
    Entertainment       UMETA(DisplayName = "Entertainment")
};

/**
 * News priority levels
 */
UENUM(BlueprintType)
enum class ENewsPriority : uint8
{
    Breaking            UMETA(DisplayName = "Breaking News"),
    Major               UMETA(DisplayName = "Major Story"),
    Standard            UMETA(DisplayName = "Standard"),
    Minor               UMETA(DisplayName = "Minor"),
    Filler              UMETA(DisplayName = "Filler")
};

/**
 * News bias types
 */
UENUM(BlueprintType)
enum class ENewsBias : uint8
{
    Neutral             UMETA(DisplayName = "Neutral"),
    ProRepublic         UMETA(DisplayName = "Pro-Republic"),
    ProSith             UMETA(DisplayName = "Pro-Sith"),
    ProJedi             UMETA(DisplayName = "Pro-Jedi"),
    AntiJedi            UMETA(DisplayName = "Anti-Jedi"),
    Corporate           UMETA(DisplayName = "Corporate"),
    Populist            UMETA(DisplayName = "Populist"),
    Propaganda          UMETA(DisplayName = "Propaganda")
};

/**
 * News article data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNewsArticle
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString ArticleID;

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString Headline;

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString Content; // LLM-generated article content

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString Summary; // Brief summary

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    ENewsCategory Category;

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    ENewsPriority Priority;

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    ENewsBias Bias;

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString NewsOutlet; // Which news organization published this

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString Author; // Reporter name

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    float PublishTime; // When article was published

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    TArray<FString> RelatedPlanets; // Planets mentioned in article

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    TArray<FString> RelatedFactions; // Factions mentioned

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    TArray<FString> Tags; // Article tags

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    bool bPlayerInvolved; // Whether player actions are mentioned

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    FString PlayerReference; // How player is referenced (if at all)

    UPROPERTY(BlueprintReadWrite, Category = "News Article")
    bool bHasAudio; // Whether article has TTS audio

    FNewsArticle()
    {
        ArticleID = TEXT("");
        Headline = TEXT("Galactic News");
        Content = TEXT("");
        Summary = TEXT("");
        Category = ENewsCategory::Politics;
        Priority = ENewsPriority::Standard;
        Bias = ENewsBias::Neutral;
        NewsOutlet = TEXT("HoloNet News");
        Author = TEXT("Unknown Reporter");
        PublishTime = 0.0f;
        bPlayerInvolved = false;
        PlayerReference = TEXT("");
        bHasAudio = false;
    }
};

/**
 * News outlet data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNewsOutlet
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    FString OutletName;

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    ENewsBias DefaultBias;

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    TArray<FString> FavoredFactions; // Factions this outlet favors

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    TArray<ENewsCategory> PreferredCategories; // Categories this outlet focuses on

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    float Credibility; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "News Outlet")
    FString HeadquartersPlanet; // Where outlet is based

    FNewsOutlet()
    {
        OutletName = TEXT("HoloNet News");
        Description = TEXT("Galactic news network");
        DefaultBias = ENewsBias::Neutral;
        Credibility = 0.7f;
        HeadquartersPlanet = TEXT("Coruscant");
    }
};

/**
 * News events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewsArticlePublished, const FNewsArticle&, Article);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBreakingNewsAlert, const FNewsArticle&, Article);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewsSearchCompleted, const FString&, SearchTerm, const TArray<FNewsArticle>&, Results);

/**
 * Galactic News System - Simulates HoloNet with dynamic news generation
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UGalacticNewsSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UGalacticNewsSystem();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize galactic news system
     * @param WorldSimulator World state simulator for events
     * @param FactionSystem Faction system for political news
     * @param NarrativeMemory Narrative memory for player actions
     * @param VoiceSynthesis Voice synthesis for audio news
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    void InitializeNewsSystem(UWorldStateSimulator* WorldSimulator,
                             UFactionDiplomacySystem* FactionSystem,
                             UNarrativeMemoryComponent* NarrativeMemory,
                             UVoiceSynthesisComponent* VoiceSynthesis);

    /**
     * Generate news article from event
     * @param TriggerEvent Event that triggered the news
     * @param Category Category of news article
     * @param NewsOutlet Outlet publishing the article
     * @return Generated news article
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    FNewsArticle GenerateNewsArticle(const FString& TriggerEvent, 
                                    ENewsCategory Category, 
                                    const FString& NewsOutlet = TEXT(""));

    /**
     * Publish news article
     * @param Article Article to publish
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    void PublishNewsArticle(const FNewsArticle& Article);

    /**
     * Generate breaking news alert
     * @param Event Major event for breaking news
     * @return Generated breaking news article
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    FNewsArticle GenerateBreakingNews(const FString& Event);

    /**
     * Update news cycle
     * Called periodically to generate new articles
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    void UpdateNewsCycle();

    /**
     * Search news articles
     * @param SearchTerm Term to search for
     * @param Categories Categories to search (empty = all)
     * @return Array of matching articles
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    TArray<FNewsArticle> SearchNewsArticles(const FString& SearchTerm, 
                                           const TArray<ENewsCategory>& Categories);

    /**
     * Get news articles by category
     * @param Category Category to filter by
     * @param MaxResults Maximum number of results
     * @return Array of articles in category
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galactic News")
    TArray<FNewsArticle> GetNewsArticlesByCategory(ENewsCategory Category, int32 MaxResults = 10) const;

    /**
     * Get latest news articles
     * @param MaxResults Maximum number of results
     * @return Array of latest articles
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galactic News")
    TArray<FNewsArticle> GetLatestNewsArticles(int32 MaxResults = 10) const;

    /**
     * Get breaking news articles
     * @return Array of breaking news articles
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galactic News")
    TArray<FNewsArticle> GetBreakingNewsArticles() const;

    /**
     * Get news outlets
     * @return Array of all news outlets
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galactic News")
    TArray<FNewsOutlet> GetNewsOutlets() const { return NewsOutlets; }

    /**
     * Add news outlet
     * @param Outlet News outlet to add
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    void AddNewsOutlet(const FNewsOutlet& Outlet);

    /**
     * Generate audio for article
     * @param ArticleID ID of article to generate audio for
     * @return True if audio was generated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    bool GenerateArticleAudio(const FString& ArticleID);

    /**
     * Set news update frequency
     * @param UpdateInterval Seconds between news updates
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    void SetNewsUpdateFrequency(float UpdateInterval);

    /**
     * Enable/disable automatic news generation
     * @param bEnabled Whether to enable automatic news
     */
    UFUNCTION(BlueprintCallable, Category = "Galactic News")
    void SetAutomaticNewsEnabled(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "News Events")
    FOnNewsArticlePublished OnNewsArticlePublished;

    UPROPERTY(BlueprintAssignable, Category = "News Events")
    FOnBreakingNewsAlert OnBreakingNewsAlert;

    UPROPERTY(BlueprintAssignable, Category = "News Events")
    FOnNewsSearchCompleted OnNewsSearchCompleted;

protected:
    // News data
    UPROPERTY(BlueprintReadOnly, Category = "Galactic News")
    TArray<FNewsArticle> PublishedArticles;

    UPROPERTY(BlueprintReadOnly, Category = "Galactic News")
    TArray<FNewsOutlet> NewsOutlets;

    // Component references
    UPROPERTY()
    UWorldStateSimulator* WorldSimulatorRef;

    UPROPERTY()
    UFactionDiplomacySystem* FactionSystemRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UVoiceSynthesisComponent* VoiceSynthesisRef;

    // News settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "News Settings")
    bool bAutomaticNewsEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "News Settings")
    float NewsUpdateInterval; // Seconds between updates

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "News Settings")
    int32 MaxStoredArticles; // Maximum articles to keep

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "News Settings")
    float PlayerMentionProbability; // Chance of mentioning player

    // Timer handles
    FTimerHandle NewsUpdateTimer;

    // Article templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Article Templates")
    /*
     * NOTE:
     * UPROPERTY does not support nested containers such as
     * `TMap<Key, TArray<Value>>`.  Storing the prompt templates in that shape
     * will cause Unreal Header Tool compilation errors.
     *
     * We therefore keep the templates in a single flat array and group / filter
     * them by category at runtime inside the news system.
     */
    TArray<FString> ArticlePromptTemplates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Article Templates")
    TMap<ENewsBias, FString> BiasModifiers;

private:
    // Helper methods
    void LoadNewsOutlets();
    void LoadArticleTemplates();
    FString GenerateArticleID();
    FString BuildArticlePrompt(const FString& Event, ENewsCategory Category, const FNewsOutlet& Outlet);
    FNewsOutlet SelectNewsOutlet(ENewsCategory Category, const FString& Event);
    ENewsPriority DetermineNewsPriority(const FString& Event);
    TArray<FString> ExtractTags(const FString& Content);
    void TrimOldArticles();
    bool ShouldMentionPlayer(const FString& Event);
    FString GeneratePlayerReference(const FString& Event);

    // Event handlers
    UFUNCTION()
    void OnGalacticEventTriggered(const FGalacticEvent& Event);

    UFUNCTION()
    void OnPlanetStateChanged(const FPlanetWorldState& NewState);

    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

public:
    /**
     * Blueprint implementable events for custom news logic
     */

    /**
     * Called when news article is published (for custom processing)
     * @param Article The published article
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "News Events")
    void OnNewsArticlePublishedEvent(const FNewsArticle& Article);

    /**
     * Called when breaking news is triggered (for custom alerts)
     * @param Article The breaking news article
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "News Events")
    void OnBreakingNewsAlertEvent(const FNewsArticle& Article);

    /**
     * Called to generate custom news content (override in Blueprint)
     * @param TriggerEvent Event that triggered the news
     * @param Category News category
     * @param Outlet News outlet
     * @return Custom generated article content
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "News Events")
    FString GenerateCustomNewsContent(const FString& TriggerEvent, ENewsCategory Category, const FNewsOutlet& Outlet);

    /**
     * Called to determine custom news priority (override in Blueprint)
     * @param Event Event to evaluate
     * @param PlayerInvolved Whether player is involved
     * @return Custom priority level
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "News Events")
    ENewsPriority DetermineCustomNewsPriority(const FString& Event, bool PlayerInvolved);
};
