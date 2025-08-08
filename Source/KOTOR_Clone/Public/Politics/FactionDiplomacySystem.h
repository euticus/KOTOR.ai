// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "FactionDiplomacySystem.generated.h"

/**
 * Faction types
 */
UENUM(BlueprintType)
enum class EFactionType : uint8
{
    Government      UMETA(DisplayName = "Government"),
    Military        UMETA(DisplayName = "Military"),
    Criminal        UMETA(DisplayName = "Criminal"),
    Religious       UMETA(DisplayName = "Religious"),
    Corporate       UMETA(DisplayName = "Corporate"),
    Rebel           UMETA(DisplayName = "Rebel"),
    Mercenary       UMETA(DisplayName = "Mercenary"),
    Academic        UMETA(DisplayName = "Academic"),
    Tribal          UMETA(DisplayName = "Tribal"),
    Cult            UMETA(DisplayName = "Cult")
};

/**
 * Diplomatic stance
 */
UENUM(BlueprintType)
enum class EDiplomaticStance : uint8
{
    Hostile         UMETA(DisplayName = "Hostile"),
    Unfriendly      UMETA(DisplayName = "Unfriendly"),
    Neutral         UMETA(DisplayName = "Neutral"),
    Friendly        UMETA(DisplayName = "Friendly"),
    Allied          UMETA(DisplayName = "Allied"),
    Vassal          UMETA(DisplayName = "Vassal")
};

