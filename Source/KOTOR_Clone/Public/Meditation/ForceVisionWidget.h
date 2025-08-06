// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Animation/WidgetAnimation.h"
#include "Visions/AlternateRealitySimulator.h"
#include "ForceVisionWidget.generated.h"

/**
 * Vision display types
 */
UENUM(BlueprintType)
enum class EVisionDisplayType : uint8
{
    Flashback           UMETA(DisplayName = "Flashback"),
    Prophecy            UMETA(DisplayName = "Prophecy"),
    Hallucination       UMETA(DisplayName = "Hallucination"),
    Memory              UMETA(DisplayName = "Memory"),
    WhatIf              UMETA(DisplayName = "What If"),
    Nightmare           UMETA(DisplayName = "Nightmare"),
    Inspiration         UMETA(DisplayName = "Inspiration"),
    Warning             UMETA(DisplayName = "Warning")
};

/**
 * Vision transition effects
 */
UENUM(BlueprintType)
enum class EVisionTransition : uint8
{
    FadeToBlack         UMETA(DisplayName = "Fade to Black"),
    FadeToWhite         UMETA(DisplayName = "Fade to White"),
    Dissolve            UMETA(DisplayName = "Dissolve"),
    Ripple              UMETA(DisplayName = "Ripple"),
    Spiral              UMETA(DisplayName = "Spiral"),
    ForceWave           UMETA(DisplayName = "Force Wave"),
    LightSaber          UMETA(DisplayName = "Lightsaber"),
    Meditation          UMETA(DisplayName = "Meditation")
};

