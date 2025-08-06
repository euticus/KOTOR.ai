// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/AIDMDebugWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"
#include "Engine/Engine.h"

UAIDMDebugWidget::UAIDMDebugWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bAutoRefresh = true;
    RefreshInterval = 1.0f;
    DefaultCampaignPath = TEXT("Campaigns/test_campaign.json");
    LastRefreshTime = 0.0f;
    bIsInitialized = false;
    
    CampaignLoaderRef = nullptr;
    AIDirectorRef = nullptr;
    QuestManagerRef = nullptr;
}

void UAIDMDebugWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Bind button events
    if (LoadCampaignButton)
    {
        LoadCampaignButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnLoadCampaignClicked);
    }
    
    if (RefreshButton)
    {
        RefreshButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnRefreshClicked);
    }
    
    if (RespawnContentButton)
    {
        RespawnContentButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnRespawnContentClicked);
    }
    
    if (ClearContentButton)
    {
        ClearContentButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnClearContentClicked);
    }
    
    if (StartTestQuestButton)
    {
        StartTestQuestButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnStartTestQuestClicked);
    }
    
    if (CompleteQuestsButton)
    {
        CompleteQuestsButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnCompleteQuestsClicked);
    }
    
    if (ToggleVisibilityButton)
    {
        ToggleVisibilityButton->OnClicked.AddDynamic(this, &UAIDMDebugWidget::OnToggleVisibilityClicked);
    }
    
    // Bind selector events
    if (PlanetSelector)
    {
        PlanetSelector->OnSelectionChanged.AddDynamic(this, &UAIDMDebugWidget::OnPlanetSelected);
    }
    
    if (LayoutSelector)
    {
        LayoutSelector->OnSelectionChanged.AddDynamic(this, &UAIDMDebugWidget::OnLayoutSelected);
    }
    
    // Bind checkbox events
    if (DebugModeCheckbox)
    {
        DebugModeCheckbox->OnCheckStateChanged.AddDynamic(this, &UAIDMDebugWidget::OnDebugModeChanged);
    }
    
    if (ShowSpawnPointsCheckbox)
    {
        ShowSpawnPointsCheckbox->OnCheckStateChanged.AddDynamic(this, &UAIDMDebugWidget::OnShowSpawnPointsChanged);
    }
    
    // Hide widget initially
    SetVisibility(ESlateVisibility::Hidden);
}

void UAIDMDebugWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (bAutoRefresh && bIsInitialized)
    {
        LastRefreshTime += InDeltaTime;
        if (LastRefreshTime >= RefreshInterval)
        {
            RefreshDebugInfo();
            LastRefreshTime = 0.0f;
        }
    }
}

void UAIDMDebugWidget::InitializeDebugWidget(UCampaignLoaderSubsystem* CampaignLoader, 
                                           UAIDirectorComponent* AIDirector, 
                                           UQuestManagerComponent* QuestManager)
{
    CampaignLoaderRef = CampaignLoader;
    AIDirectorRef = AIDirector;
    QuestManagerRef = QuestManager;
    
    bIsInitialized = true;
    
    // Initial refresh
    RefreshDebugInfo();
    
    // Broadcast initialization event
    OnDebugWidgetInitialized();
    
    UE_LOG(LogTemp, Log, TEXT("AIDMDebugWidget: Initialized with components"));
}

void UAIDMDebugWidget::RefreshDebugInfo()
{
    if (!bIsInitialized)
    {
        return;
    }
    
    UpdateCampaignInfo();
    UpdateDirectorInfo();
    UpdateQuestInfo();
    UpdatePlanetSelector();
    UpdateLayoutSelector();
    UpdateActiveQuestsList();
}

void UAIDMDebugWidget::ToggleDebugWidget()
{
    ESlateVisibility CurrentVisibility = GetVisibility();
    ESlateVisibility NewVisibility = (CurrentVisibility == ESlateVisibility::Visible) ? 
        ESlateVisibility::Hidden : ESlateVisibility::Visible;
    
    SetVisibility(NewVisibility);
}

