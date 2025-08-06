// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "UI/DialogueWidget.h"
#include "Debug/AIDMDebugWidget.h"
#include "AIDMPlayerCharacter.generated.h"

/**
 * Player alignment tracking
 */
UENUM(BlueprintType)
enum class EPlayerAlignment : uint8
{
    Light       UMETA(DisplayName = "Light Side"),
    Neutral     UMETA(DisplayName = "Neutral"),
    Dark        UMETA(DisplayName = "Dark Side")
};

/**
 * Player stats for AIDM integration
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlayerStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    int32 Level;

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    int32 Experience;

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    int32 LightSidePoints;

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    int32 DarkSidePoints;

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    int32 Credits;

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    TMap<FString, int32> Attributes; // Strength, Dexterity, etc.

    UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
    TMap<FString, int32> Skills; // Persuade, Computer Use, etc.

    FPlayerStats()
    {
        Level = 1;
        Experience = 0;
        LightSidePoints = 0;
        DarkSidePoints = 0;
        Credits = 1000;
    }
};

/**
 * Enhanced Player Character with AIDM integration
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AAIDMPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAIDMPlayerCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    virtual void Tick(float DeltaTime) override;

    /**
     * Initialize AIDM components
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void InitializeAIDMComponents();

    /**
     * Load a campaign and start playing
     * @param CampaignFilePath Path to the campaign JSON file
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    bool LoadAndStartCampaign(const FString& CampaignFilePath);

    /**
     * Interact with an NPC
     * @param NPCData The NPC data to interact with
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void InteractWithNPC(const FNPCData& NPCData);

    /**
     * Complete a quest objective
     * @param QuestID The quest ID
     * @param ObjectiveIndex Index of the objective to complete
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void CompleteQuestObjective(const FString& QuestID, int32 ObjectiveIndex);

    /**
     * Add experience points
     * @param Amount Amount of experience to add
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void AddExperience(int32 Amount);

    /**
     * Add alignment points
     * @param LightPoints Light side points to add
     * @param DarkPoints Dark side points to add
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void AddAlignmentPoints(int32 LightPoints, int32 DarkPoints);

    /**
     * Add credits
     * @param Amount Amount of credits to add
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void AddCredits(int32 Amount);

    /**
     * Get current player alignment
     * @return Current alignment based on light/dark points
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM")
    EPlayerAlignment GetCurrentAlignment() const;

    /**
     * Get player stats
     * @return Current player stats
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM")
    FPlayerStats GetPlayerStats() const { return PlayerStats; }

    /**
     * Toggle debug panel
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void ToggleDebugPanel();

    /**
     * Toggle quest log
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void ToggleQuestLog();

    /**
     * Travel to a specific planet
     * @param PlanetIndex Index of the planet to travel to
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void TravelToPlanet(int32 PlanetIndex);

    /**
     * Travel to a specific layout on current planet
     * @param LayoutName Name of the layout to travel to
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM")
    void TravelToLayout(const FString& LayoutName);

protected:
    // AIDM Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIDM")
    UAIDirectorComponent* AIDirector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIDM")
    UQuestManagerComponent* QuestManager;

    // Player stats
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    FPlayerStats PlayerStats;

    // UI Widgets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UDialogueWidget> DialogueWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UQuestLogWidget> QuestLogWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UAIDMDebugWidget> DebugWidgetClass;

    // Widget instances
    UPROPERTY()
    UDialogueWidget* DialogueWidget;

    UPROPERTY()
    class UQuestLogWidget* QuestLogWidget;

    UPROPERTY()
    UAIDMDebugWidget* DebugWidget;

    // Subsystem references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    // Input settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bDebugModeEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FKey DebugToggleKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FKey QuestLogToggleKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FKey InteractKey;

    // Interaction
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    AActor* CurrentInteractable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionRange;

private:
    // Input handlers
    void OnDebugToggle();
    void OnQuestLogToggle();
    void OnInteract();

    // Helper methods
    void CreateUIWidgets();
    void UpdateInteractable();
    AActor* FindNearestInteractable();
    bool IsActorInteractable(AActor* Actor) const;

    // Event handlers
    UFUNCTION()
    void OnQuestStarted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnQuestCompleted(const FActiveQuest& Quest);

    UFUNCTION()
    void OnCampaignLoaded(const FCampaignPlan& Campaign);

    UFUNCTION()
    void OnPlanetChanged(int32 OldPlanetIndex, int32 NewPlanetIndex);

    UFUNCTION()
    void OnLayoutChanged(const FString& OldLayout, const FString& NewLayout);

public:
    /**
     * Blueprint implementable events for custom player logic
     */

    /**
     * Called when player levels up
     * @param NewLevel The new player level
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Events")
    void OnPlayerLevelUp(int32 NewLevel);

    /**
     * Called when player alignment changes
     * @param NewAlignment The new alignment
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Events")
    void OnAlignmentChanged(EPlayerAlignment NewAlignment);

    /**
     * Called when player interacts with an NPC
     * @param NPCData The NPC being interacted with
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Events")
    void OnNPCInteraction(const FNPCData& NPCData);

    /**
     * Called when player enters a new area
     * @param PlanetData Current planet data
     * @param LayoutData Current layout data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Events")
    void OnAreaEntered(const FPlanetData& PlanetData, const FMapLayout& LayoutData);

    /**
     * Called to get custom interaction text for an actor
     * @param Actor The actor being looked at
     * @return Custom interaction text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Events")
    FString GetCustomInteractionText(AActor* Actor);

    /**
     * Called when player gains experience
     * @param Amount Amount of experience gained
     * @param Source Source of the experience (quest, combat, etc.)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Events")
    void OnExperienceGained(int32 Amount, const FString& Source);
};