/**
 * Vision data structure
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FForceVisionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString VisionID;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString VisionTitle;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString VisionText;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    EVisionDisplayType DisplayType;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    EVisionTransition TransitionIn;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    EVisionTransition TransitionOut;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    float Duration; // Duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    UTexture2D* VisionImage;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FLinearColor VisionTint;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TArray<FString> VisionChoices; // Player choices during vision

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TMap<FString, FString> VisionMetadata; // Additional data

    FForceVisionData()
    {
        VisionID = TEXT("");
        VisionTitle = TEXT("Force Vision");
        VisionText = TEXT("You see visions of the past and future...");
        DisplayType = EVisionDisplayType::Flashback;
        TransitionIn = EVisionTransition::FadeToBlack;
        TransitionOut = EVisionTransition::FadeToBlack;
        Duration = 5.0f;
        VisionImage = nullptr;
        VisionTint = FLinearColor::White;
    }
};

/**
 * Vision events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisionStarted, const FForceVisionData&, VisionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisionCompleted, const FForceVisionData&, VisionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVisionChoiceMade, const FString&, ChoiceID, const FString&, ChoiceText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVisionSkipped);

/**
 * Force Vision Widget - Displays Force visions and flashbacks
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UForceVisionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UForceVisionWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    /**
     * Show Force vision
     * @param VisionData Data for the vision to display
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void ShowVision(const FForceVisionData& VisionData);

    /**
     * Hide Force vision
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void HideVision();

    /**
     * Skip current vision
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void SkipVision();

    /**
     * Set vision skippable
     * @param bSkippable Whether vision can be skipped
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void SetVisionSkippable(bool bSkippable);

    /**
     * Update vision progress
     * @param Progress Progress from 0.0 to 1.0
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void UpdateVisionProgress(float Progress);

    /**
     * Add vision choice
     * @param ChoiceID ID of the choice
     * @param ChoiceText Text for the choice
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void AddVisionChoice(const FString& ChoiceID, const FString& ChoiceText);

    /**
     * Clear vision choices
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void ClearVisionChoices();

    /**
     * Check if vision is active
     * @return True if vision is currently being displayed
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Force Vision")
    bool IsVisionActive() const { return bVisionActive; }

    /**
     * Get current vision data
     * @return Current vision data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Force Vision")
    FForceVisionData GetCurrentVisionData() const { return CurrentVisionData; }

    /**
     * Set vision auto-advance
     * @param bAutoAdvance Whether to auto-advance vision
     * @param AdvanceDelay Delay before auto-advance
     */
    UFUNCTION(BlueprintCallable, Category = "Force Vision")
    void SetVisionAutoAdvance(bool bAutoAdvance, float AdvanceDelay = 3.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnVisionStarted OnVisionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnVisionCompleted OnVisionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnVisionChoiceMade OnVisionChoiceMade;

    UPROPERTY(BlueprintAssignable, Category = "Vision Events")
    FOnVisionSkipped OnVisionSkipped;

protected:
    // UI Components
    UPROPERTY(meta = (BindWidget))
    UImage* VisionBackgroundImage;

    UPROPERTY(meta = (BindWidget))
    UImage* VisionMainImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* VisionTitleText;

    UPROPERTY(meta = (BindWidget))
    URichTextBlock* VisionContentText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* VisionProgressBar;

    UPROPERTY(meta = (BindWidget))
    UButton* SkipButton;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* ChoicesContainer;

    // Vision state
    UPROPERTY(BlueprintReadOnly, Category = "Vision State")
    FForceVisionData CurrentVisionData;

    UPROPERTY(BlueprintReadOnly, Category = "Vision State")
    bool bVisionActive;

    UPROPERTY(BlueprintReadOnly, Category = "Vision State")
    bool bVisionSkippable;

    UPROPERTY(BlueprintReadOnly, Category = "Vision State")
    float VisionStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Vision State")
    float VisionDuration;

    // Auto-advance settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Advance")
    bool bAutoAdvanceEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Advance")
    float AutoAdvanceDelay;

    // Animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeInAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeOutAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* DissolveAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* RippleAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* SpiralAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ForceWaveAnimation;

    // Choice buttons
    UPROPERTY(BlueprintReadOnly, Category = "Choice Buttons")
    TArray<UButton*> ChoiceButtons;

    // Timer handles
    FTimerHandle VisionTimer;
    FTimerHandle AutoAdvanceTimer;

    // Widget classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UButton> ChoiceButtonClass;

private:
    // Helper methods
    void StartVisionTimer();
    void PlayTransitionAnimation(EVisionTransition Transition, bool bIsTransitionIn);
    UWidgetAnimation* GetTransitionAnimation(EVisionTransition Transition);
    void UpdateVisionDisplay();
    void CreateChoiceButtons();
    void ClearChoiceButtons();

    // Timer callbacks
    UFUNCTION()
    void OnVisionTimerComplete();

    UFUNCTION()
    void OnAutoAdvanceTimer();

    // Button events
    UFUNCTION()
    void OnSkipButtonClicked();

    UFUNCTION()
    void OnChoiceButtonClicked();

    // Animation events
    UFUNCTION()
    void OnTransitionInComplete();

    UFUNCTION()
    void OnTransitionOutComplete();

public:
    /**
     * Blueprint implementable events for custom vision logic
     */

    /**
     * Called when vision starts (for custom setup)
     * @param VisionData The vision data being displayed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void OnVisionStartedEvent(const FForceVisionData& VisionData);

    /**
     * Called when vision completes (for custom cleanup)
     * @param VisionData The vision data that completed
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void OnVisionCompletedEvent(const FForceVisionData& VisionData);

    /**
     * Called when vision choice is made (for custom processing)
     * @param ChoiceID ID of the selected choice
     * @param ChoiceText Text of the selected choice
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void OnVisionChoiceMadeEvent(const FString& ChoiceID, const FString& ChoiceText);

    /**
     * Called to customize vision appearance (override in Blueprint)
     * @param VisionData The vision data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void CustomizeVisionAppearance(const FForceVisionData& VisionData);

    /**
     * Called to generate custom vision content (override in Blueprint)
     * @param VisionType Type of vision to generate
     * @param Context Current game context
     * @return Custom vision data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    FForceVisionData GenerateCustomVision(EVisionDisplayType VisionType, const FString& Context);

    /**
     * Called when vision is skipped (for custom handling)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Vision Events")
    void OnVisionSkippedEvent();
};
