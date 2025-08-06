// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "Companions/CompanionManagerComponent.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "SandboxModeComponent.generated.h"

/**
 * Sandbox command types
 */
UENUM(BlueprintType)
enum class ESandboxCommandType : uint8
{
    Spawn           UMETA(DisplayName = "Spawn Content"),
    Modify          UMETA(DisplayName = "Modify Existing"),
    Event           UMETA(DisplayName = "Trigger Event"),
    Quest           UMETA(DisplayName = "Quest Control"),
    Companion       UMETA(DisplayName = "Companion Action"),
    Environment     UMETA(DisplayName = "Environment Change"),
    Story           UMETA(DisplayName = "Story Injection"),
    Custom          UMETA(DisplayName = "Custom Command")
};

/**
 * Sandbox command data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSandboxCommand
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    FString CommandID;

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    ESandboxCommandType Type;

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    FString Prompt; // Natural language prompt from player

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    FString ProcessedPrompt; // AI-processed version

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    TMap<FString, FString> Parameters; // Extracted parameters

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    float Timestamp; // When command was issued

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    bool bExecuted; // Whether command has been executed

    UPROPERTY(BlueprintReadWrite, Category = "Sandbox Command")
    FString ExecutionResult; // Result of execution

    FSandboxCommand()
    {
        CommandID = TEXT("");
        Type = ESandboxCommandType::Custom;
        Prompt = TEXT("");
        ProcessedPrompt = TEXT("");
        Timestamp = 0.0f;
        bExecuted = false;
        ExecutionResult = TEXT("");
    }
};

/**
 * Sandbox execution result
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FSandboxExecutionResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Execution Result")
    bool bSuccess;

    UPROPERTY(BlueprintReadWrite, Category = "Execution Result")
    FString Message; // Result message

    UPROPERTY(BlueprintReadWrite, Category = "Execution Result")
    TArray<FString> SpawnedActors; // IDs of spawned actors

    UPROPERTY(BlueprintReadWrite, Category = "Execution Result")
    TArray<FString> ModifiedActors; // IDs of modified actors

    UPROPERTY(BlueprintReadWrite, Category = "Execution Result")
    TArray<FString> TriggeredEvents; // Events that were triggered

    UPROPERTY(BlueprintReadWrite, Category = "Execution Result")
    float ExecutionTime; // How long execution took

    FSandboxExecutionResult()
    {
        bSuccess = false;
        Message = TEXT("");
        ExecutionTime = 0.0f;
    }
};

/**
 * Sandbox events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSandboxCommandExecuted, const FSandboxCommand&, Command);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSandboxModeToggled, bool, bEnabled, const FString&, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPromptProcessed, const FString&, OriginalPrompt, const FString&, ProcessedPrompt);

/**
 * Sandbox Mode Component - Allows player-controlled prompt injection for dynamic content generation
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API USandboxModeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USandboxModeComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize sandbox mode with AIDM components
     * @param CampaignLoader Campaign loader for content generation
     * @param AIDirector AI Director for content spawning
     * @param QuestManager Quest manager for quest control
     * @param CompanionManager Companion manager for companion actions
     * @param NarrativeMemory Narrative memory for story context
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    void InitializeSandboxMode(UCampaignLoaderSubsystem* CampaignLoader,
                              UAIDirectorComponent* AIDirector,
                              UQuestManagerComponent* QuestManager,
                              UCompanionManagerComponent* CompanionManager,
                              UNarrativeMemoryComponent* NarrativeMemory);

    /**
     * Enable or disable sandbox mode
     * @param bEnable Whether to enable sandbox mode
     * @param Reason Reason for the change
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    void SetSandboxModeEnabled(bool bEnable, const FString& Reason = TEXT(""));

    /**
     * Check if sandbox mode is enabled
     * @return True if sandbox mode is active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sandbox Mode")
    bool IsSandboxModeEnabled() const { return bSandboxModeEnabled; }

    /**
     * Execute a sandbox command from natural language prompt
     * @param Prompt Natural language prompt from player
     * @return Command ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    FString ExecuteSandboxPrompt(const FString& Prompt);

    /**
     * Execute a predefined sandbox command
     * @param Command The command to execute
     * @return Execution result
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    FSandboxExecutionResult ExecuteSandboxCommand(const FSandboxCommand& Command);

    /**
     * Process natural language prompt into structured command
     * @param Prompt Natural language prompt
     * @return Processed sandbox command
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    FSandboxCommand ProcessPrompt(const FString& Prompt);

    /**
     * Get command history
     * @param MaxCommands Maximum number of commands to return
     * @return Array of recent commands
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sandbox Mode")
    TArray<FSandboxCommand> GetCommandHistory(int32 MaxCommands = 10) const;

    /**
     * Clear command history
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    void ClearCommandHistory();

    /**
     * Undo last command (if possible)
     * @return True if undo was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    bool UndoLastCommand();

    /**
     * Get available sandbox commands
     * @return Array of example commands
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sandbox Mode")
    TArray<FString> GetAvailableCommands() const;

    /**
     * Validate sandbox command before execution
     * @param Command The command to validate
     * @return True if command is valid and safe
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    bool ValidateCommand(const FSandboxCommand& Command) const;

    /**
     * Get sandbox mode statistics
     * @return Map of statistics
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sandbox Mode")
    TMap<FString, int32> GetSandboxStatistics() const;

    /**
     * Save sandbox session data
     * @return Serialized session data
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    FString SaveSandboxSession() const;

    /**
     * Load sandbox session data
     * @param SessionData Serialized session data
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Sandbox Mode")
    bool LoadSandboxSession(const FString& SessionData);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Sandbox Events")
    FOnSandboxCommandExecuted OnSandboxCommandExecuted;

    UPROPERTY(BlueprintAssignable, Category = "Sandbox Events")
    FOnSandboxModeToggled OnSandboxModeToggled;

    UPROPERTY(BlueprintAssignable, Category = "Sandbox Events")
    FOnPromptProcessed OnPromptProcessed;

protected:
    // Sandbox state
    UPROPERTY(BlueprintReadOnly, Category = "Sandbox Mode")
    bool bSandboxModeEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Sandbox Mode")
    TArray<FSandboxCommand> CommandHistory;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UAIDirectorComponent* AIDirectorRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    UPROPERTY()
    UCompanionManagerComponent* CompanionManagerRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    // Sandbox settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandbox Settings")
    bool bRequireConfirmation; // Whether to require confirmation for dangerous commands

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandbox Settings")
    int32 MaxCommandHistory; // Maximum number of commands to keep in history

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandbox Settings")
    float CommandCooldown; // Minimum time between commands

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandbox Settings")
    TArray<FString> RestrictedCommands; // Commands that are not allowed

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandbox Settings")
    bool bLogAllCommands; // Whether to log all commands for debugging

    // Command processing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command Processing")
    TMap<FString, FString> CommandAliases; // Shortcuts for common commands

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command Processing")
    TArray<FString> ExampleCommands; // Example commands for help

    // Execution tracking
    UPROPERTY()
    TMap<FString, FSandboxExecutionResult> ExecutionResults;

    UPROPERTY()
    float LastCommandTime;

private:
    // Command processing methods
    ESandboxCommandType DetermineCommandType(const FString& Prompt) const;
    TMap<FString, FString> ExtractParameters(const FString& Prompt, ESandboxCommandType Type) const;
    FString GenerateCommandID();

    // Command execution methods
    FSandboxExecutionResult ExecuteSpawnCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteModifyCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteEventCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteQuestCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteCompanionCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteEnvironmentCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteStoryCommand(const FSandboxCommand& Command);
    FSandboxExecutionResult ExecuteCustomCommand(const FSandboxCommand& Command);

    // Helper methods
    bool IsCommandRestricted(const FSandboxCommand& Command) const;
    bool IsCommandOnCooldown() const;
    void AddToHistory(const FSandboxCommand& Command);
    void CleanupHistory();

    // Natural language processing helpers
    TArray<FString> TokenizePrompt(const FString& Prompt) const;
    TArray<FString> ExtractEntities(const FString& Prompt) const;
    FString NormalizePrompt(const FString& Prompt) const;

public:
    /**
     * Blueprint implementable events for custom sandbox logic
     */

    /**
     * Called when sandbox mode is toggled (for custom setup/cleanup)
     * @param bEnabled Whether sandbox mode was enabled or disabled
     * @param Reason Reason for the change
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Sandbox Events")
    void OnSandboxModeToggledEvent(bool bEnabled, const FString& Reason);

    /**
     * Called to process custom prompt (override in Blueprint)
     * @param Prompt The natural language prompt
     * @return Custom processed command
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Sandbox Events")
    FSandboxCommand ProcessCustomPrompt(const FString& Prompt);

    /**
     * Called to execute custom command (override in Blueprint)
     * @param Command The command to execute
     * @return Custom execution result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Sandbox Events")
    FSandboxExecutionResult ExecuteCustomCommandEvent(const FSandboxCommand& Command);

    /**
     * Called to validate custom command (override in Blueprint)
     * @param Command The command to validate
     * @return True if command is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Sandbox Events")
    bool ValidateCustomCommand(const FSandboxCommand& Command);

    /**
     * Called when command is executed (for custom effects)
     * @param Command The executed command
     * @param Result The execution result
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Sandbox Events")
    void OnCommandExecutedEvent(const FSandboxCommand& Command, const FSandboxExecutionResult& Result);
};