void UAIDMDebugWidget::LoadCampaignFile(const FString& CampaignFilePath)
{
    if (CampaignLoaderRef)
    {
        bool bSuccess = CampaignLoaderRef->LoadCampaign(CampaignFilePath);
        
        if (bSuccess && AIDirectorRef)
        {
            AIDirectorRef->InitializeWithCampaign(CampaignFilePath);
            
            // Broadcast event
            const FCampaignPlan& Campaign = CampaignLoaderRef->GetCurrentCampaign();
            OnCampaignLoadedForDebug(Campaign);
        }
        
        RefreshDebugInfo();
        
        UE_LOG(LogTemp, Log, TEXT("AIDMDebugWidget: Load campaign %s: %s"), 
               *CampaignFilePath, bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
    }
}

void UAIDMDebugWidget::ChangeToPlanet(int32 PlanetIndex)
{
    if (AIDirectorRef)
    {
        bool bSuccess = AIDirectorRef->ChangeToPlanet(PlanetIndex);
        
        if (bSuccess && CampaignLoaderRef)
        {
            FPlanetData PlanetData = CampaignLoaderRef->GetPlanetData(PlanetIndex);
            OnPlanetChangedForDebug(PlanetIndex, PlanetData);
        }
        
        RefreshDebugInfo();
    }
}

void UAIDMDebugWidget::ChangeToLayout(const FString& LayoutName)
{
    if (AIDirectorRef)
    {
        AIDirectorRef->ChangeToLayout(LayoutName);
        RefreshDebugInfo();
    }
}

void UAIDMDebugWidget::ForceRespawnContent()
{
    if (AIDirectorRef)
    {
        AIDirectorRef->SpawnContentForCurrentLayout(true);
        RefreshDebugInfo();
    }
}

void UAIDMDebugWidget::ClearAllContent()
{
    if (AIDirectorRef)
    {
        AIDirectorRef->ClearAllSpawnedContent();
        RefreshDebugInfo();
    }
}

void UAIDMDebugWidget::StartTestQuest()
{
    if (QuestManagerRef)
    {
        // Create a test quest
        FQuestData TestQuest;
        TestQuest.Title = TEXT("Debug Test Quest");
        TestQuest.Description = TEXT("A test quest created from the debug panel");
        TestQuest.QuestType = TEXT("test");
        TestQuest.Difficulty = TEXT("easy");
        TestQuest.EstimatedTimeMinutes = 5;
        
        FString QuestID = QuestManagerRef->StartQuest(TestQuest, TEXT("Debug Panel"), 0, TEXT("Debug"));
        
        UE_LOG(LogTemp, Log, TEXT("AIDMDebugWidget: Started test quest: %s"), *QuestID);
        RefreshDebugInfo();
    }
}

void UAIDMDebugWidget::CompleteAllQuests()
{
    if (QuestManagerRef)
    {
        TArray<FActiveQuest> ActiveQuests = QuestManagerRef->GetActiveQuests();
        
        for (const FActiveQuest& Quest : ActiveQuests)
        {
            QuestManagerRef->CompleteQuest(Quest.QuestID);
        }
        
        UE_LOG(LogTemp, Log, TEXT("AIDMDebugWidget: Completed %d quests"), ActiveQuests.Num());
        RefreshDebugInfo();
    }
}

void UAIDMDebugWidget::UpdateCampaignInfo()
{
    if (!CampaignLoaderRef)
    {
        return;
    }
    
    if (CampaignLoaderRef->IsCampaignLoaded())
    {
        const FCampaignPlan& Campaign = CampaignLoaderRef->GetCurrentCampaign();
        
        if (CampaignNameText)
        {
            CampaignNameText->SetText(FText::FromString(Campaign.Config.StorySeed));
        }
        
        if (CampaignStatsText)
        {
            FString StatsText = FString::Printf(
                TEXT("Length: %d hours | Planets: %d | Era: %s | Focus: %s"),
                Campaign.Config.GameLengthHours,
                Campaign.Planets.Num(),
                *Campaign.Config.TimePeriod,
                *Campaign.Config.AlignmentFocus
            );
            CampaignStatsText->SetText(FText::FromString(StatsText));
        }
    }
    else
    {
        if (CampaignNameText)
        {
            CampaignNameText->SetText(FText::FromString(TEXT("No Campaign Loaded")));
        }
        
        if (CampaignStatsText)
        {
            CampaignStatsText->SetText(FText::FromString(TEXT("Load a campaign to see stats")));
        }
    }
}

void UAIDMDebugWidget::UpdateDirectorInfo()
{
    if (!AIDirectorRef)
    {
        return;
    }
    
    if (DirectorStatusText)
    {
        FString StatusText;
        if (AIDirectorRef->IsInitialized())
        {
            FPlanetData CurrentPlanet = AIDirectorRef->GetCurrentPlanetData();
            FMapLayout CurrentLayout = AIDirectorRef->GetCurrentLayoutData();
            
            StatusText = FString::Printf(
                TEXT("ACTIVE | Planet: %s | Layout: %s"),
                *CurrentPlanet.Name,
                *CurrentLayout.Name
            );
        }
        else
        {
            StatusText = TEXT("NOT INITIALIZED");
        }
        
        DirectorStatusText->SetText(FText::FromString(StatusText));
    }
    
    if (SpawnPointsText)
    {
        TArray<FSpawnPointData> NPCSpawns = AIDirectorRef->GetSpawnPointsByType(TEXT("NPC"));
        TArray<FSpawnPointData> EnemySpawns = AIDirectorRef->GetSpawnPointsByType(TEXT("Enemy"));
        TArray<FSpawnPointData> LootSpawns = AIDirectorRef->GetSpawnPointsByType(TEXT("Loot"));
        
        FString SpawnText = FString::Printf(
            TEXT("Spawn Points - NPC: %d | Enemy: %d | Loot: %d"),
            NPCSpawns.Num(),
            EnemySpawns.Num(),
            LootSpawns.Num()
        );
        
        SpawnPointsText->SetText(FText::FromString(SpawnText));
    }
}

void UAIDMDebugWidget::UpdateQuestInfo()
{
    if (!QuestManagerRef)
    {
        return;
    }
    
    if (QuestStatsText)
    {
        TArray<FActiveQuest> ActiveQuests = QuestManagerRef->GetActiveQuests();
        TArray<FActiveQuest> CompletedQuests = QuestManagerRef->GetCompletedQuests();
        
        FString QuestText = FString::Printf(
            TEXT("Active: %d | Completed: %d"),
            ActiveQuests.Num(),
            CompletedQuests.Num()
        );
        
        QuestStatsText->SetText(FText::FromString(QuestText));
    }
}

void UAIDMDebugWidget::UpdatePlanetSelector()
{
    if (!PlanetSelector || !CampaignLoaderRef || !CampaignLoaderRef->IsCampaignLoaded())
    {
        return;
    }
    
    PlanetSelector->ClearOptions();
    
    const FCampaignPlan& Campaign = CampaignLoaderRef->GetCurrentCampaign();
    for (int32 i = 0; i < Campaign.Planets.Num(); i++)
    {
        FString OptionText = FString::Printf(TEXT("%d: %s"), i, *Campaign.Planets[i].Name);
        PlanetSelector->AddOption(OptionText);
    }
}

void UAIDMDebugWidget::UpdateLayoutSelector()
{
    if (!LayoutSelector || !AIDirectorRef || !AIDirectorRef->IsInitialized())
    {
        return;
    }
    
    LayoutSelector->ClearOptions();
    
    FPlanetData CurrentPlanet = AIDirectorRef->GetCurrentPlanetData();
    for (const FMapLayout& Layout : CurrentPlanet.Layouts)
    {
        LayoutSelector->AddOption(Layout.Name);
    }
}

void UAIDMDebugWidget::UpdateActiveQuestsList()
{
    if (!ActiveQuestsContainer || !QuestManagerRef)
    {
        return;
    }
    
    ActiveQuestsContainer->ClearChildren();
    
    TArray<FActiveQuest> ActiveQuests = QuestManagerRef->GetActiveQuests();
    for (const FActiveQuest& Quest : ActiveQuests)
    {
        UTextBlock* QuestEntry = NewObject<UTextBlock>(this);
        if (QuestEntry)
        {
            FString QuestText = FString::Printf(TEXT("%s - %s"), 
                                              *Quest.QuestID, *Quest.QuestData.Title);
            QuestEntry->SetText(FText::FromString(QuestText));
            ActiveQuestsContainer->AddChild(QuestEntry);
        }
    }
}

// Button event handlers
void UAIDMDebugWidget::OnLoadCampaignClicked()
{
    LoadCampaignFile(DefaultCampaignPath);
}

void UAIDMDebugWidget::OnRefreshClicked()
{
    RefreshDebugInfo();
}

void UAIDMDebugWidget::OnRespawnContentClicked()
{
    ForceRespawnContent();
}

void UAIDMDebugWidget::OnClearContentClicked()
{
    ClearAllContent();
}

void UAIDMDebugWidget::OnStartTestQuestClicked()
{
    StartTestQuest();
}

void UAIDMDebugWidget::OnCompleteQuestsClicked()
{
    CompleteAllQuests();
}

void UAIDMDebugWidget::OnToggleVisibilityClicked()
{
    ToggleDebugWidget();
}

void UAIDMDebugWidget::OnPlanetSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (SelectionType != ESelectInfo::Direct)
    {
        // Extract planet index from the selection
        FString IndexString;
        if (SelectedItem.Split(TEXT(":"), &IndexString, nullptr))
        {
            int32 PlanetIndex = FCString::Atoi(*IndexString);
            ChangeToPlanet(PlanetIndex);
        }
    }
}

void UAIDMDebugWidget::OnLayoutSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (SelectionType != ESelectInfo::Direct)
    {
        ChangeToLayout(SelectedItem);
    }
}

void UAIDMDebugWidget::OnDebugModeChanged(bool bIsChecked)
{
    // This would set debug mode on AI Director
    UE_LOG(LogTemp, Log, TEXT("AIDMDebugWidget: Debug mode %s"), bIsChecked ? TEXT("ON") : TEXT("OFF"));
}

void UAIDMDebugWidget::OnShowSpawnPointsChanged(bool bIsChecked)
{
    // This would toggle spawn point visualization
    UE_LOG(LogTemp, Log, TEXT("AIDMDebugWidget: Show spawn points %s"), bIsChecked ? TEXT("ON") : TEXT("OFF"));
}
