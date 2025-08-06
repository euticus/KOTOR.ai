// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "AIDMDebugWidget.generated.h"

/**
 * AIDM Debug Widget - Development tool for monitoring and controlling the AIDM system
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAIDMDebugWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UAIDMDebugWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize the debug widget with AIDM components
     * @param CampaignLoader Campaign loader subsystem
     * @param AIDirector AI Director component
     * @param QuestManager Quest manager component
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void InitializeDebugWidget(UCampaignLoaderSubsystem* CampaignLoader, 
                              UAIDirectorComponent* AIDirector, 
                              UQuestManagerComponent* QuestManager);

    /**
     * Refresh all debug information
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void RefreshDebugInfo();

    /**
     * Toggle debug widget visibility
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void ToggleDebugWidget();

    /**
     * Load a campaign file for testing
     * @param CampaignFilePath Path to the campaign JSON file
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void LoadCampaignFile(const FString& CampaignFilePath);

    /**
     * Change to a specific planet
     * @param PlanetIndex Index of the planet to change to
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void ChangeToPlanet(int32 PlanetIndex);

    /**
     * Change to a specific layout
     * @param LayoutName Name of the layout to change to
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void ChangeToLayout(const FString& LayoutName);

    /**
     * Force respawn all content
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void ForceRespawnContent();

    /**
     * Clear all spawned content
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void ClearAllContent();

    /**
     * Start a test quest
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void StartTestQuest();

    /**
     * Complete all active quests
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug")
    void CompleteAllQuests();

protected:
    // UI Components (to be bound in Blueprint)
    
    // Campaign Info Section
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Campaign")
    class UTextBlock* CampaignNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Campaign")
    class UTextBlock* CampaignStatsText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Campaign")
    class UComboBoxString* PlanetSelector;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Campaign")
    class UComboBoxString* LayoutSelector;

    // AI Director Section
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Director")
    class UTextBlock* DirectorStatusText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Director")
    class UTextBlock* SpawnPointsText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Director")
    class UTextBlock* SpawnedActorsText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Director")
    class UCheckBox* DebugModeCheckbox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Director")
    class UCheckBox* ShowSpawnPointsCheckbox;

    // Quest Manager Section
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Quests")
    class UTextBlock* QuestStatsText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Quests")
    class UVerticalBox* ActiveQuestsContainer;

    // Control Buttons
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* LoadCampaignButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* RefreshButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* RespawnContentButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* ClearContentButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* StartTestQuestButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* CompleteQuestsButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI|Controls")
    class UButton* ToggleVisibilityButton;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UAIDirectorComponent* AIDirectorRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    // Debug settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bAutoRefresh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    float RefreshInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    FString DefaultCampaignPath;

private:
    // Internal state
    float LastRefreshTime;
    bool bIsInitialized;

    // UI update methods
    void UpdateCampaignInfo();
    void UpdateDirectorInfo();
    void UpdateQuestInfo();
    void UpdatePlanetSelector();
    void UpdateLayoutSelector();
    void UpdateActiveQuestsList();

    // Button event handlers
    UFUNCTION()
    void OnLoadCampaignClicked();

    UFUNCTION()
    void OnRefreshClicked();

    UFUNCTION()
    void OnRespawnContentClicked();

    UFUNCTION()
    void OnClearContentClicked();

    UFUNCTION()
    void OnStartTestQuestClicked();

    UFUNCTION()
    void OnCompleteQuestsClicked();

    UFUNCTION()
    void OnToggleVisibilityClicked();

    UFUNCTION()
    void OnPlanetSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnLayoutSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnDebugModeChanged(bool bIsChecked);

    UFUNCTION()
    void OnShowSpawnPointsChanged(bool bIsChecked);

public:
    /**
     * Blueprint implementable events for custom debug functionality
     */

    /**
     * Called when debug widget is initialized (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Events")
    void OnDebugWidgetInitialized();

    /**
     * Called when campaign is loaded (for custom debug info)
     * @param Campaign The loaded campaign data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Events")
    void OnCampaignLoadedForDebug(const FCampaignPlan& Campaign);

    /**
     * Called when planet changes (for custom debug updates)
     * @param PlanetIndex New planet index
     * @param PlanetData New planet data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Events")
    void OnPlanetChangedForDebug(int32 PlanetIndex, const FPlanetData& PlanetData);

    /**
     * Called to add custom debug information (override in Blueprint)
     * @return Custom debug text to display
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Events")
    FString GetCustomDebugInfo();

    /**
     * Called to perform custom debug actions (override in Blueprint)
     * @param ActionName Name of the custom action
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Events")
    void PerformCustomDebugAction(const FString& ActionName);
};

/**
 * Simple debug console for AIDM commands
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAIDMDebugConsole : public UUserWidget
{
    GENERATED_BODY()

public:
    UAIDMDebugConsole(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;

public:
    /**
     * Execute a debug command
     * @param Command The command to execute
     */
    UFUNCTION(BlueprintCallable, Category = "Debug Console")
    void ExecuteCommand(const FString& Command);

    /**
     * Add a message to the console log
     * @param Message The message to add
     * @param MessageType Type of message (Info, Warning, Error)
     */
    UFUNCTION(BlueprintCallable, Category = "Debug Console")
    void AddConsoleMessage(const FString& Message, const FString& MessageType = TEXT("Info"));

    /**
     * Clear the console log
     */
    UFUNCTION(BlueprintCallable, Category = "Debug Console")
    void ClearConsole();

protected:
    // UI Components
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UVerticalBox* ConsoleLogContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UEditableTextBox* CommandInputBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UButton* ExecuteButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UButton* ClearButton;

    // Component references for command execution
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoader;

    UPROPERTY()
    UAIDirectorComponent* AIDirector;

    UPROPERTY()
    UQuestManagerComponent* QuestManager;

private:
    // Command handlers
    void HandleLoadCommand(const TArray<FString>& Args);
    void HandlePlanetCommand(const TArray<FString>& Args);
    void HandleLayoutCommand(const TArray<FString>& Args);
    void HandleSpawnCommand(const TArray<FString>& Args);
    void HandleQuestCommand(const TArray<FString>& Args);
    void HandleHelpCommand(const TArray<FString>& Args);

    // Button event handlers
    UFUNCTION()
    void OnExecuteClicked();

    UFUNCTION()
    void OnClearClicked();

    UFUNCTION()
    void OnCommandEntered(const FText& Text, ETextCommit::Type CommitMethod);

public:
    /**
     * Set component references for command execution
     */
    UFUNCTION(BlueprintCallable, Category = "Debug Console")
    void SetComponentReferences(UCampaignLoaderSubsystem* InCampaignLoader,
                               UAIDirectorComponent* InAIDirector,
                               UQuestManagerComponent* InQuestManager);
};
