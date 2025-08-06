// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "Politics/FactionDiplomacySystem.h"
#include "NPCs/NPCMemoryMatrixComponent.h"
#include "ProceduralNPCActor.generated.h"

/**
 * NPC roles
 */
UENUM(BlueprintType)
enum class ENPCRole : uint8
{
    Merchant            UMETA(DisplayName = "Merchant"),
    QuestGiver          UMETA(DisplayName = "Quest Giver"),
    Guard               UMETA(DisplayName = "Guard"),
    Civilian            UMETA(DisplayName = "Civilian"),
    Informant           UMETA(DisplayName = "Informant"),
    Trainer             UMETA(DisplayName = "Trainer"),
    Companion           UMETA(DisplayName = "Companion"),
    Leader              UMETA(DisplayName = "Leader"),
    Prisoner            UMETA(DisplayName = "Prisoner"),
    Refugee             UMETA(DisplayName = "Refugee")
};

/**
 * NPC interaction types
 */
UENUM(BlueprintType)
enum class ENPCInteractionType : uint8
{
    Dialogue            UMETA(DisplayName = "Dialogue"),
    Trade               UMETA(DisplayName = "Trade"),
    QuestGive           UMETA(DisplayName = "Quest Give"),
    QuestTurnIn         UMETA(DisplayName = "Quest Turn In"),
    Training            UMETA(DisplayName = "Training"),
    Information         UMETA(DisplayName = "Information"),
    Recruitment         UMETA(DisplayName = "Recruitment"),
    Bribe               UMETA(DisplayName = "Bribe")
};

