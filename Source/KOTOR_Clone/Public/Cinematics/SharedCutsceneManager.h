// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/NetSerialization.h"
#include "Net/UnrealNetwork.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Camera/CameraActor.h"
#include "SharedCutsceneManager.generated.h"

/**
 * Cutscene sync states
 */
UENUM(BlueprintType)
enum class ECutsceneSyncState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    Loading             UMETA(DisplayName = "Loading"),
    WaitingForPlayers   UMETA(DisplayName = "Waiting for Players"),
    Playing             UMETA(DisplayName = "Playing"),
    Paused              UMETA(DisplayName = "Paused"),
    Completed           UMETA(DisplayName = "Completed"),
    Error               UMETA(DisplayName = "Error")
};

/**
 * Player cutscene state
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlayerCutsceneState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player State")
    FString PlayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Player State")
    bool bIsReady;

    UPROPERTY(BlueprintReadWrite, Category = "Player State")
    bool bIsLoaded;

    UPROPERTY(BlueprintReadWrite, Category = "Player State")
    float CurrentTime; // Current playback time

    UPROPERTY(BlueprintReadWrite, Category = "Player State")
    float LastSyncTime; // Last time sync was received

    UPROPERTY(BlueprintReadWrite, Category = "Player State")
    bool bIsBuffering; // Whether player is buffering

    FPlayerCutsceneState()
    {
        PlayerID = TEXT("");
        bIsReady = false;
        bIsLoaded = false;
        CurrentTime = 0.0f;
        LastSyncTime = 0.0f;
        bIsBuffering = false;
    }
};

/**
 * Cutscene session data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCutsceneSession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    FString CutsceneName;

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    TSoftObjectPtr<ULevelSequence> LevelSequence;

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    ECutsceneSyncState SyncState;

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    TArray<FPlayerCutsceneState> PlayerStates;

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    float MasterTime; // Authoritative playback time

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    float Duration; // Total cutscene duration

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    bool bIsPaused;

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    float StartTime; // When cutscene started

    UPROPERTY(BlueprintReadWrite, Category = "Cutscene Session")
    TMap<FString, FString> CutsceneParameters; // Custom parameters

    FCutsceneSession()
    {
        SessionID = TEXT("");
        CutsceneName = TEXT("");
        SyncState = ECutsceneSyncState::Idle;
        MasterTime = 0.0f;
        Duration = 0.0f;
        bIsPaused = false;
        StartTime = 0.0f;
    }
};

/**
 * Cutscene events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCutsceneSessionStarted, const FCutsceneSession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCutscenePlaybackStarted, const FString&, SessionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCutscenePlaybackPaused, const FString&, SessionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCutscenePlaybackCompleted, const FString&, SessionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCutsceneStateChanged, const FString&, PlayerID, const FPlayerCutsceneState&, State);

/**
 * Shared Cutscene Manager - Handles synchronized cutscene playback for multiplayer
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ASharedCutsceneManager : public AActor
{
    GENERATED_BODY()

public:
    ASharedCutsceneManager();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    /**
     * Start shared cutscene
     * @param CutsceneName Name of the cutscene
     * @param LevelSequence Level sequence to play
     * @param Parameters Custom parameters for the cutscene
     * @return Session ID
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Reliable)
    FString StartSharedCutscene(const FString& CutsceneName, ULevelSequence* LevelSequence, 
                               const TMap<FString, FString>& Parameters = TMap<FString, FString>());

    /**
     * Stop current cutscene
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Reliable)
    void StopCurrentCutscene();

    /**
     * Pause/unpause cutscene
     * @param bPause Whether to pause or unpause
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Reliable)
    void PauseCutscene(bool bPause);

    /**
     * Set player ready state
     * @param PlayerID ID of the player
     * @param bReady Whether player is ready
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Reliable)
    void SetPlayerReady(const FString& PlayerID, bool bReady);

    /**
     * Update player cutscene time
     * @param PlayerID ID of the player
     * @param CurrentTime Player's current playback time
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Unreliable)
    void UpdatePlayerTime(const FString& PlayerID, float CurrentTime);

    /**
     * Get current cutscene session
     * @return Current active session
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shared Cutscene")
    FCutsceneSession GetCurrentSession() const { return CurrentSession; }

    /**
     * Check if cutscene is active
     * @return True if a cutscene is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shared Cutscene")
    bool IsCutsceneActive() const { return CurrentSession.SyncState != ECutsceneSyncState::Idle; }

    /**
     * Get player cutscene state
     * @param PlayerID ID of the player
     * @return Player's cutscene state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shared Cutscene")
    FPlayerCutsceneState GetPlayerState(const FString& PlayerID) const;

    /**
     * Get sync tolerance
     * @return Maximum time difference allowed between players
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shared Cutscene")
    float GetSyncTolerance() const { return SyncTolerance; }

    /**
     * Set sync tolerance
     * @param NewTolerance New sync tolerance in seconds
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene")
    void SetSyncTolerance(float NewTolerance);

    /**
     * Force sync all players to master time
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Reliable)
    void ForceSyncAllPlayers();

    /**
     * Skip cutscene (if allowed)
     * @param PlayerID ID of player requesting skip
     * @return True if skip was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Shared Cutscene", Server, Reliable)
    bool SkipCutscene(const FString& PlayerID);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Cutscene Events")
    FOnCutsceneSessionStarted OnCutsceneSessionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Cutscene Events")
    FOnCutscenePlaybackStarted OnCutscenePlaybackStarted;

    UPROPERTY(BlueprintAssignable, Category = "Cutscene Events")
    FOnCutscenePlaybackPaused OnCutscenePlaybackPaused;

    UPROPERTY(BlueprintAssignable, Category = "Cutscene Events")
    FOnCutscenePlaybackCompleted OnCutscenePlaybackCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Cutscene Events")
    FOnPlayerCutsceneStateChanged OnPlayerCutsceneStateChanged;

protected:
    // Current cutscene session
    UPROPERTY(ReplicatedUsing = OnRep_CurrentSession, BlueprintReadOnly, Category = "Shared Cutscene")
    FCutsceneSession CurrentSession;

    // Level sequence player
    UPROPERTY(BlueprintReadOnly, Category = "Shared Cutscene")
    ULevelSequencePlayer* SequencePlayer;

    UPROPERTY(BlueprintReadOnly, Category = "Shared Cutscene")
    ALevelSequenceActor* SequenceActor;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene Settings")
    float SyncTolerance; // Maximum time difference allowed between players

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene Settings")
    float SyncCheckInterval; // How often to check sync (seconds)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene Settings")
    bool bAllowSkipping; // Whether players can skip cutscenes

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene Settings")
    float SkipVoteThreshold; // Percentage of players needed to skip (0.0-1.0)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene Settings")
    bool bWaitForAllPlayers; // Whether to wait for all players before starting

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene Settings")
    float MaxWaitTime; // Maximum time to wait for players

    // Skip voting
    UPROPERTY(BlueprintReadOnly, Category = "Skip Voting")
    TArray<FString> SkipVotes; // Players who voted to skip

    // Timer handles
    FTimerHandle SyncCheckTimer;
    FTimerHandle WaitTimer;

private:
    // Helper methods
    void UpdateMasterTime();
    void CheckPlayerSync();
    bool AllPlayersReady() const;
    bool AllPlayersLoaded() const;
    void StartCutscenePlayback();
    void CompleteCutsceneSession();
    FPlayerCutsceneState* FindPlayerState(const FString& PlayerID);
    void AddPlayerState(const FString& PlayerID);
    void RemovePlayerState(const FString& PlayerID);

    // Skip voting
    void ProcessSkipVote(const FString& PlayerID);
    bool ShouldSkipCutscene() const;

    // Network replication
    UFUNCTION()
    void OnRep_CurrentSession();

    // Server implementations
    FString StartSharedCutscene_Implementation(const FString& CutsceneName, ULevelSequence* LevelSequence, 
                                              const TMap<FString, FString>& Parameters);
    void StopCurrentCutscene_Implementation();
    void PauseCutscene_Implementation(bool bPause);
    void SetPlayerReady_Implementation(const FString& PlayerID, bool bReady);
    void UpdatePlayerTime_Implementation(const FString& PlayerID, float CurrentTime);
    void ForceSyncAllPlayers_Implementation();
    bool SkipCutscene_Implementation(const FString& PlayerID);

    // Timer callbacks
    UFUNCTION()
    void OnSyncCheckTimer();

    UFUNCTION()
    void OnWaitTimer();

    // Sequence events
    UFUNCTION()
    void OnSequenceFinished();

public:
    /**
     * Blueprint implementable events for custom cutscene logic
     */

    /**
     * Called when cutscene session starts (for custom setup)
     * @param Session The cutscene session data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cutscene Events")
    void OnCutsceneSessionStartedEvent(const FCutsceneSession& Session);

    /**
     * Called when cutscene playback starts (for custom effects)
     * @param SessionID The session ID
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cutscene Events")
    void OnCutscenePlaybackStartedEvent(const FString& SessionID);

    /**
     * Called when player state changes (for custom UI updates)
     * @param PlayerID ID of the player
     * @param State New player state
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cutscene Events")
    void OnPlayerCutsceneStateChangedEvent(const FString& PlayerID, const FPlayerCutsceneState& State);

    /**
     * Called to customize cutscene parameters (override in Blueprint)
     * @param CutsceneName Name of the cutscene
     * @param BaseParameters Base parameters
     * @return Customized parameters
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cutscene Events")
    TMap<FString, FString> CustomizeCutsceneParameters(const FString& CutsceneName, const TMap<FString, FString>& BaseParameters);

    /**
     * Called to validate cutscene skip (override in Blueprint)
     * @param PlayerID ID of player requesting skip
     * @param Session Current session
     * @return True if skip should be allowed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Cutscene Events")
    bool ValidateCutsceneSkip(const FString& PlayerID, const FCutsceneSession& Session);
};
