// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "DialogueBoxWidget.generated.h"

/**
 * Dialogue choice data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDialogueChoice
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Choice")
    FString ChoiceText;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Choice")
    FString ChoiceID;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Choice")
    bool bIsEnabled;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Choice")
    FLinearColor TextColor;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Choice")
    FString TooltipText;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Choice")
    TMap<FString, FString> ChoiceMetadata; // Additional data

    FDialogueChoice()
    {
        ChoiceText = TEXT("Continue");
        ChoiceID = TEXT("continue");
        bIsEnabled = true;
        TextColor = FLinearColor::White;
        TooltipText = TEXT("");
    }
};

/**
 * Dialogue speaker data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDialogueSpeaker
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Speaker")
    FString SpeakerName;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Speaker")
    FString SpeakerID;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Speaker")
    UTexture2D* SpeakerPortrait;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Speaker")
    FLinearColor NameColor;

    UPROPERTY(BlueprintReadWrite, Category = "Dialogue Speaker")
    FString SpeakerTitle; // Optional title/subtitle

    FDialogueSpeaker()
    {
        SpeakerName = TEXT("Unknown");
        SpeakerID = TEXT("");
        SpeakerPortrait = nullptr;
        NameColor = FLinearColor::White;
        SpeakerTitle = TEXT("");
    }
};

/**
 * Dialogue events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueChoiceMade, const FString&, SelectedChoiceID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTextDisplayed, const FString&, DialogueText);

/**
 * Dialogue Choice Button Widget - Individual choice button
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UDialogueChoiceButton : public UUserWidget
{
    GENERATED_BODY()

public:
    UDialogueChoiceButton(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;

public:
    /**
     * Set choice data
     * @param ChoiceData Data for this choice
     * @param ChoiceIndex Index of this choice
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Choice")
    void SetChoiceData(const FDialogueChoice& ChoiceData, int32 ChoiceIndex);

    /**
     * Get choice data
     * @return Current choice data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue Choice")
    FDialogueChoice GetChoiceData() const { return CurrentChoiceData; }

    /**
     * Set choice enabled state
     * @param bEnabled Whether choice should be enabled
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Choice")
    void SetChoiceEnabled(bool bEnabled);

    /**
     * Get choice index
     * @return Index of this choice
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue Choice")
    int32 GetChoiceIndex() const { return ChoiceIndex; }

    // Event delegate
    UPROPERTY(BlueprintAssignable, Category = "Choice Events")
    FOnDialogueChoiceMade OnChoiceSelected;

protected:
    // UI Components
    UPROPERTY(meta = (BindWidget))
    UButton* ChoiceButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ChoiceTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ChoiceNumberText;

    // Choice data
    UPROPERTY(BlueprintReadOnly, Category = "Choice Data")
    FDialogueChoice CurrentChoiceData;

    UPROPERTY(BlueprintReadOnly, Category = "Choice Data")
    int32 ChoiceIndex;

    // Animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HoverAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ClickAnimation;

private:
    // Button events
    UFUNCTION()
    void OnChoiceButtonClicked();

    UFUNCTION()
    void OnChoiceButtonHovered();

    UFUNCTION()
    void OnChoiceButtonUnhovered();

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when choice is selected (for custom effects)
     * @param ChoiceData The selected choice data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Choice Events")
    void OnChoiceSelectedEvent(const FDialogueChoice& ChoiceData);

    /**
     * Called to customize choice appearance (override in Blueprint)
     * @param ChoiceData The choice data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Choice Events")
    void CustomizeChoiceAppearance(const FDialogueChoice& ChoiceData);
};

/**
 * Dialogue Box Widget - Universal dialogue system UI
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UDialogueBoxWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UDialogueBoxWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    /**
     * Show dialogue
     * @param SpeakerData Speaker information
     * @param DialogueText Main dialogue text
     * @param Choices Available dialogue choices
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void ShowDialogue(const FDialogueSpeaker& SpeakerData, const FString& DialogueText, 
                     const TArray<FDialogueChoice>& Choices);

    /**
     * Hide dialogue
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void HideDialogue();

    /**
     * Update dialogue text
     * @param NewDialogueText New dialogue text to display
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void UpdateDialogueText(const FString& NewDialogueText);

    /**
     * Update speaker
     * @param NewSpeakerData New speaker data
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void UpdateSpeaker(const FDialogueSpeaker& NewSpeakerData);

    /**
     * Add dialogue choice
     * @param ChoiceData Choice to add
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void AddDialogueChoice(const FDialogueChoice& ChoiceData);

    /**
     * Clear all choices
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void ClearDialogueChoices();

    /**
     * Set dialogue visible
     * @param bVisible Whether dialogue should be visible
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void SetDialogueVisible(bool bVisible);

    /**
     * Check if dialogue is visible
     * @return True if dialogue is currently visible
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue Box")
    bool IsDialogueVisible() const { return bIsVisible; }

    /**
     * Enable/disable input
     * @param bEnabled Whether input should be enabled
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void SetInputEnabled(bool bEnabled);

    /**
     * Set typewriter effect enabled
     * @param bEnabled Whether to use typewriter effect
     * @param TypewriterSpeed Speed of typewriter effect
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue Box")
    void SetTypewriterEffect(bool bEnabled, float TypewriterSpeed = 50.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueChoiceMade OnDialogueChoiceMade;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueStarted OnDialogueStarted;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueEnded OnDialogueEnded;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueTextDisplayed OnDialogueTextDisplayed;

protected:
    // UI Components
    UPROPERTY(meta = (BindWidget))
    UImage* SpeakerPortraitImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SpeakerNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SpeakerTitleText;

    UPROPERTY(meta = (BindWidget))
    URichTextBlock* DialogueRichText;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ChoicesVerticalBox;

    UPROPERTY(meta = (BindWidget))
    UButton* ContinueButton;

    // Dialogue state
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue State")
    bool bIsVisible;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue State")
    bool bInputEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue State")
    FDialogueSpeaker CurrentSpeaker;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue State")
    FString CurrentDialogueText;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue State")
    TArray<FDialogueChoice> CurrentChoices;

    // Choice buttons
    UPROPERTY(BlueprintReadOnly, Category = "Choice Buttons")
    TArray<UDialogueChoiceButton*> ChoiceButtons;

    // Typewriter effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typewriter Effect")
    bool bUseTypewriterEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typewriter Effect")
    float TypewriterSpeed;

    UPROPERTY()
    FTimerHandle TypewriterTimer;

    UPROPERTY()
    int32 TypewriterCharIndex;

    // Animations
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeInAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeOutAnimation;

    // Choice button class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UDialogueChoiceButton> ChoiceButtonClass;

private:
    // Helper methods
    void CreateChoiceButtons();
    void UpdateChoiceButtons();
    void StartTypewriterEffect();
    void ProcessTypewriterEffect();
    void CompleteTypewriterEffect();

    // Event handlers
    UFUNCTION()
    void OnChoiceButtonSelected(const FString& SelectedChoiceID);

    UFUNCTION()
    void OnContinueButtonClicked();

    // Input handling
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
    /**
     * Blueprint implementable events for custom dialogue logic
     */

    /**
     * Called when dialogue is shown (for custom setup)
     * @param SpeakerData The speaker data
     * @param DialogueText The dialogue text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void OnDialogueShown(const FDialogueSpeaker& SpeakerData, const FString& DialogueText);

    /**
     * Called when dialogue is hidden (for custom cleanup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void OnDialogueHidden();

    /**
     * Called to customize dialogue appearance (override in Blueprint)
     * @param SpeakerData The speaker data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void CustomizeDialogueAppearance(const FDialogueSpeaker& SpeakerData);

    /**
     * Called when choice is made (for custom processing)
     * @param ChoiceData The selected choice data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue Events")
    void OnDialogueChoiceMadeEvent(const FDialogueChoice& ChoiceData);
};
