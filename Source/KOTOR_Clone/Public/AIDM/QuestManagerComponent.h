// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "QuestManagerComponent.generated.h"

/**
 * Quest state enumeration
 */
UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted      UMETA(DisplayName = "Not Started"),
    Active          UMETA(DisplayName = "Active"),
    Completed       UMETA(DisplayName = "Completed"),
    Failed          UMETA(DisplayName = "Failed"),
    TurnedIn        UMETA(DisplayName = "Turned In")
};

/**
 * Quest objective data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quest Objective")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Objective")
    bool bIsCompleted;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Objective")
    bool bIsOptional;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Objective")
    int32 CurrentProgress;

    UPROPERTY(BlueprintReadWrite, Category = "Quest Objective")
    int32 RequiredProgress;

    FQuestObjective()
    {
        Description = TEXT("");
        bIsCompleted = false;
        bIsOptional = false;
        CurrentProgress = 0;
        RequiredProgress = 1;
    }
};

/**
 * Active quest instance
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FActiveQuest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString QuestID;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FQuestData QuestData;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    EQuestState State;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    TArray<FQuestObjective> Objectives;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    float StartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    float CompletionTime;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FString QuestGiverName;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    int32 PlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FString LayoutName;

    FActiveQuest()
    {
        QuestID = TEXT("");
        State = EQuestState::NotStarted;
        StartTime = 0.0f;
        CompletionTime = 0.0f;
        QuestGiverName = TEXT("");
        PlanetIndex = -1;
        LayoutName = TEXT("");
    }
};

/**
 * Quest events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStarted, const FActiveQuest&, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, const FActiveQuest&, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestFailed, const FActiveQuest&, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestObjectiveUpdated, const FString&, QuestID, int32, ObjectiveIndex);

/**
 * Quest Manager Component - Manages quest state and progression
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UQuestManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UQuestManagerComponent();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Start a new quest
     * @param QuestData The quest data from AIDM
     * @param QuestGiverName Name of the NPC giving the quest
     * @param PlanetIndex Current planet index
     * @param LayoutName Current layout name
     * @return The quest ID if successful, empty string if failed
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    FString StartQuest(const FQuestData& QuestData, const FString& QuestGiverName, 
                      int32 PlanetIndex, const FString& LayoutName);

    /**
     * Complete a quest
     * @param QuestID The ID of the quest to complete
     * @return True if quest was completed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    bool CompleteQuest(const FString& QuestID);

    /**
     * Fail a quest
     * @param QuestID The ID of the quest to fail
     * @return True if quest was failed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    bool FailQuest(const FString& QuestID);

    /**
     * Update quest objective progress
     * @param QuestID The ID of the quest
     * @param ObjectiveIndex Index of the objective to update
     * @param Progress Amount of progress to add
     * @return True if objective was updated
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    bool UpdateQuestObjective(const FString& QuestID, int32 ObjectiveIndex, int32 Progress = 1);

    /**
     * Get an active quest by ID
     * @param QuestID The quest ID to find
     * @return The active quest, or empty struct if not found
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
    FActiveQuest GetActiveQuest(const FString& QuestID) const;

    /**
     * Get all active quests
     * @return Array of all active quests
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
    TArray<FActiveQuest> GetActiveQuests() const;

    /**
     * Get all completed quests
     * @return Array of all completed quests
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
    TArray<FActiveQuest> GetCompletedQuests() const;

    /**
     * Check if a quest is active
     * @param QuestID The quest ID to check
     * @return True if the quest is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
    bool IsQuestActive(const FString& QuestID) const;

    /**
     * Check if a quest is completed
     * @param QuestID The quest ID to check
     * @return True if the quest is completed
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
    bool IsQuestCompleted(const FString& QuestID) const;

    /**
     * Get quest completion percentage
     * @param QuestID The quest ID to check
     * @return Completion percentage (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Manager")
    float GetQuestCompletionPercentage(const FString& QuestID) const;

    /**
     * Clear all quests (useful for testing or new campaigns)
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    void ClearAllQuests();

    /**
     * Save quest data to a save game
     * @return Serialized quest data as JSON string
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    FString SaveQuestData() const;

    /**
     * Load quest data from a save game
     * @param SaveData Serialized quest data as JSON string
     * @return True if data was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Manager")
    bool LoadQuestData(const FString& SaveData);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestStarted OnQuestStarted;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestFailed OnQuestFailed;

    UPROPERTY(BlueprintAssignable, Category = "Quest Events")
    FOnQuestObjectiveUpdated OnQuestObjectiveUpdated;

protected:
    // Active quests storage
    UPROPERTY(BlueprintReadOnly, Category = "Quest Manager")
    TArray<FActiveQuest> ActiveQuests;

    // Completed quests storage
    UPROPERTY(BlueprintReadOnly, Category = "Quest Manager")
    TArray<FActiveQuest> CompletedQuests;

    // Failed quests storage
    UPROPERTY(BlueprintReadOnly, Category = "Quest Manager")
    TArray<FActiveQuest> FailedQuests;

    // Quest ID counter for generating unique IDs
    UPROPERTY(BlueprintReadOnly, Category = "Quest Manager")
    int32 NextQuestID;

    // Debug settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Manager|Debug")
    bool bDebugMode;

private:
    // Helper methods
    FString GenerateQuestID();
    FActiveQuest* FindActiveQuest(const FString& QuestID);
    void CreateDefaultObjectives(FActiveQuest& Quest);
    bool AreAllObjectivesCompleted(const FActiveQuest& Quest) const;
    void LogQuestEvent(const FString& Event, const FActiveQuest& Quest) const;

public:
    /**
     * Blueprint implementable events for custom quest logic
     */

    /**
     * Called when a quest is started (for custom logic)
     * @param Quest The quest that was started
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestStartedEvent(const FActiveQuest& Quest);

    /**
     * Called when a quest is completed (for custom logic)
     * @param Quest The quest that was completed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestCompletedEvent(const FActiveQuest& Quest);

    /**
     * Called when a quest objective is updated (for custom logic)
     * @param Quest The quest with updated objective
     * @param ObjectiveIndex The index of the updated objective
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Events")
    void OnQuestObjectiveUpdatedEvent(const FActiveQuest& Quest, int32 ObjectiveIndex);
};