/**
 * Faction data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FFactionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    FString FactionID;

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    FString FactionName;

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    EFactionType Type;

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    FString LeaderName;

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    FString HomePlanet;

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    TArray<FString> ControlledTerritories; // Planets/layouts controlled

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    int32 Influence; // 0-100 influence level

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    int32 MilitaryStrength; // 0-100 military power

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    int32 EconomicPower; // 0-100 economic strength

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    TArray<FString> Goals; // Faction objectives

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    TArray<FString> Resources; // Available resources

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    TMap<FString, FString> Ideology; // Belief system

    UPROPERTY(BlueprintReadWrite, Category = "Faction")
    bool bIsActive; // Whether faction is currently active

    FFactionData()
    {
        FactionID = TEXT("");
        FactionName = TEXT("Unknown Faction");
        Type = EFactionType::Government;
        Description = TEXT("");
        LeaderName = TEXT("");
        HomePlanet = TEXT("");
        Influence = 50;
        MilitaryStrength = 50;
        EconomicPower = 50;
        bIsActive = true;
    }
};

/**
 * Diplomatic relationship
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDiplomaticRelationship
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    FString FactionA;

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    FString FactionB;

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    EDiplomaticStance Stance;

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    int32 RelationshipValue; // -100 to 100

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    TArray<FString> Treaties; // Active treaties

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    TArray<FString> Conflicts; // Active conflicts

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    TArray<FString> SharedHistory; // Historical events

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Relationship")
    float LastInteraction; // Timestamp of last interaction

    FDiplomaticRelationship()
    {
        FactionA = TEXT("");
        FactionB = TEXT("");
        Stance = EDiplomaticStance::Neutral;
        RelationshipValue = 0;
        LastInteraction = 0.0f;
    }
};

/**
 * Player reputation with faction
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlayerReputation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    FString FactionID;

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    int32 ReputationValue; // -100 to 100

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    FString ReputationTitle; // "Enemy", "Neutral", "Friend", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    TArray<FString> ReputationSources; // What affected reputation

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    TMap<FString, int32> ServiceRecord; // Service type -> count

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    bool bHasSpecialStatus; // Whether player has special rank/status

    UPROPERTY(BlueprintReadWrite, Category = "Player Reputation")
    FString SpecialStatus; // Special rank or title

    FPlayerReputation()
    {
        FactionID = TEXT("");
        ReputationValue = 0;
        ReputationTitle = TEXT("Unknown");
        bHasSpecialStatus = false;
        SpecialStatus = TEXT("");
    }
};

/**
 * Diplomatic action
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDiplomaticAction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    FString ActionID;

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    FString ActionType; // "treaty", "trade", "war", "alliance", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    FString InitiatorFaction;

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    FString TargetFaction;

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    TMap<FString, FString> Terms; // Action terms and conditions

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    bool bRequiresPlayerApproval; // Whether player must approve

    UPROPERTY(BlueprintReadWrite, Category = "Diplomatic Action")
    float ExpirationTime; // When action expires

    FDiplomaticAction()
    {
        ActionID = TEXT("");
        ActionType = TEXT("treaty");
        InitiatorFaction = TEXT("");
        TargetFaction = TEXT("");
        bRequiresPlayerApproval = false;
        ExpirationTime = 0.0f;
    }
};

/**
 * Faction events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiplomaticStanceChanged, const FString&, FactionA, const FString&, FactionB);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReputationChanged, const FString&, FactionID, int32, NewReputation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiplomaticActionProposed, const FDiplomaticAction&, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTerritoryChanged, const FString&, Territory, const FString&, NewController);

/**
 * Faction Diplomacy System - Manages political relationships and territory control
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UFactionDiplomacySystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UFactionDiplomacySystem();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize faction diplomacy system
     * @param CampaignLoader Campaign loader for faction data
     * @param NarrativeMemory Narrative memory for tracking player actions
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void InitializeDiplomacySystem(UCampaignLoaderSubsystem* CampaignLoader, 
                                  UNarrativeMemoryComponent* NarrativeMemory);

    /**
     * Add faction to the system
     * @param FactionData Faction data to add
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void AddFaction(const FFactionData& FactionData);

    /**
     * Remove faction from the system
     * @param FactionID ID of faction to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void RemoveFaction(const FString& FactionID);

    /**
     * Update diplomatic relationship
     * @param FactionA First faction
     * @param FactionB Second faction
     * @param RelationshipChange Change in relationship value
     * @param Reason Reason for the change
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void UpdateDiplomaticRelationship(const FString& FactionA, const FString& FactionB, 
                                     int32 RelationshipChange, const FString& Reason);

    /**
     * Update player reputation with faction
     * @param FactionID ID of the faction
     * @param ReputationChange Change in reputation value
     * @param Source Source of reputation change
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void UpdatePlayerReputation(const FString& FactionID, int32 ReputationChange, const FString& Source);

    /**
     * Propose diplomatic action
     * @param Action Diplomatic action to propose
     * @return Action ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    FString ProposeDiplomaticAction(const FDiplomaticAction& Action);

    /**
     * Approve diplomatic action
     * @param ActionID ID of action to approve
     * @param bApproved Whether action is approved
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void ApproveDiplomaticAction(const FString& ActionID, bool bApproved);

    /**
     * Transfer territory control
     * @param Territory Territory to transfer
     * @param FromFaction Current controlling faction
     * @param ToFaction New controlling faction
     * @param Reason Reason for transfer
     */
    UFUNCTION(BlueprintCallable, Category = "Faction Diplomacy")
    void TransferTerritoryControl(const FString& Territory, const FString& FromFaction, 
                                 const FString& ToFaction, const FString& Reason);

    /**
     * Get faction data
     * @param FactionID ID of the faction
     * @return Faction data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    FFactionData GetFactionData(const FString& FactionID) const;

    /**
     * Get diplomatic relationship
     * @param FactionA First faction
     * @param FactionB Second faction
     * @return Diplomatic relationship data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    FDiplomaticRelationship GetDiplomaticRelationship(const FString& FactionA, const FString& FactionB) const;

    /**
     * Get player reputation with faction
     * @param FactionID ID of the faction
     * @return Player reputation data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    FPlayerReputation GetPlayerReputation(const FString& FactionID) const;

    /**
     * Get all factions
     * @return Array of all faction data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    TArray<FFactionData> GetAllFactions() const { return Factions; }

    /**
     * Get factions controlling territory
     * @param Territory Territory to check
     * @return Array of controlling factions
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    TArray<FString> GetTerritoryControllers(const FString& Territory) const;

    /**
     * Get pending diplomatic actions
     * @return Array of pending actions
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    TArray<FDiplomaticAction> GetPendingDiplomaticActions() const { return PendingActions; }

    /**
     * Check if factions are at war
     * @param FactionA First faction
     * @param FactionB Second faction
     * @return True if factions are at war
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    bool AreFactionsAtWar(const FString& FactionA, const FString& FactionB) const;

    /**
     * Get faction influence on planet
     * @param FactionID ID of the faction
     * @param Planet Planet to check
     * @return Influence level (0-100)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Diplomacy")
    int32 GetFactionInfluenceOnPlanet(const FString& FactionID, const FString& Planet) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Faction Events")
    FOnDiplomaticStanceChanged OnDiplomaticStanceChanged;

    UPROPERTY(BlueprintAssignable, Category = "Faction Events")
    FOnPlayerReputationChanged OnPlayerReputationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Faction Events")
    FOnDiplomaticActionProposed OnDiplomaticActionProposed;

    UPROPERTY(BlueprintAssignable, Category = "Faction Events")
    FOnTerritoryChanged OnTerritoryChanged;

protected:
    // Faction data
    UPROPERTY(BlueprintReadOnly, Category = "Faction Diplomacy")
    TArray<FFactionData> Factions;

    UPROPERTY(BlueprintReadOnly, Category = "Faction Diplomacy")
    TArray<FDiplomaticRelationship> DiplomaticRelationships;

    UPROPERTY(BlueprintReadOnly, Category = "Faction Diplomacy")
    TArray<FPlayerReputation> PlayerReputations;

    UPROPERTY(BlueprintReadOnly, Category = "Faction Diplomacy")
    TArray<FDiplomaticAction> PendingActions;

    // Territory control
    UPROPERTY(BlueprintReadOnly, Category = "Faction Diplomacy")
    /*
     * NOTE:
     * UPROPERTY does not support nested containers such as
     * `TMap<Key, TArray<Value>>`.  Keeping territory control in that shape
     * causes Unreal Header Tool compilation failures.
     *
     * We instead store the data in a flat array of strings using the format
     * "TerritoryID::FactionID".  Helper functions inside the diplomacy system
     * can reconstruct the map view (Territory -> Factions) at runtime when
     * needed.
     */
    TArray<FString> TerritoryControl; 

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    // Diplomacy settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diplomacy Settings")
    bool bAutomaticDiplomacy; // Whether AI handles diplomacy automatically

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Diplomacy Settings")
    float DiplomacyUpdateInterval; // Seconds between diplomacy updates

    // Timer handles
    FTimerHandle DiplomacyTimer;

