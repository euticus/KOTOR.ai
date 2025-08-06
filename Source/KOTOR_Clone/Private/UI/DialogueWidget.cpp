// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/DialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Engine/Engine.h"

UDialogueWidget::UDialogueWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsDialogueActive = false;
    CurrentQuestManager = nullptr;
}

void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Bind end dialogue button if it exists
    if (EndDialogueButton)
    {
        EndDialogueButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnEndDialogueClicked);
    }
    
    // Hide widget initially
    SetVisibility(ESlateVisibility::Hidden);
}

void UDialogueWidget::NativeDestruct()
{
    EndDialogue();
    Super::NativeDestruct();
}

void UDialogueWidget::StartDialogue(const FNPCData& NPCData, UQuestManagerComponent* QuestManager)
{
    if (bIsDialogueActive)
    {
        EndDialogue();
    }
    
    CurrentNPCData = NPCData;
    CurrentQuestManager = QuestManager;
    bIsDialogueActive = true;
    
    // Update UI
    if (NPCNameText)
    {
        NPCNameText->SetText(FText::FromString(NPCData.Name));
    }
    
    if (DialogueText)
    {
        FString GreetingText = GetGreetingText();
        DialogueText->SetText(FText::FromString(GreetingText));
    }
    
    // Generate dialogue options
    GenerateDialogueOptions();
    
    // Show widget
    SetVisibility(ESlateVisibility::Visible);
    
    // Broadcast events
    OnDialogueStarted.Broadcast(NPCData);
    OnDialogueStartedEvent(NPCData);
    
    UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Started dialogue with %s"), *NPCData.Name);
}

void UDialogueWidget::EndDialogue()
{
    if (!bIsDialogueActive)
    {
        return;
    }
    
    bIsDialogueActive = false;
    CurrentQuestManager = nullptr;
    
    // Clear options
    ClearOptionButtons();
    CurrentOptions.Empty();
    
    // Hide widget
    SetVisibility(ESlateVisibility::Hidden);
    
    // Broadcast events
    OnDialogueEnded.Broadcast();
    OnDialogueEndedEvent();
    
    UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Ended dialogue"));
}

void UDialogueWidget::SelectDialogueOption(int32 OptionIndex)
{
    if (!bIsDialogueActive || OptionIndex < 0 || OptionIndex >= CurrentOptions.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget: Invalid option index %d"), OptionIndex);
        return;
    }
    
    const FDialogueOption& SelectedOption = CurrentOptions[OptionIndex];
    
    // Update dialogue text with response
    if (DialogueText && !SelectedOption.ResponseText.IsEmpty())
    {
        DialogueText->SetText(FText::FromString(SelectedOption.ResponseText));
    }
    
    // Handle quest starting
    if (SelectedOption.bStartsQuest && CurrentQuestManager)
    {
        FString QuestID = CurrentQuestManager->StartQuest(
            SelectedOption.QuestToStart,
            CurrentNPCData.Name,
            0, // Planet index - would need to be passed in
            CurrentNPCData.Location
        );
        
        if (!QuestID.IsEmpty())
        {
            OnQuestOffered.Broadcast(SelectedOption.QuestToStart);
            UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Started quest %s"), *QuestID);
        }
    }
    
    // Broadcast events
    OnDialogueOptionSelected.Broadcast(OptionIndex, SelectedOption);
    OnDialogueOptionSelectedEvent(OptionIndex, SelectedOption);
    
    // End dialogue if option specifies
    if (SelectedOption.bEndsDialogue)
    {
        EndDialogue();
    }
    else
    {
        // Regenerate options for continued dialogue
        GenerateDialogueOptions();
    }
}

