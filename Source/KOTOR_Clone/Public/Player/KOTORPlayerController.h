// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/DialogueBoxWidget.h"
#include "UI/GalaxyMapWidget.h"
#include "Photography/CinematicScreenshotNarrator.h"
#include "KOTORPlayerController.generated.h"

/**
 * Input context types
 */
UENUM(BlueprintType)
enum class EInputContext : uint8
{
    Default             UMETA(DisplayName = "Default"),
    Dialogue            UMETA(DisplayName = "Dialogue"),
    GalaxyMap           UMETA(DisplayName = "Galaxy Map"),
    PhotoMode           UMETA(DisplayName = "Photo Mode"),
    Meditation          UMETA(DisplayName = "Meditation"),
    Combat              UMETA(DisplayName = "Combat"),
    Inventory           UMETA(DisplayName = "Inventory"),
    Menu                UMETA(DisplayName = "Menu")
};

/**
 * Input action data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FInputActionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Input Action")
    FString ActionName;

    UPROPERTY(BlueprintReadWrite, Category = "Input Action")
    FString ActionDescription;

    UPROPERTY(BlueprintReadWrite, Category = "Input Action")
    EInputContext Context;

    UPROPERTY(BlueprintReadWrite, Category = "Input Action")
    bool bIsEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Input Action")
    TArray<FKey> BoundKeys;

    FInputActionData()
    {
        ActionName = TEXT("");
        ActionDescription = TEXT("");
        Context = EInputContext::Default;
        bIsEnabled = true;
    }
};

/**
 * Input events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputContextChanged, EInputContext, OldContext, EInputContext, NewContext);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionPromptChanged, const FString&, PromptText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputActionTriggered, const FString&, ActionName, const FInputActionValue&, ActionValue);

/**
 * KOTOR Player Controller - Enhanced input system with context switching
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AKOTORPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AKOTORPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Set input context
     * @param NewContext New input context to activate
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void SetInputContext(EInputContext NewContext);

    /**
     * Get current input context
     * @return Current active input context
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Input")
    EInputContext GetCurrentInputContext() const { return CurrentInputContext; }

    /**
     * Enable/disable input action
     * @param ActionName Name of action to enable/disable
     * @param bEnabled Whether action should be enabled
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void SetInputActionEnabled(const FString& ActionName, bool bEnabled);

    /**
     * Check if input action is enabled
     * @param ActionName Name of action to check
     * @return True if action is enabled
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Input")
    bool IsInputActionEnabled(const FString& ActionName) const;

    /**
     * Set interaction prompt
     * @param PromptText Text to display for interaction
     * @param bVisible Whether prompt should be visible
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void SetInteractionPrompt(const FString& PromptText, bool bVisible = true);

    /**
     * Clear interaction prompt
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void ClearInteractionPrompt();

    /**
     * Show dialogue interface
     * @param SpeakerData Speaker information
     * @param DialogueText Dialogue text
     * @param Choices Available dialogue choices
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void ShowDialogue(const FDialogueSpeaker& SpeakerData, const FString& DialogueText, 
                     const TArray<FDialogueChoice>& Choices);

    /**
     * Hide dialogue interface
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void HideDialogue();

    /**
     * Show galaxy map
     * @param CurrentPlanetID ID of current planet
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void ShowGalaxyMap(const FString& CurrentPlanetID);

    /**
     * Hide galaxy map
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void HideGalaxyMap();

    /**
     * Toggle photo mode
     */
    UFUNCTION(BlueprintCallable, Category = "KOTOR Input")
    void TogglePhotoMode();

    /**
     * Get input action data
     * @param ActionName Name of action
     * @return Input action data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Input")
    FInputActionData GetInputActionData(const FString& ActionName) const;

    /**
     * Get all input actions for context
     * @param Context Input context to get actions for
     * @return Array of input action data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KOTOR Input")
    TArray<FInputActionData> GetInputActionsForContext(EInputContext Context) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "KOTOR Input Events")
    FOnInputContextChanged OnInputContextChanged;

    UPROPERTY(BlueprintAssignable, Category = "KOTOR Input Events")
    FOnInteractionPromptChanged OnInteractionPromptChanged;

    UPROPERTY(BlueprintAssignable, Category = "KOTOR Input Events")
    FOnInputActionTriggered OnInputActionTriggered;

protected:
    // Input contexts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* DefaultInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* DialogueInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* GalaxyMapInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* PhotoModeInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* MeditationInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* CombatInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* InventoryInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Contexts")
    class UInputMappingContext* MenuInputContext;

    // Input actions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* RunAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* OpenGalaxyMapAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* TogglePhotoModeAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* OpenInventoryAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* OpenMenuAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* QuickSaveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Actions")
    class UInputAction* QuickLoadAction;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    EInputContext CurrentInputContext;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    FString CurrentInteractionPrompt;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bInteractionPromptVisible;

    // UI references
    UPROPERTY()
    UDialogueBoxWidget* DialogueWidget;

    UPROPERTY()
    UGalaxyMapWidget* GalaxyMapWidget;

    UPROPERTY()
    UCinematicScreenshotNarrator* ScreenshotNarrator;

    // Input action registry
    UPROPERTY(BlueprintReadOnly, Category = "Input Registry")
    TMap<FString, FInputActionData> InputActionRegistry;

    // Widget classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UDialogueBoxWidget> DialogueWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UGalaxyMapWidget> GalaxyMapWidgetClass;

private:
    // Input action handlers
    void OnMoveTriggered(const FInputActionValue& Value);
    void OnLookTriggered(const FInputActionValue& Value);
    void OnInteractTriggered(const FInputActionValue& Value);
    void OnJumpTriggered(const FInputActionValue& Value);
    void OnRunStarted(const FInputActionValue& Value);
    void OnRunCompleted(const FInputActionValue& Value);
    void OnOpenGalaxyMapTriggered(const FInputActionValue& Value);
    void OnTogglePhotoModeTriggered(const FInputActionValue& Value);
    void OnOpenInventoryTriggered(const FInputActionValue& Value);
    void OnOpenMenuTriggered(const FInputActionValue& Value);
    void OnQuickSaveTriggered(const FInputActionValue& Value);
    void OnQuickLoadTriggered(const FInputActionValue& Value);

    // Context management
    void ApplyInputContext(EInputContext Context);
    void RemoveInputContext(EInputContext Context);
    UInputMappingContext* GetInputMappingContext(EInputContext Context) const;

    // UI management
    void CreateUIWidgets();
    void SetupInputActionRegistry();

    // Helper methods
    void BroadcastInputAction(const FString& ActionName, const FInputActionValue& Value);

public:
    /**
     * Blueprint implementable events for custom input logic
     */

    /**
     * Called when input context changes (for custom setup)
     * @param OldContext Previous input context
     * @param NewContext New input context
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "KOTOR Input Events")
    void OnInputContextChangedEvent(EInputContext OldContext, EInputContext NewContext);

    /**
     * Called when interaction is triggered (for custom interaction logic)
     * @param InteractionTarget Target of interaction
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "KOTOR Input Events")
    void OnInteractionTriggeredEvent(AActor* InteractionTarget);

    /**
     * Called to handle custom input action (override in Blueprint)
     * @param ActionName Name of the action
     * @param ActionValue Value of the action
     * @return True if action was handled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "KOTOR Input Events")
    bool HandleCustomInputAction(const FString& ActionName, const FInputActionValue& ActionValue);

    /**
     * Called to get custom interaction target (override in Blueprint)
     * @return Custom interaction target
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "KOTOR Input Events")
    AActor* GetCustomInteractionTarget();

    /**
     * Called when photo mode is toggled (for custom photo mode setup)
     * @param bEnabled Whether photo mode was enabled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "KOTOR Input Events")
    void OnPhotoModeToggledEvent(bool bEnabled);
};