private:
    // Helper methods
    void LoadFactionsFromCampaign();
    void InitializeDiplomaticRelationships();
    FFactionData* FindFaction(const FString& FactionID);
    FDiplomaticRelationship* FindRelationship(const FString& FactionA, const FString& FactionB);
    FPlayerReputation* FindPlayerReputation(const FString& FactionID);
    FDiplomaticAction* FindAction(const FString& ActionID);
    EDiplomaticStance CalculateDiplomaticStance(int32 RelationshipValue);
    FString CalculateReputationTitle(int32 ReputationValue);
    void ProcessAutomaticDiplomacy();
    FString GenerateActionID();

    // Event handlers
    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

public:
    /**
     * Blueprint implementable events for custom diplomacy logic
     */

    /**
     * Called when diplomatic stance changes (for custom effects)
     * @param FactionA First faction
     * @param FactionB Second faction
     * @param OldStance Previous stance
     * @param NewStance New stance
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Faction Events")
    void OnDiplomaticStanceChangedEvent(const FString& FactionA, const FString& FactionB, 
                                       EDiplomaticStance OldStance, EDiplomaticStance NewStance);

    /**
     * Called when player reputation changes (for custom effects)
     * @param FactionID The faction
     * @param OldReputation Previous reputation
     * @param NewReputation New reputation
     * @param Source Source of change
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Faction Events")
    void OnPlayerReputationChangedEvent(const FString& FactionID, int32 OldReputation, 
                                       int32 NewReputation, const FString& Source);

    /**
     * Called to process custom diplomatic action (override in Blueprint)
     * @param Action The diplomatic action
     * @return True if action was processed successfully
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Faction Events")
    bool ProcessCustomDiplomaticAction(const FDiplomaticAction& Action);

    /**
     * Called to generate automatic diplomatic actions (override in Blueprint)
     * @param FactionA First faction
     * @param FactionB Second faction
     * @return Generated diplomatic action
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Faction Events")
    FDiplomaticAction GenerateAutomaticDiplomaticAction(const FString& FactionA, const FString& FactionB);
};
