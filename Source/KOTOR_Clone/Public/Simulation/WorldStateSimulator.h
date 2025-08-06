// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Politics/FactionDiplomacySystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "WorldStateSimulator.generated.h"

/**
 * Planet state types
 */
UENUM(BlueprintType)
enum class EPlanetState : uint8
{
    Peaceful        UMETA(DisplayName = "Peaceful"),
    Tense           UMETA(DisplayName = "Tense"),
    Conflict        UMETA(DisplayName = "Conflict"),
    War             UMETA(DisplayName = "War"),
    Occupied        UMETA(DisplayName = "Occupied"),
    Liberated       UMETA(DisplayName = "Liberated"),
    Devastated      UMETA(DisplayName = "Devastated"),
    Rebuilding      UMETA(DisplayName = "Rebuilding")
};

/**
 * Economic conditions
 */
UENUM(BlueprintType)
enum class EEconomicState : uint8
{
    Prosperous      UMETA(DisplayName = "Prosperous"),
    Stable          UMETA(DisplayName = "Stable"),
    Struggling      UMETA(DisplayName = "Struggling"),
    Recession       UMETA(DisplayName = "Recession"),
    Collapse        UMETA(DisplayName = "Collapse"),
    Embargo         UMETA(DisplayName = "Embargo"),
    BlackMarket     UMETA(DisplayName = "Black Market"),
    Rationing       UMETA(DisplayName = "Rationing")
};

/**
 * Weather patterns
 */
UENUM(BlueprintType)
enum class EWeatherPattern : uint8
{
    Normal          UMETA(DisplayName = "Normal"),
    Storm           UMETA(DisplayName = "Storm"),
    Drought         UMETA(DisplayName = "Drought"),
    Flood           UMETA(DisplayName = "Flood"),
    Toxic           UMETA(DisplayName = "Toxic"),
    Volcanic        UMETA(DisplayName = "Volcanic"),
    Frozen          UMETA(DisplayName = "Frozen"),
    Radiation       UMETA(DisplayName = "Radiation")
};

/**
 * Planet world state
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlanetWorldState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    FString PlanetName;

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    FString ControllingFaction; // Current faction in control

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    EPlanetState PoliticalState;

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    EEconomicState EconomicCondition;

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    EWeatherPattern WeatherCondition;

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    float CivilianMorale; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    float ResourceAvailability; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    float SecurityLevel; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    TMap<FString, float> PriceModifiers; // Item type -> price multiplier

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    TArray<FString> ActiveEvents; // Current events affecting planet

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    TArray<FString> RecentHistory; // Recent major events

    UPROPERTY(BlueprintReadWrite, Category = "Planet State")
    float LastUpdateTime;

    FPlanetWorldState()
    {
        PlanetName = TEXT("");
        ControllingFaction = TEXT("neutral");
        PoliticalState = EPlanetState::Peaceful;
        EconomicCondition = EEconomicState::Stable;
        WeatherCondition = EWeatherPattern::Normal;
        CivilianMorale = 0.5f;
        ResourceAvailability = 0.5f;
        SecurityLevel = 0.5f;
        LastUpdateTime = 0.0f;
    }
};

/**
 * Galactic event data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FGalacticEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    FString EventID;

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    FString EventType; // "war", "trade", "disaster", "discovery", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    FString Title;

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    TArray<FString> AffectedPlanets;

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    TArray<FString> AffectedFactions;

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    float Duration; // How long event lasts (hours)

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    float StartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    TMap<FString, float> StateEffects; // State type -> effect magnitude

    UPROPERTY(BlueprintReadWrite, Category = "Galactic Event")
    bool bPlayerTriggered; // Whether player actions caused this

    FGalacticEvent()
    {
        EventID = TEXT("");
        EventType = TEXT("trade");
        Title = TEXT("Galactic Event");
        Description = TEXT("");
        Duration = 24.0f; // 24 hours default
        StartTime = 0.0f;
        bPlayerTriggered = false;
    }
};

/**
 * World simulation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlanetStateChanged, const FPlanetWorldState&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGalacticEventTriggered, const FGalacticEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFactionControlChanged, const FString&, Planet, const FString&, NewFaction);

/**
 * World State Simulator - Simulates dynamic galaxy state with factions, economy, and events
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API UWorldStateSimulator : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Initialize world state simulator
     * @param CampaignLoader Campaign loader for planet data
     * @param FactionSystem Faction diplomacy system
     * @param NarrativeMemory Narrative memory for player actions
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    void InitializeWorldSimulator(UCampaignLoaderSubsystem* CampaignLoader,
                                 UFactionDiplomacySystem* FactionSystem,
                                 UNarrativeMemoryComponent* NarrativeMemory);

    /**
     * Update world simulation
     * Called every real-time hour or major story beat
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    void UpdateWorldSimulation();

    /**
     * Trigger galactic event
     * @param EventType Type of event to trigger
     * @param AffectedPlanets Planets affected by event
     * @param PlayerTriggered Whether player actions caused this
     * @return Generated event data
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    FGalacticEvent TriggerGalacticEvent(const FString& EventType, 
                                       const TArray<FString>& AffectedPlanets, 
                                       bool PlayerTriggered = false);

    /**
     * Update planet state
     * @param PlanetName Name of the planet
     * @param StateChanges Map of state changes to apply
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    void UpdatePlanetState(const FString& PlanetName, const TMap<FString, float>& StateChanges);

    /**
     * Change faction control of planet
     * @param PlanetName Name of the planet
     * @param NewFaction Faction taking control
     * @param Reason Reason for the change
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    void ChangePlanetControl(const FString& PlanetName, const FString& NewFaction, const FString& Reason);

    /**
     * Get planet world state
     * @param PlanetName Name of the planet
     * @return Current world state of the planet
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "World Simulation")
    FPlanetWorldState GetPlanetWorldState(const FString& PlanetName) const;

    /**
     * Get all planet states
     * @return Array of all planet world states
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "World Simulation")
    TArray<FPlanetWorldState> GetAllPlanetStates() const { return PlanetStates; }

    /**
     * Get active galactic events
     * @return Array of currently active events
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "World Simulation")
    TArray<FGalacticEvent> GetActiveGalacticEvents() const;

    /**
     * Get galactic intelligence summary
     * @return LLM-generated summary of current galactic state
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    FString GetGalacticIntelligenceSummary() const;

    /**
     * Generate AIDM context from world state
     * @param PlanetName Planet to generate context for
     * @return Context string for AIDM prompts
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    FString GenerateAIDMWorldContext(const FString& PlanetName) const;

    /**
     * Set simulation speed
     * @param SpeedMultiplier Multiplier for simulation speed (1.0 = normal)
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    void SetSimulationSpeed(float SpeedMultiplier);

    /**
     * Enable/disable automatic simulation
     * @param bEnabled Whether to enable automatic updates
     */
    UFUNCTION(BlueprintCallable, Category = "World Simulation")
    void SetAutomaticSimulationEnabled(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "World Simulation Events")
    FOnPlanetStateChanged OnPlanetStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "World Simulation Events")
    FOnGalacticEventTriggered OnGalacticEventTriggered;

    UPROPERTY(BlueprintAssignable, Category = "World Simulation Events")
    FOnFactionControlChanged OnFactionControlChanged;

