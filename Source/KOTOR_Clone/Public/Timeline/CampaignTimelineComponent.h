// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/QuestManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "CampaignTimelineComponent.generated.h"

/**
 * Timeline event types
 */
UENUM(BlueprintType)
enum class ETimelineEventType : uint8
{
    QuestStarted        UMETA(DisplayName = "Quest Started"),
    QuestCompleted      UMETA(DisplayName = "Quest Completed"),
    QuestFailed         UMETA(DisplayName = "Quest Failed"),
    MoralChoice         UMETA(DisplayName = "Moral Choice"),
    CompanionRecruited  UMETA(DisplayName = "Companion Recruited"),
    CompanionLoyalty    UMETA(DisplayName = "Companion Loyalty Change"),
    PlanetVisited       UMETA(DisplayName = "Planet Visited"),
    CombatEncounter     UMETA(DisplayName = "Combat Encounter"),
    DialogueChoice      UMETA(DisplayName = "Dialogue Choice"),
    ItemAcquired        UMETA(DisplayName = "Item Acquired"),
    LevelUp             UMETA(DisplayName = "Level Up"),
    StoryMilestone      UMETA(DisplayName = "Story Milestone"),
    Custom              UMETA(DisplayName = "Custom Event")
};

/**
 * Timeline event data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FTimelineEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    FString EventID;

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    ETimelineEventType EventType;

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    FString Title;

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    float Timestamp; // Game time when event occurred

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    FString Location; // Planet/Layout where event occurred

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    TArray<FString> ParticipantIDs; // NPCs/Companions involved

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    TMap<FString, FString> EventData; // Additional event-specific data

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    float AlignmentImpact; // Impact on player alignment

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    int32 ImportanceLevel; // 1-5 scale of importance

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    TArray<FString> Consequences; // What happened as a result

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    bool bIsReplayable; // Whether this event can be replayed

    UPROPERTY(BlueprintReadWrite, Category = "Timeline Event")
    FString ReplayData; // Data needed to replay this event

    FTimelineEvent()
    {
        EventID = TEXT("");
        EventType = ETimelineEventType::Custom;
        Title = TEXT("Timeline Event");
        Description = TEXT("");
        Timestamp = 0.0f;
        Location = TEXT("");
        AlignmentImpact = 0.0f;
        ImportanceLevel = 1;
        bIsReplayable = false;
        ReplayData = TEXT("");
    }
};

/**
 * World state snapshot
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FWorldStateSnapshot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FString SnapshotID;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    float Timestamp;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FString SnapshotName;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    int32 CurrentPlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FString CurrentLayout;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TArray<FString> ActiveQuests;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TArray<FString> CompletedQuests;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TArray<FString> RecruitedCompanions;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TMap<FString, int32> CompanionLoyalty;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TMap<FString, bool> StoryFlags;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FString PlayerAlignment;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    int32 PlayerLevel;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TArray<FString> PlayerInventory;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TMap<FString, FString> CustomData; // Additional state data

    FWorldStateSnapshot()
    {
        SnapshotID = TEXT("");
        Timestamp = 0.0f;
        SnapshotName = TEXT("World State");
        CurrentPlanetIndex = 0;
        CurrentLayout = TEXT("");
        PlayerAlignment = TEXT("neutral");
        PlayerLevel = 1;
    }
};

/**
 * Replay session data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FReplaySession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    FString SessionName;

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    TArray<FString> EventIDs; // Events to replay in order

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    float StartTimestamp;

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    float EndTimestamp;

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    bool bIsPlaying;

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    int32 CurrentEventIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Replay Session")
    float PlaybackSpeed; // 1.0 = normal speed

    FReplaySession()
    {
        SessionID = TEXT("");
        SessionName = TEXT("Replay Session");
        StartTimestamp = 0.0f;
        EndTimestamp = 0.0f;
        bIsPlaying = false;
        CurrentEventIndex = 0;
        PlaybackSpeed = 1.0f;
    }
};

/**
 * Timeline events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineEventAdded, const FTimelineEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldStateSnapshot, const FWorldStateSnapshot&, Snapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplayStarted, const FReplaySession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplayEventPlayed, const FTimelineEvent&, Event);

/**
 * Campaign Timeline Component - Tracks campaign history and enables replay
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCampaignTimelineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCampaignTimelineComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize timeline system
     * @param QuestManager Quest manager for quest events
     * @param CompanionManager Companion manager for companion events
     * @param NarrativeMemory Narrative memory for decision tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    void InitializeTimeline(UQuestManagerComponent* QuestManager, 
                           UCompanionManagerComponent* CompanionManager,
                           UNarrativeMemoryComponent* NarrativeMemory);

    /**
     * Add event to timeline
     * @param Event Event to add
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    void AddTimelineEvent(const FTimelineEvent& Event);

    /**
     * Create world state snapshot
     * @param SnapshotName Name for the snapshot
     * @return Created snapshot
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    FWorldStateSnapshot CreateWorldStateSnapshot(const FString& SnapshotName);

    /**
     * Restore world state from snapshot
     * @param Snapshot Snapshot to restore from
     * @return True if restoration was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    bool RestoreWorldState(const FWorldStateSnapshot& Snapshot);

    /**
     * Start replay session
     * @param StartTime Start time for replay
     * @param EndTime End time for replay
     * @param PlaybackSpeed Speed of playback (1.0 = normal)
     * @return Replay session ID
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    FString StartReplaySession(float StartTime, float EndTime, float PlaybackSpeed = 1.0f);

    /**
     * Stop current replay session
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    void StopReplaySession();

    /**
     * Pause/resume replay session
     * @param bPause Whether to pause or resume
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    void PauseReplaySession(bool bPause);

    /**
     * Seek to specific time in replay
     * @param Timestamp Time to seek to
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    void SeekReplayToTime(float Timestamp);

    /**
     * Get timeline events in time range
     * @param StartTime Start time
     * @param EndTime End time
     * @return Array of events in range
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    TArray<FTimelineEvent> GetEventsInTimeRange(float StartTime, float EndTime) const;

    /**
     * Get events by type
     * @param EventType Type of events to get
     * @return Array of events of specified type
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    TArray<FTimelineEvent> GetEventsByType(ETimelineEventType EventType) const;

    /**
     * Get major story milestones
     * @return Array of important story events
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    TArray<FTimelineEvent> GetStoryMilestones() const;

    /**
     * Get campaign statistics
     * @return Map of campaign statistics
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    TMap<FString, int32> GetCampaignStatistics() const;

    /**
     * Export timeline data
     * @return JSON string of timeline data
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    FString ExportTimelineData() const;

    /**
     * Import timeline data
     * @param TimelineData JSON string of timeline data
     * @return True if import was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Timeline")
    bool ImportTimelineData(const FString& TimelineData);

    /**
     * Get current game time
     * @return Current game time in seconds
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Timeline")
    float GetCurrentGameTime() const { return CurrentGameTime; }

    /**
     * Get timeline events
     * @return Array of all timeline events
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Timeline")
    TArray<FTimelineEvent> GetTimelineEvents() const { return TimelineEvents; }

    /**
     * Get world state snapshots
     * @return Array of all snapshots
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Timeline")
    TArray<FWorldStateSnapshot> GetWorldStateSnapshots() const { return WorldStateSnapshots; }

    /**
     * Check if replay is active
     * @return True if replay session is running
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Timeline")
    bool IsReplayActive() const { return CurrentReplaySession.bIsPlaying; }

    /**
     * Get current replay session
     * @return Current replay session data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Timeline")
    FReplaySession GetCurrentReplaySession() const { return CurrentReplaySession; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Timeline Events")
    FOnTimelineEventAdded OnTimelineEventAdded;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Events")
    FOnWorldStateSnapshot OnWorldStateSnapshot;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Events")
    FOnReplayStarted OnReplayStarted;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Events")
    FOnReplayEventPlayed OnReplayEventPlayed;

protected:
    // Timeline data
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Timeline")
    TArray<FTimelineEvent> TimelineEvents;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Timeline")
    TArray<FWorldStateSnapshot> WorldStateSnapshots;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Timeline")
    float CurrentGameTime;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Timeline")
    FReplaySession CurrentReplaySession;

    // Component references
    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    // Timeline settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
    bool bAutoCreateSnapshots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
    float SnapshotInterval; // Seconds between auto snapshots

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
    int32 MaxTimelineEvents; // Maximum events to keep

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
    int32 MaxSnapshots; // Maximum snapshots to keep

    // Replay state
    UPROPERTY()
    bool bReplayPaused;

    UPROPERTY()
    float ReplayStartTime;

    UPROPERTY()
    float LastSnapshotTime;

    // Timer handles
    FTimerHandle SnapshotTimer;
    FTimerHandle ReplayTimer;

private:
    // Helper methods
    FString GenerateEventID();
    FString GenerateSnapshotID();
    FString GenerateSessionID();
    void CleanupOldEvents();
    void CleanupOldSnapshots();
    void ProcessReplayTick();
    void PlayReplayEvent(const FTimelineEvent& Event);
    FWorldStateSnapshot CaptureCurrentWorldState(const FString& SnapshotName);

    // Event handlers
    UFUNCTION()
    void OnQuestStarted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnQuestCompleted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnCompanionRecruited(const FActiveCompanion& Companion);

    UFUNCTION()
    void OnCompanionLoyaltyChanged(const FActiveCompanion& Companion);

    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

public:
    /**
     * Blueprint implementable events for custom timeline logic
     */

    /**
     * Called when timeline event is added (for custom processing)
     * @param Event The added event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Timeline Events")
    void OnTimelineEventAddedEvent(const FTimelineEvent& Event);

    /**
     * Called when world state snapshot is created (for custom data)
     * @param Snapshot The created snapshot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Timeline Events")
    void OnWorldStateSnapshotEvent(const FWorldStateSnapshot& Snapshot);

    /**
     * Called when replay event is played (for custom effects)
     * @param Event The replayed event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Timeline Events")
    void OnReplayEventPlayedEvent(const FTimelineEvent& Event);

    /**
     * Called to capture custom world state data (override in Blueprint)
     * @param Snapshot The snapshot being created
     * @return Custom world state data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Timeline Events")
    TMap<FString, FString> CaptureCustomWorldState(const FWorldStateSnapshot& Snapshot);

    /**
     * Called to restore custom world state data (override in Blueprint)
     * @param Snapshot The snapshot being restored
     * @param CustomData Custom data to restore
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Timeline Events")
    void RestoreCustomWorldState(const FWorldStateSnapshot& Snapshot, const TMap<FString, FString>& CustomData);
};
