// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/ProgressBar.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "AIDMDebugPanel.generated.h"

/**
 * Debug panel sections
 */
UENUM(BlueprintType)
enum class EDebugPanelSection : uint8
{
    CampaignState       UMETA(DisplayName = "Campaign State"),
    QuestSystem         UMETA(DisplayName = "Quest System"),
    NPCSystem           UMETA(DisplayName = "NPC System"),
    EnemySystem         UMETA(DisplayName = "Enemy System"),
    LootSystem          UMETA(DisplayName = "Loot System"),
    WorldSimulation     UMETA(DisplayName = "World Simulation"),
    PlayerStats         UMETA(DisplayName = "Player Stats"),
    SystemPerformance   UMETA(DisplayName = "System Performance"),
    DevTools            UMETA(DisplayName = "Developer Tools")
};

/**
 * AIDM system status
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAIDMSystemStatus
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "System Status")
    FString SystemName;

    UPROPERTY(BlueprintReadWrite, Category = "System Status")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "System Status")
    bool bIsHealthy;

    UPROPERTY(BlueprintReadWrite, Category = "System Status")
    FString StatusMessage;

    UPROPERTY(BlueprintReadWrite, Category = "System Status")
    float PerformanceMetric; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "System Status")
    int32 ActiveElements; // Number of active elements

    FAIDMSystemStatus()
    {
        SystemName = TEXT("Unknown System");
        bIsActive = false;
        bIsHealthy = true;
        StatusMessage = TEXT("OK");
        PerformanceMetric = 1.0f;
        ActiveElements = 0;
    }
};

/**
 * Debug command data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDebugCommand
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Debug Command")
    FString CommandID;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Command")
    FString CommandName;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Command")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Command")
    TArray<FString> Parameters;

    UPROPERTY(BlueprintReadWrite, Category = "Debug Command")
    bool bRequiresConfirmation;

    FDebugCommand()
    {
        CommandID = TEXT("");
        CommandName = TEXT("Unknown Command");
        Description = TEXT("");
        bRequiresConfirmation = false;
    }
};

/**
 * Debug panel events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDebugCommandExecuted, const FString&, CommandID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDebugPanelSectionChanged, EDebugPanelSection, NewSection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSystemStatusChanged, const FString&, SystemName, bool, bIsHealthy);

/**
 * AIDM Debug Panel - Developer debug interface for AIDM systems
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAIDMDebugPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    UAIDMDebugPanel(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize debug panel
     * @param CampaignLoader Campaign loader subsystem
     * @param AIDirector AI Director component
     * @param QuestManager Quest manager component
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void InitializeDebugPanel(UCampaignLoaderSubsystem* CampaignLoader, 
                             UAIDirectorComponent* AIDirector, 
                             UQuestManagerComponent* QuestManager);

    /**
     * Toggle debug panel visibility
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void ToggleDebugPanel();

    /**
     * Show debug panel
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void ShowDebugPanel();

    /**
     * Hide debug panel
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void HideDebugPanel();

    /**
     * Switch to debug section
     * @param Section Section to switch to
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void SwitchToSection(EDebugPanelSection Section);

    /**
     * Execute debug command
     * @param CommandID ID of command to execute
     * @param Parameters Command parameters
     * @return True if command was executed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    bool ExecuteDebugCommand(const FString& CommandID, const TArray<FString>& Parameters);

    /**
     * Refresh debug data
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void RefreshDebugData();

    /**
     * Get system status
     * @param SystemName Name of system to check
     * @return System status data
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    FAIDMSystemStatus GetSystemStatus(const FString& SystemName) const;

    /**
     * Get all system statuses
     * @return Array of all system statuses
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM Debug Panel")
    TArray<FAIDMSystemStatus> GetAllSystemStatuses() const { return SystemStatuses; }

    /**
     * Add custom debug command
     * @param Command Debug command to add
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void AddCustomDebugCommand(const FDebugCommand& Command);

    /**
     * Remove debug command
     * @param CommandID ID of command to remove
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Debug Panel")
    void RemoveDebugCommand(const FString& CommandID);

    /**
     * Check if debug panel is visible
     * @return True if debug panel is currently visible
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM Debug Panel")
    bool IsDebugPanelVisible() const { return bIsVisible; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Debug Panel Events")
    FOnDebugCommandExecuted OnDebugCommandExecuted;

    UPROPERTY(BlueprintAssignable, Category = "Debug Panel Events")
    FOnDebugPanelSectionChanged OnDebugPanelSectionChanged;

    UPROPERTY(BlueprintAssignable, Category = "Debug Panel Events")
    FOnSystemStatusChanged OnSystemStatusChanged;

protected:
    // UI Components
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* MainContainer;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SectionTabs;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ContentScrollBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* SystemStatusContainer;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* DebugCommandsContainer;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentCampaignText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentPlanetText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerAlignmentText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SystemHealthBar;

    UPROPERTY(meta = (BindWidget))
    UButton* RefreshButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CloseButton;

    // Section buttons
    UPROPERTY(meta = (BindWidget))
    UButton* CampaignStateButton;

    UPROPERTY(meta = (BindWidget))
    UButton* QuestSystemButton;

    UPROPERTY(meta = (BindWidget))
    UButton* NPCSystemButton;

    UPROPERTY(meta = (BindWidget))
    UButton* EnemySystemButton;

    UPROPERTY(meta = (BindWidget))
    UButton* LootSystemButton;

    UPROPERTY(meta = (BindWidget))
    UButton* WorldSimulationButton;

    UPROPERTY(meta = (BindWidget))
    UButton* PlayerStatsButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SystemPerformanceButton;

    UPROPERTY(meta = (BindWidget))
    UButton* DevToolsButton;

    // Debug command controls
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* CommandComboBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* CommandParametersTextBox;

    UPROPERTY(meta = (BindWidget))
    UButton* ExecuteCommandButton;

    // Debug panel state
    UPROPERTY(BlueprintReadOnly, Category = "Debug Panel State")
    bool bIsVisible;

    UPROPERTY(BlueprintReadOnly, Category = "Debug Panel State")
    EDebugPanelSection CurrentSection;

    UPROPERTY(BlueprintReadOnly, Category = "Debug Panel State")
    TArray<FAIDMSystemStatus> SystemStatuses;

    UPROPERTY(BlueprintReadOnly, Category = "Debug Panel State")
    TArray<FDebugCommand> DebugCommands;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UAIDirectorComponent* AIDirectorRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    // Update timer
    FTimerHandle UpdateTimer;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel Settings")
    float UpdateInterval; // Seconds between updates

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel Settings")
    bool bAutoRefresh;

private:
    // Helper methods
    void SetupDebugCommands();
    void UpdateSystemStatuses();
    void UpdateCampaignState();
    void UpdateQuestSystem();
    void UpdateNPCSystem();
    void UpdateEnemySystem();
    void UpdateLootSystem();
    void UpdateWorldSimulation();
    void UpdatePlayerStats();
    void UpdateSystemPerformance();
    void UpdateDevTools();

    void PopulateCommandComboBox();
    void CreateSystemStatusWidget(const FAIDMSystemStatus& Status);
    void CreateDebugCommandWidget(const FDebugCommand& Command);

    // Built-in debug commands
    bool ExecuteForceQuest();
    bool ExecuteForceEnemyWave();
    bool ExecuteInjectHallucination();
    bool ExecuteSpawnTestNPC();
    bool ExecuteSpawnTestEnemy();
    bool ExecuteSpawnTestLoot();
    bool ExecuteReloadCampaign();
    bool ExecuteResetPlayerStats();
    bool ExecuteToggleGodMode();
    bool ExecuteToggleDebugDraw();
    bool ExecuteClearLog();

    // UI event handlers
    UFUNCTION()
    void OnRefreshButtonClicked();

    UFUNCTION()
    void OnCloseButtonClicked();

    UFUNCTION()
    void OnExecuteCommandButtonClicked();

    UFUNCTION()
    void OnSectionButtonClicked();

    // Timer callback
    UFUNCTION()
    void OnUpdateTimer();

public:
    /**
     * Blueprint implementable events for custom debug logic
     */

    /**
     * Called when debug panel is shown (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Panel Events")
    void OnDebugPanelShownEvent();

    /**
     * Called when debug panel section changes (for custom updates)
     * @param NewSection The new active section
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Panel Events")
    void OnDebugPanelSectionChangedEvent(EDebugPanelSection NewSection);

    /**
     * Called when debug command is executed (for custom processing)
     * @param CommandID ID of executed command
     * @param Parameters Command parameters
     * @param bSuccess Whether command was successful
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Panel Events")
    void OnDebugCommandExecutedEvent(const FString& CommandID, const TArray<FString>& Parameters, bool bSuccess);

    /**
     * Called to execute custom debug command (override in Blueprint)
     * @param CommandID ID of command to execute
     * @param Parameters Command parameters
     * @return True if command was handled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Panel Events")
    bool ExecuteCustomDebugCommand(const FString& CommandID, const TArray<FString>& Parameters);

    /**
     * Called to get custom system status (override in Blueprint)
     * @param SystemName Name of system to check
     * @return Custom system status
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Panel Events")
    FAIDMSystemStatus GetCustomSystemStatus(const FString& SystemName);

    /**
     * Called to customize debug panel appearance (override in Blueprint)
     * @param Section Current active section
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Debug Panel Events")
    void CustomizeDebugPanelAppearance(EDebugPanelSection Section);
};