protected:
    // World state data
    UPROPERTY(BlueprintReadOnly, Category = "World Simulation")
    TArray<FPlanetWorldState> PlanetStates;

    UPROPERTY(BlueprintReadOnly, Category = "World Simulation")
    TArray<FGalacticEvent> ActiveEvents;

    UPROPERTY(BlueprintReadOnly, Category = "World Simulation")
    TArray<FGalacticEvent> EventHistory;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UFactionDiplomacySystem* FactionSystemRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    // Simulation settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
    bool bAutomaticSimulation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
    float SimulationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Settings")
    float UpdateInterval; // Hours between updates

    // Timer handles
    FTimerHandle SimulationTimer;

    // Event templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Templates")
    TMap<FString, TArray<FGalacticEvent>> EventTemplates;

private:
    // Helper methods
    void LoadPlanetStatesFromCampaign();
    void LoadEventTemplates();
    void ProcessActiveEvents();
    void GenerateRandomEvents();
    void ApplyEventEffects(const FGalacticEvent& Event);
    void UpdatePlanetEconomy(FPlanetWorldState& Planet);
    void UpdatePlanetSecurity(FPlanetWorldState& Planet);
    void UpdateCivilianMorale(FPlanetWorldState& Planet);
    FPlanetWorldState* FindPlanetState(const FString& PlanetName);
    FString GenerateEventID();
    float CalculateEventProbability(const FString& EventType) const;

    // Event handlers
    UFUNCTION()
    void OnMemoryAdded(const FNarrativeMemory& Memory);

    UFUNCTION()
    void OnFactionRelationshipChanged(const FString& FactionA, const FString& FactionB);

public:
    /**
     * Blueprint implementable events for custom simulation logic
     */

    /**
     * Called when planet state changes (for custom effects)
     * @param Planet The planet that changed
     * @param OldState Previous state
     * @param NewState New state
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "World Simulation Events")
    void OnPlanetStateChangedEvent(const FString& Planet, const FPlanetWorldState& OldState, const FPlanetWorldState& NewState);

    /**
     * Called when galactic event is triggered (for custom processing)
     * @param Event The triggered event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "World Simulation Events")
    void OnGalacticEventTriggeredEvent(const FGalacticEvent& Event);

    /**
     * Called to generate custom galactic event (override in Blueprint)
     * @param EventType Type of event to generate
     * @param Context Current galactic context
     * @return Custom generated event
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "World Simulation Events")
    FGalacticEvent GenerateCustomGalacticEvent(const FString& EventType, const FString& Context);

    /**
     * Called to calculate custom planet state changes (override in Blueprint)
     * @param Planet Planet to update
     * @param DeltaTime Time since last update
     * @return Custom state changes
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "World Simulation Events")
    TMap<FString, float> CalculateCustomPlanetChanges(const FPlanetWorldState& Planet, float DeltaTime);
};