/**
 * Procedural NPC data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FProceduralNPCData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    FString NPCID;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    FString DisplayName;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    FString Faction;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    ENPCRole Role;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    FString Species;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    FString Appearance; // Description for visual customization

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    TArray<FString> DialogueLines; // Available dialogue options

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    TArray<FString> QuestIDs; // Quests this NPC can give

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    TArray<FString> CompletableQuestIDs; // Quests this NPC can complete

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    TMap<FString, int32> Inventory; // Item -> quantity for merchants

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    int32 Credits; // Available credits for trading

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    TMap<FString, FString> PersonalityTraits; // Trait -> description

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    bool bIsHostile;

    UPROPERTY(BlueprintReadWrite, Category = "NPC Data")
    bool bCanBeRecruited;

    FProceduralNPCData()
    {
        NPCID = TEXT("");
        DisplayName = TEXT("Unknown NPC");
        Faction = TEXT("neutral");
        Role = ENPCRole::Civilian;
        Species = TEXT("human");
        Appearance = TEXT("");
        Credits = 100;
        bIsHostile = false;
        bCanBeRecruited = false;
    }
};

/**
 * NPC dialogue option
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNPCDialogueOption
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Option")
    FString OptionText;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Option")
    FString ResponseText;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Option")
    ENPCInteractionType InteractionType;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Option")
    TMap<FString, FString> Requirements; // Requirement type -> value

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Option")
    TMap<FString, FString> Consequences; // Consequence type -> value

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Option")
    bool bIsAvailable;

    FNPCDialogueOption()
    {
        OptionText = TEXT("Hello");
        ResponseText = TEXT("Greetings, traveler.");
        InteractionType = ENPCInteractionType::Dialogue;
        bIsAvailable = true;
    }
};

/**
 * NPC events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCInteraction, AProceduralNPCActor*, NPC, ENPCInteractionType, InteractionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCDialogueStarted, AProceduralNPCActor*, NPC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCDialogueEnded, AProceduralNPCActor*, NPC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCQuestGiven, AProceduralNPCActor*, NPC, const FString&, QuestID);

/**
 * Procedural NPC Actor - Generic NPC with AIDM integration
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AProceduralNPCActor : public ACharacter
{
    GENERATED_BODY()

public:
    AProceduralNPCActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Initialize NPC with data
     * @param NPCData Data to initialize the NPC with
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    void InitializeNPC(const FProceduralNPCData& NPCData);

    /**
     * Start interaction with player
     * @param PlayerCharacter Player character interacting
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    void StartInteraction(APawn* PlayerCharacter);

    /**
     * End interaction with player
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    void EndInteraction();

    /**
     * Get available dialogue options
     * @param PlayerCharacter Player character for context
     * @return Array of available dialogue options
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    TArray<FNPCDialogueOption> GetAvailableDialogueOptions(APawn* PlayerCharacter) const;

    /**
     * Select dialogue option
     * @param OptionIndex Index of selected option
     * @param PlayerCharacter Player character making selection
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    void SelectDialogueOption(int32 OptionIndex, APawn* PlayerCharacter);

    /**
     * Give quest to player
     * @param QuestID ID of quest to give
     * @param PlayerCharacter Player character receiving quest
     * @return True if quest was given successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    bool GiveQuestToPlayer(const FString& QuestID, APawn* PlayerCharacter);

    /**
     * Complete quest for player
     * @param QuestID ID of quest to complete
     * @param PlayerCharacter Player character completing quest
     * @return True if quest was completed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    bool CompleteQuestForPlayer(const FString& QuestID, APawn* PlayerCharacter);

    /**
     * Update reputation with player
     * @param ReputationChange Change in reputation
     * @param Reason Reason for reputation change
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    void UpdateReputationWithPlayer(int32 ReputationChange, const FString& Reason);

    /**
     * Get NPC data
     * @return Current NPC data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural NPC")
    FProceduralNPCData GetNPCData() const { return NPCData; }

    /**
     * Set NPC hostility
     * @param bHostile Whether NPC should be hostile
     */
    UFUNCTION(BlueprintCallable, Category = "Procedural NPC")
    void SetHostile(bool bHostile);

    /**
     * Check if player can interact
     * @param PlayerCharacter Player character to check
     * @return True if interaction is possible
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural NPC")
    bool CanInteractWithPlayer(APawn* PlayerCharacter) const;

    /**
     * Get interaction prompt text
     * @param PlayerCharacter Player character for context
     * @return Interaction prompt text
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural NPC")
    FString GetInteractionPrompt(APawn* PlayerCharacter) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "NPC Events")
    FOnNPCInteraction OnNPCInteraction;

    UPROPERTY(BlueprintAssignable, Category = "NPC Events")
    FOnNPCDialogueStarted OnNPCDialogueStarted;

    UPROPERTY(BlueprintAssignable, Category = "NPC Events")
    FOnNPCDialogueEnded OnNPCDialogueEnded;

    UPROPERTY(BlueprintAssignable, Category = "NPC Events")
    FOnNPCQuestGiven OnNPCQuestGiven;

protected:
    // NPC data
    UPROPERTY(BlueprintReadOnly, Category = "NPC Data")
    FProceduralNPCData NPCData;

    UPROPERTY(BlueprintReadOnly, Category = "NPC Data")
    TArray<FNPCDialogueOption> CurrentDialogueOptions;

    // Interaction state
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    bool bIsInteracting;

    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    APawn* InteractingPlayer;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* InteractionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* NameplateWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* InteractionPromptWidget;

    // System references
    UPROPERTY()
    UQuestManagerComponent* QuestManager;

    UPROPERTY()
    UFactionDiplomacySystem* FactionSystem;

    UPROPERTY()
    UNPCMemoryMatrixComponent* NPCMemory;

private:
    // Helper methods
    void SetupInteractionSphere();
    void SetupNameplate();
    void UpdateDialogueOptions(APawn* PlayerCharacter);
    bool CheckDialogueRequirements(const FNPCDialogueOption& Option, APawn* PlayerCharacter) const;
    void ApplyDialogueConsequences(const FNPCDialogueOption& Option, APawn* PlayerCharacter);
    void UpdateAppearance();

    // Interaction events
    UFUNCTION()
    void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    /**
     * Blueprint implementable events for custom NPC logic
     */

    /**
     * Called when NPC is initialized (for custom setup)
     * @param InitializedNPCData The NPC data used for initialization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Events")
    void OnNPCInitialized(const FProceduralNPCData& InitializedNPCData);

    /**
     * Called when interaction starts (for custom behavior)
     * @param PlayerCharacter Player starting interaction
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Events")
    void OnInteractionStarted(APawn* PlayerCharacter);

    /**
     * Called when interaction ends (for custom behavior)
     * @param PlayerCharacter Player ending interaction
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Events")
    void OnInteractionEnded(APawn* PlayerCharacter);

    /**
     * Called to generate custom dialogue options (override in Blueprint)
     * @param PlayerCharacter Player character for context
     * @return Custom dialogue options
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Events")
    TArray<FNPCDialogueOption> GenerateCustomDialogueOptions(APawn* PlayerCharacter);

    /**
     * Called to customize NPC appearance (override in Blueprint)
     * @param NPCData The NPC data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Events")
    void CustomizeNPCAppearance(const FProceduralNPCData& NPCData);

    /**
     * Called when dialogue option is selected (for custom responses)
     * @param SelectedOption The selected dialogue option
     * @param PlayerCharacter Player who made the selection
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "NPC Events")
    void OnDialogueOptionSelected(const FNPCDialogueOption& SelectedOption, APawn* PlayerCharacter);
};
