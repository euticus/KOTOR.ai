// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "CampaignSessionManager.generated.h"

/**
 * Player session data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlayerSessionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    FString PlayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    FString PlayerName;

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    int32 PlayerIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    float DecisionWeight; // Influence on group decisions (0.0 to 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    FString PlayerAlignment; // "light", "dark", "neutral"

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    bool bIsGameMaster; // Whether this player has GM privileges

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    bool bIsConnected;

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    float LastActivityTime;

    UPROPERTY(BlueprintReadWrite, Category = "Player Session")
    TMap<FString, FString> PlayerChoices; // Recent choices made by this player

    FPlayerSessionData()
    {
        PlayerID = TEXT("");
        PlayerName = TEXT("Player");
        PlayerIndex = 0;
        DecisionWeight = 1.0f;
        PlayerAlignment = TEXT("neutral");
        bIsGameMaster = false;
        bIsConnected = false;
        LastActivityTime = 0.0f;
    }
};

/**
 * Campaign session state
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCampaignSessionState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    FString CampaignName;

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    int32 CurrentPlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    FString CurrentLayout;

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    TMap<FString, bool> StoryFlags; // Global story state

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    TArray<FString> ActiveQuests; // Quest IDs

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    TArray<FString> CompletedQuests; // Quest IDs

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    float SessionStartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    int32 MaxPlayers;

    UPROPERTY(BlueprintReadWrite, Category = "Session State")
    bool bIsPrivate; // Whether session requires invitation

    FCampaignSessionState()
    {
        SessionID = TEXT("");
        CampaignName = TEXT("");
        CurrentPlanetIndex = 0;
        CurrentLayout = TEXT("");
        SessionStartTime = 0.0f;
        MaxPlayers = 4;
        bIsPrivate = false;
    }
};

/**
 * Dialogue voting data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDialogueVote
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    FString VoteID;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    FString DialogueContext;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    TArray<FString> Options; // Available dialogue options

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    TMap<FString, int32> PlayerVotes; // PlayerID -> Option Index

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    float VoteStartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    float VoteDuration; // How long to wait for votes

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    bool bVoteCompleted;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Vote")
    int32 WinningOption;

    FDialogueVote()
    {
        VoteID = TEXT("");
        DialogueContext = TEXT("");
        VoteStartTime = 0.0f;
        VoteDuration = 30.0f;
        bVoteCompleted = false;
        WinningOption = -1;
    }
};

/**
 * Multiplayer events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, const FPlayerSessionData&, PlayerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, const FPlayerSessionData&, PlayerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionStateChanged, const FCampaignSessionState&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueVoteStarted, const FDialogueVote&, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueVoteCompleted, const FDialogueVote&, Vote);

/**
 * Campaign Session Manager - Manages multiplayer campaign sessions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCampaignSessionManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UCampaignSessionManager();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    /**
     * Host a new campaign session
     * @param CampaignPath Path to the campaign file
     * @param SessionName Name for the session
     * @param MaxPlayers Maximum number of players
     * @param bPrivate Whether session is private
     * @return True if session was created successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session", CallInEditor = true)
    bool HostCampaignSession(const FString& CampaignPath, const FString& SessionName, 
                            int32 MaxPlayers = 4, bool bPrivate = false);

    /**
     * Join an existing campaign session
     * @param SessionID ID of the session to join
     * @param PlayerName Name for this player
     * @return True if successfully joined
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    bool JoinCampaignSession(const FString& SessionID, const FString& PlayerName);

    /**
     * Leave the current session
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    void LeaveCampaignSession();

    /**
     * Add a player to the session
     * @param PlayerController Player controller to add
     * @param PlayerName Name for the player
     * @return Generated player session data
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    FPlayerSessionData AddPlayer(APlayerController* PlayerController, const FString& PlayerName);

    /**
     * Remove a player from the session
     * @param PlayerID ID of the player to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    void RemovePlayer(const FString& PlayerID);

    /**
     * Start a dialogue vote
     * @param DialogueContext Context of the dialogue
     * @param Options Available dialogue options
     * @param VoteDuration How long to wait for votes
     * @return Vote ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    FString StartDialogueVote(const FString& DialogueContext, const TArray<FString>& Options, float VoteDuration = 30.0f);

    /**
     * Cast a vote for dialogue option
     * @param VoteID ID of the vote
     * @param PlayerID ID of the voting player
     * @param OptionIndex Index of chosen option
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    void CastDialogueVote(const FString& VoteID, const FString& PlayerID, int32 OptionIndex);

    /**
     * Set player as Game Master
     * @param PlayerID ID of the player
     * @param bIsGM Whether to grant GM privileges
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    void SetPlayerGameMaster(const FString& PlayerID, bool bIsGM);

    /**
     * Update session state (server only)
     * @param NewState New session state
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session", meta = (CallInEditor = true))
    void UpdateSessionState(const FCampaignSessionState& NewState);

    /**
     * Sync story flags across all players
     * @param StoryFlags Map of story flags to sync
     */
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    void SyncStoryFlags(const TMap<FString, bool>& StoryFlags);

    /**
     * Get current session state
     * @return Current session state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Session")
    FCampaignSessionState GetSessionState() const { return SessionState; }

    /**
     * Get all connected players
     * @return Array of player session data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Session")
    TArray<FPlayerSessionData> GetConnectedPlayers() const;

    /**
     * Get player data by ID
     * @param PlayerID ID of the player
     * @return Player session data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Session")
    FPlayerSessionData GetPlayerData(const FString& PlayerID) const;

    /**
     * Check if player is Game Master
     * @param PlayerID ID of the player
     * @return True if player has GM privileges
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Session")
    bool IsPlayerGameMaster(const FString& PlayerID) const;

    /**
     * Check if session is active
     * @return True if session is running
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Session")
    bool IsSessionActive() const { return bSessionActive; }

    /**
     * Get current dialogue vote
     * @return Current active vote, if any
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Campaign Session")
    FDialogueVote GetCurrentDialogueVote() const { return CurrentDialogueVote; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Session Events")
    FOnPlayerJoined OnPlayerJoined;

    UPROPERTY(BlueprintAssignable, Category = "Session Events")
    FOnPlayerLeft OnPlayerLeft;

    UPROPERTY(BlueprintAssignable, Category = "Session Events")
    FOnSessionStateChanged OnSessionStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Session Events")
    FOnDialogueVoteStarted OnDialogueVoteStarted;

    UPROPERTY(BlueprintAssignable, Category = "Session Events")
    FOnDialogueVoteCompleted OnDialogueVoteCompleted;

protected:
    // Replicated session data
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Campaign Session")
    FCampaignSessionState SessionState;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Campaign Session")
    TArray<FPlayerSessionData> ConnectedPlayers;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Campaign Session")
    FDialogueVote CurrentDialogueVote;

    // Session management
    UPROPERTY(BlueprintReadOnly, Category = "Campaign Session")
    bool bSessionActive;

    UPROPERTY(BlueprintReadOnly, Category = "Campaign Session")
    bool bIsHost;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    UPROPERTY()
    UAIDirectorComponent* AIDirector;

    UPROPERTY()
    UQuestManagerComponent* QuestManager;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemory;

    // Networking
    UPROPERTY()
    TMap<FString, APlayerController*> PlayerControllers;

    // Vote management
    UPROPERTY()
    FTimerHandle VoteTimerHandle;

private:
    // Helper methods
    FString GenerateSessionID();
    FString GeneratePlayerID();
    FString GenerateVoteID();
    void ProcessDialogueVoteResult();
    void CleanupDisconnectedPlayers();
    void BroadcastSessionUpdate();

    // Network RPC functions
    UFUNCTION(Server, Reliable)
    void ServerJoinSession(const FString& PlayerName);

    UFUNCTION(Server, Reliable)
    void ServerLeaveSession();

    UFUNCTION(Server, Reliable)
    void ServerCastVote(const FString& VoteID, int32 OptionIndex);

    UFUNCTION(Client, Reliable)
    void ClientSessionJoined(const FCampaignSessionState& State);

    UFUNCTION(Client, Reliable)
    void ClientSessionLeft();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayerJoined(const FPlayerSessionData& PlayerData);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayerLeft(const FPlayerSessionData& PlayerData);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastSessionStateChanged(const FCampaignSessionState& NewState);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastDialogueVoteStarted(const FDialogueVote& Vote);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastDialogueVoteCompleted(const FDialogueVote& Vote);

public:
    /**
     * Blueprint implementable events for custom session logic
     */

    /**
     * Called when session is created (for custom setup)
     * @param SessionState The created session state
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Session Events")
    void OnSessionCreated(const FCampaignSessionState& SessionState);

    /**
     * Called when player joins (for custom welcome logic)
     * @param PlayerData The joining player data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Session Events")
    void OnPlayerJoinedEvent(const FPlayerSessionData& PlayerData);

    /**
     * Called when dialogue vote is needed (for custom UI)
     * @param Vote The dialogue vote data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Session Events")
    void OnDialogueVoteNeeded(const FDialogueVote& Vote);

    /**
     * Called to resolve vote ties (override in Blueprint)
     * @param Vote The tied vote
     * @return Index of winning option
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Session Events")
    int32 ResolveTiedVote(const FDialogueVote& Vote);
};
