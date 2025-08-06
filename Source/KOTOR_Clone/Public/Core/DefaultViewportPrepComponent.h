// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/EditorPrepUtilityWidget.h"
#include "Audio/MusicDebugPanel.h"
#include "Animation/AnimationTestConsole.h"
#include "DefaultViewportPrepComponent.generated.h"

/**
 * Debug panel types
 */
UENUM(BlueprintType)
enum class EDebugPanelType : uint8
{
    AIDirectorHUD       UMETA(DisplayName = "AI Director HUD"),
    MusicDebugPanel     UMETA(DisplayName = "Music Debug Panel"),
    AnimationTestConsole UMETA(DisplayName = "Animation Test Console"),
    EditorPrepUtility   UMETA(DisplayName = "Editor Prep Utility"),
    PerformanceMonitor  UMETA(DisplayName = "Performance Monitor"),
    GameplayDebugger    UMETA(DisplayName = "Gameplay Debugger"),
    NetworkDebugger     UMETA(DisplayName = "Network Debugger"),
    AudioDebugger       UMETA(DisplayName = "Audio Debugger")
};

/**
 * Debug panel data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDebugPanelData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    EDebugPanelType PanelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    TSoftClassPtr<UUserWidget> WidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    FVector2D Position; // Position on screen (0-1 normalized)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    FVector2D Size; // Size on screen (0-1 normalized)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    bool bVisibleByDefault; // Whether panel is visible by default

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    bool bCanBeMoved; // Whether panel can be moved by user

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    bool bCanBeResized; // Whether panel can be resized by user

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    int32 ZOrder; // Z-order for layering

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Panel")
    FString HotKey; // Hotkey to toggle panel

    FDebugPanelData()
    {
        PanelType = EDebugPanelType::AIDirectorHUD;
        WidgetClass = nullptr;
        Position = FVector2D(0.0f, 0.0f);
        Size = FVector2D(0.3f, 0.4f);
        bVisibleByDefault = false;
        bCanBeMoved = true;
        bCanBeResized = true;
        ZOrder = 0;
        HotKey = TEXT("");
    }
};

/**
 * Viewport prep configuration
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FViewportPrepConfiguration
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    bool bEnableViewportPrep; // Whether viewport prep is enabled

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    TSoftClassPtr<AHUD> DefaultHUDClass; // Default HUD class to set

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    TArray<FDebugPanelData> DebugPanels; // Debug panels to add

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    bool bAutoSetHUD; // Whether to automatically set HUD

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    bool bAutoAddDebugPanels; // Whether to automatically add debug panels

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    bool bOnlyInEditor; // Whether to only prep in editor

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep")
    bool bOnlyInDevelopment; // Whether to only prep in development builds

    FViewportPrepConfiguration()
    {
        bEnableViewportPrep = true;
        DefaultHUDClass = nullptr;
        bAutoSetHUD = true;
        bAutoAddDebugPanels = true;
        bOnlyInEditor = false;
        bOnlyInDevelopment = true;
    }
};

/**
 * Viewport prep events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnViewportPrepCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDSet, AHUD*, NewHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDebugPanelAdded, EDebugPanelType, PanelType, UUserWidget*, Widget);

/**
 * Default Viewport Prep Component - On BeginPlay call ClientSetHUD and add debug panels to viewport
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UDefaultViewportPrepComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDefaultViewportPrepComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    /**
     * Initialize viewport prep
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    void InitializeViewportPrep();

    /**
     * Set HUD class
     * @param HUDClass HUD class to set
     * @return True if HUD was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    bool SetHUDClass(TSubclassOf<AHUD> HUDClass);

    /**
     * Add debug panel to viewport
     * @param PanelData Debug panel data
     * @return Created widget instance
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    UUserWidget* AddDebugPanelToViewport(const FDebugPanelData& PanelData);

    /**
     * Remove debug panel from viewport
     * @param PanelType Type of panel to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    void RemoveDebugPanelFromViewport(EDebugPanelType PanelType);

    /**
     * Toggle debug panel visibility
     * @param PanelType Type of panel to toggle
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    void ToggleDebugPanelVisibility(EDebugPanelType PanelType);

    /**
     * Set debug panel visibility
     * @param PanelType Type of panel
     * @param bVisible Whether panel should be visible
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    void SetDebugPanelVisibility(EDebugPanelType PanelType, bool bVisible);

    /**
     * Get debug panel widget
     * @param PanelType Type of panel to get
     * @return Widget instance or nullptr
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Default Viewport Prep")
    UUserWidget* GetDebugPanelWidget(EDebugPanelType PanelType) const;

    /**
     * Set viewport prep configuration
     * @param Configuration New configuration
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    void SetViewportPrepConfiguration(const FViewportPrepConfiguration& Configuration);

    /**
     * Get viewport prep configuration
     * @return Current configuration
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Default Viewport Prep")
    FViewportPrepConfiguration GetViewportPrepConfiguration() const { return PrepConfiguration; }

    /**
     * Check if viewport prep should run
     * @return True if viewport prep should run
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Default Viewport Prep")
    bool ShouldRunViewportPrep() const;

    /**
     * Get active debug panels
     * @return Array of active debug panel types
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Default Viewport Prep")
    TArray<EDebugPanelType> GetActiveDebugPanels() const;

    /**
     * Clear all debug panels
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    void ClearAllDebugPanels();

    /**
     * Save debug panel layout
     * @param LayoutName Name of layout to save
     * @return True if layout was saved
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    bool SaveDebugPanelLayout(const FString& LayoutName);

    /**
     * Load debug panel layout
     * @param LayoutName Name of layout to load
     * @return True if layout was loaded
     */
    UFUNCTION(BlueprintCallable, Category = "Default Viewport Prep")
    bool LoadDebugPanelLayout(const FString& LayoutName);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Viewport Prep Events")
    FOnViewportPrepCompleted OnViewportPrepCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Viewport Prep Events")
    FOnHUDSet OnHUDSet;

    UPROPERTY(BlueprintAssignable, Category = "Viewport Prep Events")
    FOnDebugPanelAdded OnDebugPanelAdded;

