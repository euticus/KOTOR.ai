// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/NetSerialization.h"
#include "Net/UnrealNetwork.h"
#include "UI/DialogueBoxWidget.h"
#include "MultiplayerDecisionSyncComponent.generated.h"

/**
 * Decision sync modes
 */
UENUM(BlueprintType)
enum class EDecisionSyncMode : uint8
{
    Majority            UMETA(DisplayName = "Majority Vote"),
    Unanimous           UMETA(DisplayName = "Unanimous"),
    Leader              UMETA(DisplayName = "Leader Decides"),
    FirstVote           UMETA(DisplayName = "First Vote Wins"),
    Weighted            UMETA(DisplayName = "Weighted Vote"),
    Consensus           UMETA(DisplayName = "Consensus Building")
};

/**
 * Player vote data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlayerVote
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    FString PlayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    FString PlayerName;

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    int32 ChoiceIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    FString ChoiceText;

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    float VoteWeight; // For weighted voting

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    float VoteTime; // When vote was cast

    UPROPERTY(BlueprintReadWrite, Category = "Player Vote")
    bool bIsLeader; // Whether this player is the party leader

    FPlayerVote()
    {
        PlayerID = TEXT("");
        PlayerName = TEXT("");
        ChoiceIndex = -1;
        ChoiceText = TEXT("");
        VoteWeight = 1.0f;
        VoteTime = 0.0f;
        bIsLeader = false;
    }
};

/**
 * Decision session data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDecisionSession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    FString DecisionContext; // What decision is being made

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    TArray<FString> AvailableChoices;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    TArray<FPlayerVote> PlayerVotes;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    EDecisionSyncMode SyncMode;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    float DecisionTimeout; // Seconds before auto-decision

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    float SessionStartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    int32 FinalChoiceIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Decision Session")
    FString FinalChoiceReason; // Why this choice was selected

    FDecisionSession()
    {
        SessionID = TEXT("");
        DecisionContext = TEXT("");
        SyncMode = EDecisionSyncMode::Majority;
        DecisionTimeout = 30.0f;
        SessionStartTime = 0.0f;
        bIsActive = false;
        FinalChoiceIndex = -1;
        FinalChoiceReason = TEXT("");
    }
};

/**
 * Decision sync events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecisionSessionStarted, const FDecisionSession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerVoteCast, const FString&, PlayerID, const FPlayerVote&, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDecisionReached, const FDecisionSession&, Session, int32, FinalChoice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecisionTimeout, const FString&, SessionID);

/**
 * Multiplayer Decision Sync Component - Handles shared dialogue decisions and voting
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMultiplayerDecisionSyncComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMultiplayerDecisionSyncComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    /**
     * Start decision session
     * @param DecisionContext Context for the decision
     * @param Choices Available choices
     * @param SyncMode How to sync the decision
     * @param Timeout Timeout in seconds
     * @return Session ID
     */
    UFUNCTION(BlueprintCallable, Category = "Decision Sync", CallInEditor = true)
    FString StartDecisionSession(const FString& DecisionContext, const TArray<FString>& Choices, 
                                EDecisionSyncMode SyncMode = EDecisionSyncMode::Majority, float Timeout = 30.0f);

    /**
     * Cast vote for current decision
     * @param ChoiceIndex Index of chosen option
     * @param PlayerID ID of voting player
     * @return True if vote was cast successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Decision Sync", Server, Reliable)
    bool CastVote(int32 ChoiceIndex, const FString& PlayerID);

    /**
     * Get current decision session
     * @return Current active session
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Decision Sync")
    FDecisionSession GetCurrentSession() const { return CurrentSession; }

    /**
     * Check if decision session is active
     * @return True if a session is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Decision Sync")
    bool IsDecisionSessionActive() const { return CurrentSession.bIsActive; }

    /**
     * Get vote results
     * @return Array of vote counts per choice
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Decision Sync")
    TArray<int32> GetVoteResults() const;

    /**
     * Get voting progress
     * @return Percentage of players who have voted (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Decision Sync")
    float GetVotingProgress() const;

    /**
     * Force decision completion
     * @param ChoiceIndex Choice to force (optional, -1 = calculate normally)
     */
    UFUNCTION(BlueprintCallable, Category = "Decision Sync", Server, Reliable)
    void ForceDecisionCompletion(int32 ChoiceIndex = -1);

    /**
     * Set party leader
     * @param PlayerID ID of player to make leader
     */
    UFUNCTION(BlueprintCallable, Category = "Decision Sync", Server, Reliable)
    void SetPartyLeader(const FString& PlayerID);

    /**
     * Get party leader
     * @return ID of current party leader
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Decision Sync")
    FString GetPartyLeader() const { return PartyLeaderID; }

    /**
     * Set player vote weight
     * @param PlayerID ID of player
     * @param Weight Vote weight (default 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "Decision Sync")
    void SetPlayerVoteWeight(const FString& PlayerID, float Weight);

    /**
     * Get connected players
     * @return Array of connected player IDs
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Decision Sync")
    TArray<FString> GetConnectedPlayers() const { return ConnectedPlayers; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Decision Sync Events")
    FOnDecisionSessionStarted OnDecisionSessionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Decision Sync Events")
    FOnPlayerVoteCast OnPlayerVoteCast;

    UPROPERTY(BlueprintAssignable, Category = "Decision Sync Events")
    FOnDecisionReached OnDecisionReached;

    UPROPERTY(BlueprintAssignable, Category = "Decision Sync Events")
    FOnDecisionTimeout OnDecisionTimeout;

protected:
    // Current decision session
    UPROPERTY(ReplicatedUsing = OnRep_CurrentSession, BlueprintReadOnly, Category = "Decision Sync")
    FDecisionSession CurrentSession;

    // Connected players
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Decision Sync")
    TArray<FString> ConnectedPlayers;

    // Party leader
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Decision Sync")
    FString PartyLeaderID;

    // Player vote weights
    UPROPERTY(BlueprintReadOnly, Category = "Decision Sync")
    TMap<FString, float> PlayerVoteWeights;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision Sync Settings")
    float DefaultDecisionTimeout;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision Sync Settings")
    EDecisionSyncMode DefaultSyncMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision Sync Settings")
    bool bAutoCompleteOnAllVotes; // Complete when all players vote

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision Sync Settings")
    bool bShowVotingProgress; // Show voting progress to players

    // Timer handle
    FTimerHandle DecisionTimeoutTimer;

private:
    // Helper methods
    void UpdateConnectedPlayers();
    int32 CalculateFinalChoice();
    int32 CalculateMajorityChoice();
    int32 CalculateWeightedChoice();
    bool CheckUnanimousChoice();
    void CompleteDecisionSession(int32 FinalChoice, const FString& Reason);
    FPlayerVote* FindPlayerVote(const FString& PlayerID);
    bool AllPlayersVoted() const;

    // Network replication
    UFUNCTION()
    void OnRep_CurrentSession();

    // Server-side implementations
    bool CastVote_Implementation(int32 ChoiceIndex, const FString& PlayerID);
    void ForceDecisionCompletion_Implementation(int32 ChoiceIndex);
    void SetPartyLeader_Implementation(const FString& PlayerID);

    // Timer callback
    UFUNCTION()
    void OnDecisionTimeout();

public:
    /**
     * Blueprint implementable events for custom decision logic
     */

    /**
     * Called when decision session starts (for custom UI)
     * @param Session The decision session data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Decision Sync Events")
    void OnDecisionSessionStartedEvent(const FDecisionSession& Session);

    /**
     * Called when player casts vote (for custom feedback)
     * @param PlayerID ID of voting player
     * @param Vote The vote data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Decision Sync Events")
    void OnPlayerVoteCastEvent(const FString& PlayerID, const FPlayerVote& Vote);

    /**
     * Called when decision is reached (for custom processing)
     * @param Session The completed session
     * @param FinalChoice The final choice index
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Decision Sync Events")
    void OnDecisionReachedEvent(const FDecisionSession& Session, int32 FinalChoice);

    /**
     * Called to customize decision calculation (override in Blueprint)
     * @param Session Current session data
     * @return Custom final choice index
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Decision Sync Events")
    int32 CalculateCustomDecision(const FDecisionSession& Session);

    /**
     * Called to validate vote (override in Blueprint)
     * @param PlayerID ID of voting player
     * @param ChoiceIndex Choice being voted for
     * @return True if vote is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Decision Sync Events")
    bool ValidatePlayerVote(const FString& PlayerID, int32 ChoiceIndex);

    /**
     * Called to get custom vote weight (override in Blueprint)
     * @param PlayerID ID of player
     * @param Session Current session
     * @return Custom vote weight
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Decision Sync Events")
    float GetCustomVoteWeight(const FString& PlayerID, const FDecisionSession& Session);
};