void UDialogueWidget::GenerateDialogueOptions()
{
    CurrentOptions.Empty();
    ClearOptionButtons();
    
    // Try to get custom options from Blueprint first
    TArray<FDialogueOption> CustomOptions = GenerateCustomDialogueOptions(CurrentNPCData);
    if (CustomOptions.Num() > 0)
    {
        CurrentOptions = CustomOptions;
    }
    else
    {
        // Generate default options based on NPC data
        
        // Standard greeting option
        FDialogueOption GreetingOption;
        GreetingOption.OptionText = TEXT("Hello there.");
        GreetingOption.ResponseText = FString::Printf(TEXT("Greetings, traveler. I am %s."), *CurrentNPCData.Name);
        CurrentOptions.Add(GreetingOption);
        
        // Quest option if NPC has a quest
        if (!CurrentNPCData.Quest.Title.IsEmpty())
        {
            FDialogueOption QuestOption;
            QuestOption.OptionText = TEXT("Do you have any work for me?");
            QuestOption.ResponseText = FString::Printf(TEXT("As a matter of fact, I do. %s"), *CurrentNPCData.Quest.Description);
            QuestOption.bStartsQuest = true;
            QuestOption.QuestToStart = CurrentNPCData.Quest;
            CurrentOptions.Add(QuestOption);
        }
        
        // Information option
        FDialogueOption InfoOption;
        InfoOption.OptionText = TEXT("Tell me about yourself.");
        InfoOption.ResponseText = CurrentNPCData.Backstory.IsEmpty() ? 
            TEXT("There's not much to say about me.") : CurrentNPCData.Backstory;
        CurrentOptions.Add(InfoOption);
        
        // Alignment-based options
        if (CurrentNPCData.Alignment == TEXT("light"))
        {
            FDialogueOption LightOption;
            LightOption.OptionText = TEXT("How can I help the people here?");
            LightOption.ResponseText = TEXT("Your desire to help others is admirable. The galaxy needs more people like you.");
            LightOption.AlignmentRequirement = TEXT("light");
            CurrentOptions.Add(LightOption);
        }
        else if (CurrentNPCData.Alignment == TEXT("dark"))
        {
            FDialogueOption DarkOption;
            DarkOption.OptionText = TEXT("I'm looking for power. Can you help?");
            DarkOption.ResponseText = TEXT("Power... yes, I can sense your ambition. Perhaps we can help each other.");
            DarkOption.AlignmentRequirement = TEXT("dark");
            CurrentOptions.Add(DarkOption);
        }
        
        // Goodbye option
        FDialogueOption GoodbyeOption;
        GoodbyeOption.OptionText = TEXT("I should be going.");
        GoodbyeOption.ResponseText = TEXT("Farewell, traveler. May the Force be with you.");
        GoodbyeOption.bEndsDialogue = true;
        CurrentOptions.Add(GoodbyeOption);
    }
    
    // Create UI buttons for options
    for (int32 i = 0; i < CurrentOptions.Num(); i++)
    {
        if (IsOptionAvailable(CurrentOptions[i]))
        {
            CreateOptionButton(CurrentOptions[i], i);
        }
    }
}

void UDialogueWidget::CreateOptionButton(const FDialogueOption& Option, int32 OptionIndex)
{
    if (!OptionsContainer)
    {
        return;
    }
    
    // Create button (this would typically be done in Blueprint)
    UButton* OptionButton = NewObject<UButton>(this);
    if (OptionButton)
    {
        // Set button text (this is simplified - normally you'd create a proper button widget)
        OptionButton->SetToolTipText(FText::FromString(Option.OptionText));
        
        // Bind click event
        OptionButton->OnClicked.AddDynamic(this, &UDialogueWidget::SelectDialogueOption);
        
        // Add to container
        OptionsContainer->AddChild(OptionButton);
        
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Created option button: %s"), *Option.OptionText);
    }
}

void UDialogueWidget::ClearOptionButtons()
{
    if (OptionsContainer)
    {
        OptionsContainer->ClearChildren();
    }
}