protected:
    // Viewport prep configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewport Prep Configuration")
    FViewportPrepConfiguration PrepConfiguration;

    // Active debug panels
    UPROPERTY(BlueprintReadOnly, Category = "Debug Panels")
    TMap<EDebugPanelType, UUserWidget*> ActiveDebugPanels;

    // Player controller reference
    UPROPERTY()
    APlayerController* PlayerController;

    // State tracking
    UPROPERTY(BlueprintReadOnly, Category = "Prep State")
    bool bViewportPrepCompleted;

    UPROPERTY(BlueprintReadOnly, Category = "Prep State")
    bool bHUDSet;

private:
    // Helper methods
    void LoadDefaultConfiguration();
    void SetupDefaultHUD();
    void AddDefaultDebugPanels();
    UUserWidget* CreateDebugPanelWidget(const FDebugPanelData& PanelData);
    void PositionDebugPanel(UUserWidget* Widget, const FDebugPanelData& PanelData);
    bool IsInEditor() const;
    bool IsDevelopmentBuild() const;

    // Input handling
    void SetupInputBindings();
    void HandleDebugPanelHotkey(EDebugPanelType PanelType);

public:
    /**
     * Blueprint implementable events for custom viewport prep logic
     */

    /**
     * Called when viewport prep begins (for custom initialization)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Viewport Prep Events")
    void OnViewportPrepBeginEvent();

    /**
     * Called when HUD is set (for custom setup)
     * @param NewHUD The new HUD instance
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Viewport Prep Events")
    void OnHUDSetEvent(AHUD* NewHUD);

    /**
     * Called when debug panel is added (for custom setup)
     * @param PanelType Type of panel added
     * @param Widget The widget instance
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Viewport Prep Events")
    void OnDebugPanelAddedEvent(EDebugPanelType PanelType, UUserWidget* Widget);

    /**
     * Called to customize debug panel creation (override in Blueprint)
     * @param PanelData Panel data to customize
     * @return Customized panel data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Viewport Prep Events")
    FDebugPanelData CustomizeDebugPanelCreation(const FDebugPanelData& PanelData);

    /**
     * Called to validate viewport prep (override in Blueprint)
     * @return True if viewport prep should proceed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Viewport Prep Events")
    bool ValidateViewportPrep();
};
