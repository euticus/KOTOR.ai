// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/QuestManagerComponent.h"
#include "DialogueWidget.generated.h"

/**
 * Dialogue option data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDialogueOption
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    FString OptionText;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    FString ResponseText;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    bool bEndsDialogue;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    bool bStartsQuest;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    FQuestData QuestToStart;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    FString AlignmentRequirement; // "light", "dark", "neutral", or empty for any

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
    int32 ReputationRequirement; // Minimum reputation required

    FDialogueOption()
    {
        OptionText = TEXT("");
        ResponseText = TEXT("");
        bEndsDialogue = false;
        bStartsQuest = false;
        AlignmentRequirement = TEXT("");
        ReputationRequirement = 0;
    }
};

/**
 * Dialogue events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, const FNPCData&, NPCData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueOptionSelected, int32, OptionIndex, const FDialogueOption&, Option);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestOffered, const FQuestData&, QuestData);

/**
 * Dialogue Widget - Handles NPC dialogue and quest interactions
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UDialogueWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UDialogueWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    /**
     * Start dialogue with an NPC
     * @param NPCData The NPC data from AIDM
     * @param QuestManager Quest manager component for quest handling
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void StartDialogue(const FNPCData& NPCData, UQuestManagerComponent* QuestManager);

    /**
     * End the current dialogue
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void EndDialogue();

    /**
     * Select a dialogue option
     * @param OptionIndex Index of the selected option
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SelectDialogueOption(int32 OptionIndex);

    /**
     * Check if dialogue is currently active
     * @return True if dialogue is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
    bool IsDialogueActive() const { return bIsDialogueActive; }

    /**
     * Get the current NPC data
     * @return Current NPC data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
    FNPCData GetCurrentNPCData() const { return CurrentNPCData; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueStarted OnDialogueStarted;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueEnded OnDialogueEnded;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueOptionSelected OnDialogueOptionSelected;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnQuestOffered OnQuestOffered;

protected:
    // UI Components (to be bound in Blueprint)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* NPCNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* DialogueText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UVerticalBox* OptionsContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UButton* EndDialogueButton;

    // Current dialogue state
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    bool bIsDialogueActive;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    FNPCData CurrentNPCData;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    TArray<FDialogueOption> CurrentOptions;

    UPROPERTY()
    UQuestManagerComponent* CurrentQuestManager;

    // Option button class for creating dialogue options
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|UI")
    TSubclassOf<class UButton> OptionButtonClass;

private:
    // Internal methods
    void GenerateDialogueOptions();
    void CreateOptionButton(const FDialogueOption& Option, int32 OptionIndex);
    void ClearOptionButtons();
    FString GetGreetingText() const;
    bool IsOptionAvailable(const FDialogueOption& Option) const;

    UFUNCTION()
    void OnEndDialogueClicked();

public:
    /**
     * Blueprint implementable events for custom dialogue logic
     */

    /**
     * Called when dialogue starts (for custom setup)
     * @param NPCData The NPC starting dialogue
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void OnDialogueStartedEvent(const FNPCData& NPCData);

    /**
     * Called when dialogue ends (for custom cleanup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void OnDialogueEndedEvent();

    /**
     * Called when a dialogue option is selected (for custom logic)
     * @param OptionIndex The selected option index
     * @param Option The selected option data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void OnDialogueOptionSelectedEvent(int32 OptionIndex, const FDialogueOption& Option);

    /**
     * Called to generate custom dialogue options (override in Blueprint)
     * @param NPCData The current NPC data
     * @return Array of custom dialogue options
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    TArray<FDialogueOption> GenerateCustomDialogueOptions(const FNPCData& NPCData);

    /**
     * Called to get custom greeting text (override in Blueprint)
     * @param NPCData The current NPC data
     * @return Custom greeting text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    FString GetCustomGreetingText(const FNPCData& NPCData);
};

/**
 * Simple Quest Log Widget for displaying active quests
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UQuestLogWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UQuestLogWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;

public:
    /**
     * Refresh the quest log display
     * @param QuestManager Quest manager component to get quest data from
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void RefreshQuestLog(UQuestManagerComponent* QuestManager);

    /**
     * Toggle quest log visibility
     */
    UFUNCTION(BlueprintCallable, Category = "Quest Log")
    void ToggleQuestLog();

protected:
    // UI Components (to be bound in Blueprint)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UVerticalBox* QuestListContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* QuestCountText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UButton* ToggleButton;

    // Quest entry widget class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Log|UI")
    TSubclassOf<UUserWidget> QuestEntryWidgetClass;

private:
    void CreateQuestEntry(const FActiveQuest& Quest);
    void ClearQuestEntries();

    UFUNCTION()
    void OnToggleClicked();

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when quest log is refreshed (for custom logic)
     * @param ActiveQuestCount Number of active quests
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Log Events")
    void OnQuestLogRefreshed(int32 ActiveQuestCount);

    /**
     * Called to create custom quest entry widget (override in Blueprint)
     * @param Quest The quest data
     * @return Custom quest entry widget
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Quest Log Events")
    UUserWidget* CreateCustomQuestEntry(const FActiveQuest& Quest);
};
