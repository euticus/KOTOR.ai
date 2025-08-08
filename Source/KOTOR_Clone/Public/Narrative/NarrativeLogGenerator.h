// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Timeline/CampaignTimelineComponent.h"
#include "Items/MythicArtifactSystem.h"
#include "NarrativeLogGenerator.generated.h"

/**
 * Codex entry types
 */
UENUM(BlueprintType)
enum class ECodexEntryType : uint8
{
    QuestSummary        UMETA(DisplayName = "Quest Summary"),
    CharacterProfile    UMETA(DisplayName = "Character Profile"),
    LocationDescription UMETA(DisplayName = "Location Description"),
    ArtifactLore        UMETA(DisplayName = "Artifact Lore"),
    MoralChoice         UMETA(DisplayName = "Moral Choice"),
    BattleRecord        UMETA(DisplayName = "Battle Record"),
    Discovery           UMETA(DisplayName = "Discovery"),
    Betrayal            UMETA(DisplayName = "Betrayal"),
    Alliance            UMETA(DisplayName = "Alliance"),
    Prophecy            UMETA(DisplayName = "Prophecy"),
    Reflection          UMETA(DisplayName = "Reflection"),
    Legacy              UMETA(DisplayName = "Legacy")
};

/**
 * Writing style types
 */
UENUM(BlueprintType)
enum class EWritingStyle : uint8
{
    Poetic              UMETA(DisplayName = "Poetic"),
    Historical          UMETA(DisplayName = "Historical"),
    Personal            UMETA(DisplayName = "Personal Journal"),
    Mythical            UMETA(DisplayName = "Mythical"),
    Clinical            UMETA(DisplayName = "Clinical"),
    Dramatic            UMETA(DisplayName = "Dramatic"),
    Philosophical       UMETA(DisplayName = "Philosophical"),
    Prophetic           UMETA(DisplayName = "Prophetic")
};