FString UDialogueWidget::GetGreetingText() const
{
    // Try to get custom greeting from Blueprint first
    FString CustomGreeting = GetCustomGreetingText(CurrentNPCData);
    if (!CustomGreeting.IsEmpty())
    {
        return CustomGreeting;
    }
    
    // Generate default greeting based on NPC data
    FString Greeting;
    
    if (CurrentNPCData.DialogueStyle == TEXT("Formal"))
    {
        Greeting = FString::Printf(TEXT("Good day. I am %s, %s."), 
                                 *CurrentNPCData.Name, *CurrentNPCData.Role);
    }
    else if (CurrentNPCData.DialogueStyle == TEXT("Casual"))
    {
        Greeting = FString::Printf(TEXT("Hey there! Name's %s. What's up?"), *CurrentNPCData.Name);
    }
    else if (CurrentNPCData.DialogueStyle == TEXT("Aggressive"))
    {
        Greeting = FString::Printf(TEXT("What do you want? I'm %s, and I don't have time for games."), 
                                 *CurrentNPCData.Name);
    }
    else if (CurrentNPCData.DialogueStyle == TEXT("Mysterious"))
    {
        Greeting = FString::Printf(TEXT("Ah, another traveler... I am known as %s."), *CurrentNPCData.Name);
    }
    else
    {
        // Default friendly greeting
        Greeting = FString::Printf(TEXT("Hello, traveler. I'm %s. How can I help you?"), *CurrentNPCData.Name);
    }
    
    return Greeting;
}

bool UDialogueWidget::IsOptionAvailable(const FDialogueOption& Option) const
{
    // Check alignment requirement
    if (!Option.AlignmentRequirement.IsEmpty())
    {
        // This would normally check player's alignment
        // For now, just allow all options
    }
    
    // Check reputation requirement
    if (Option.ReputationRequirement > 0)
    {
        // This would normally check player's reputation with this NPC
        // For now, just allow all options
    }
    
    return true;
}

void UDialogueWidget::OnEndDialogueClicked()
{
    EndDialogue();
}

// Quest Log Widget Implementation

UQuestLogWidget::UQuestLogWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UQuestLogWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Bind toggle button if it exists
    if (ToggleButton)
    {
        ToggleButton->OnClicked.AddDynamic(this, &UQuestLogWidget::OnToggleClicked);
    }
    
    // Hide widget initially
    SetVisibility(ESlateVisibility::Hidden);
}

void UQuestLogWidget::RefreshQuestLog(UQuestManagerComponent* QuestManager)
{
    if (!QuestManager)
    {
        return;
    }
    
    ClearQuestEntries();
    
    TArray<FActiveQuest> ActiveQuests = QuestManager->GetActiveQuests();
    
    // Update quest count
    if (QuestCountText)
    {
        FString CountText = FString::Printf(TEXT("Active Quests: %d"), ActiveQuests.Num());
        QuestCountText->SetText(FText::FromString(CountText));
    }
    
    // Create quest entries
    for (const FActiveQuest& Quest : ActiveQuests)
    {
        CreateQuestEntry(Quest);
    }
    
    // Broadcast event
    OnQuestLogRefreshed(ActiveQuests.Num());
    
    UE_LOG(LogTemp, Log, TEXT("QuestLogWidget: Refreshed with %d active quests"), ActiveQuests.Num());
}

void UQuestLogWidget::ToggleQuestLog()
{
    ESlateVisibility CurrentVisibility = GetVisibility();
    ESlateVisibility NewVisibility = (CurrentVisibility == ESlateVisibility::Visible) ? 
        ESlateVisibility::Hidden : ESlateVisibility::Visible;
    
    SetVisibility(NewVisibility);
}

void UQuestLogWidget::CreateQuestEntry(const FActiveQuest& Quest)
{
    if (!QuestListContainer)
    {
        return;
    }
    
    // Try to create custom quest entry from Blueprint first
    UUserWidget* CustomEntry = CreateCustomQuestEntry(Quest);
    if (CustomEntry)
    {
        QuestListContainer->AddChild(CustomEntry);
        return;
    }
    
    // Create default quest entry (simplified)
    UTextBlock* QuestEntry = NewObject<UTextBlock>(this);
    if (QuestEntry)
    {
        FString QuestText = FString::Printf(TEXT("%s - %s"), 
                                          *Quest.QuestData.Title, *Quest.QuestData.Description);
        QuestEntry->SetText(FText::FromString(QuestText));
        QuestListContainer->AddChild(QuestEntry);
    }
}

void UQuestLogWidget::ClearQuestEntries()
{
    if (QuestListContainer)
    {
        QuestListContainer->ClearChildren();
    }
}

void UQuestLogWidget::OnToggleClicked()
{
    ToggleQuestLog();
}