/**
 * Codex entry data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCodexEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    FString EntryID;

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    FString Title;

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    ECodexEntryType EntryType;

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    EWritingStyle WritingStyle;

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    FString Content; // LLM-generated poetic content

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    FString Summary; // Brief summary

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    TArray<FString> Tags; // Searchable tags

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    TArray<FString> RelatedEntries; // Related entry IDs

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    FString TriggerEvent; // What triggered this entry

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    float Timestamp; // When entry was created

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    int32 ImportanceLevel; // 1-5, higher = more important

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    bool bIsFavorited; // Player favorited this entry

    UPROPERTY(BlueprintReadWrite, Category = "Codex Entry")
    TMap<FString, FString> Metadata; // Additional metadata

    FCodexEntry()
    {
        EntryID = TEXT("");
        Title = TEXT("Untitled Entry");
        EntryType = ECodexEntryType::QuestSummary;
        WritingStyle = EWritingStyle::Poetic;
        Content = TEXT("");
        Summary = TEXT("");
        TriggerEvent = TEXT("");
        Timestamp = 0.0f;
        ImportanceLevel = 1;
        bIsFavorited = false;
    }
};

/**
 * Narrative theme data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNarrativeTheme
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Theme")
    FString ThemeName;

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Theme")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Theme")
    float Prevalence; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Theme")
    TArray<FString> RelatedEvents; // Events that reinforce this theme

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Theme")
    TArray<FString> KeySymbols; // Symbolic elements

    FNarrativeTheme()
    {
        ThemeName = TEXT("");
        Description = TEXT("");
        Prevalence = 0.0f;
    }
};

/**
 * Codex events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCodexEntryGenerated, const FCodexEntry&, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNarrativeThemeIdentified, const FNarrativeTheme&, Theme);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCodexSearchCompleted, const FString&, SearchTerm, const TArray<FCodexEntry>&, Results);

/**
 * Narrative Log Generator - Creates poetic campaign logs and codex entries
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UNarrativeLogGenerator : public UActorComponent
{
    GENERATED_BODY()

public:
    UNarrativeLogGenerator();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize narrative log generator
     * @param QuestManager Quest manager for quest data
     * @param NarrativeMemory Narrative memory for events
     * @param Timeline Campaign timeline for chronology
     * @param ArtifactSystem Artifact system for item lore
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    void InitializeLogGenerator(UQuestManagerComponent* QuestManager,
                               UNarrativeMemoryComponent* NarrativeMemory,
                               UCampaignTimelineComponent* Timeline,
                               UMythicArtifactSystem* ArtifactSystem);

    /**
     * Generate codex entry from event
     * @param TriggerEvent Event that triggered the entry
     * @param EntryType Type of entry to generate
     * @param WritingStyle Style of writing to use
     * @return Generated codex entry
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    FCodexEntry GenerateCodexEntry(const FString& TriggerEvent, 
                                  ECodexEntryType EntryType, 
                                  EWritingStyle WritingStyle = EWritingStyle::Poetic);

    /**
     * Generate quest summary entry
     * @param QuestID ID of the completed quest
     * @return Generated quest summary
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    FCodexEntry GenerateQuestSummary(const FString& QuestID);

    /**
     * Generate character profile entry
     * @param CharacterName Name of the character
     * @param RelationshipData Relationship data with character
     * @return Generated character profile
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    FCodexEntry GenerateCharacterProfile(const FString& CharacterName, const FString& RelationshipData);

    /**
     * Generate artifact lore entry
     * @param ArtifactID ID of the artifact
     * @return Generated artifact lore
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    FCodexEntry GenerateArtifactLore(const FString& ArtifactID);

    /**
     * Generate campaign reflection
     * @param TimeWindow Time window to reflect on (hours)
     * @return Generated reflection entry
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    FCodexEntry GenerateCampaignReflection(float TimeWindow = 24.0f);

    /**
     * Search codex entries
     * @param SearchTerm Term to search for
     * @param EntryTypes Types of entries to search (empty = all)
     * @return Array of matching entries
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    TArray<FCodexEntry> SearchCodexEntries(const FString& SearchTerm, 
                                          const TArray<ECodexEntryType>& EntryTypes = );

    /**
     * Get codex entries by type
     * @param EntryType Type of entries to retrieve
     * @return Array of entries of the specified type
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Log")
    TArray<FCodexEntry> GetCodexEntriesByType(ECodexEntryType EntryType) const;

    /**
     * Get all codex entries
     * @return Array of all codex entries
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Log")
    TArray<FCodexEntry> GetAllCodexEntries() const { return CodexEntries; }

    /**
     * Get favorited entries
     * @return Array of favorited entries
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Narrative Log")
    TArray<FCodexEntry> GetFavoritedEntries() const;

    /**
     * Toggle entry favorite status
     * @param EntryID ID of the entry to toggle
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    void ToggleEntryFavorite(const FString& EntryID);

    /**
     * Identify narrative themes
     * @return Array of identified themes in the campaign
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    TArray<FNarrativeTheme> IdentifyNarrativeThemes();

    /**
     * Generate campaign summary
     * @param IncludeFavorites Whether to focus on favorited entries
     * @return LLM-generated campaign summary
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    FString GenerateCampaignSummary(bool IncludeFavorites = false);

    /**
     * Export codex to file
     * @param FilePath Path to export file
     * @param Format Export format ("json", "txt", "html")
     * @return True if export was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    bool ExportCodex(const FString& FilePath, const FString& Format = TEXT("json"));

    /**
     * Set automatic entry generation enabled
     * @param bEnabled Whether to automatically generate entries
     */
    UFUNCTION(BlueprintCallable, Category = "Narrative Log")
    void SetAutomaticGenerationEnabled(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Narrative Log Events")
    FOnCodexEntryGenerated OnCodexEntryGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Narrative Log Events")
    FOnNarrativeThemeIdentified OnNarrativeThemeIdentified;

    UPROPERTY(BlueprintAssignable, Category = "Narrative Log Events")
    FOnCodexSearchCompleted OnCodexSearchCompleted;

protected:
    // Codex data
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Log")
    TArray<FCodexEntry> CodexEntries;

    UPROPERTY(BlueprintReadOnly, Category = "Narrative Log")
    TArray<FNarrativeTheme> IdentifiedThemes;

    // Component references
    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UCampaignTimelineComponent* TimelineRef;

    UPROPERTY()
    UMythicArtifactSystem* ArtifactSystemRef;

    // Generation settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
    bool bAutomaticGeneration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
    EWritingStyle DefaultWritingStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
    int32 MaxEntriesPerType; // Maximum entries per type

    // LLM prompts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Prompts")
    TMap<ECodexEntryType, FString> EntryPromptTemplates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Prompts")
    TMap<EWritingStyle, FString> StylePromptModifiers;

private:
    // Helper methods
    void LoadPromptTemplates();
    FString GenerateEntryID();
    FString BuildLLMPrompt(ECodexEntryType EntryType, EWritingStyle WritingStyle, const FString& Context);
    TArray<FString> ExtractTags(const FString& Content);
    TArray<FString> FindRelatedEntries(const FCodexEntry& Entry);
    int32 CalculateImportanceLevel(const FString& TriggerEvent);
    void TrimOldEntries();

    // Event handlers
    UFUNCTION()
    void OnQuestCompleted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

    UFUNCTION()
    void OnTimelineEventAdded(const FTimelineEvent& Event);

public:
    /**
     * Blueprint implementable events for custom log generation
     */

    /**
     * Called when codex entry is generated (for custom processing)
     * @param Entry The generated entry
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Log Events")
    void OnCodexEntryGeneratedEvent(const FCodexEntry& Entry);

    /**
     * Called when narrative theme is identified (for custom effects)
     * @param Theme The identified theme
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Log Events")
    void OnNarrativeThemeIdentifiedEvent(const FNarrativeTheme& Theme);

    /**
     * Called to generate custom codex content (override in Blueprint)
     * @param EntryType Type of entry to generate
     * @param Context Context for the entry
     * @param WritingStyle Style to use
     * @return Custom generated content
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Log Events")
    FString GenerateCustomCodexContent(ECodexEntryType EntryType, const FString& Context, EWritingStyle WritingStyle);

    /**
     * Called to determine custom entry importance (override in Blueprint)
     * @param TriggerEvent Event that triggered the entry
     * @param EntryType Type of entry
     * @return Custom importance level (1-5)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Log Events")
    int32 CalculateCustomImportanceLevel(const FString& TriggerEvent, ECodexEntryType EntryType);
};
