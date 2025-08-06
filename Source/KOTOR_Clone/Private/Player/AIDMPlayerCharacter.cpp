// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/AIDMPlayerCharacter.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/DialogueWidget.h"
#include "Debug/AIDMDebugWidget.h"
#include "DrawDebugHelpers.h"

AAIDMPlayerCharacter::AAIDMPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create AIDM components
    AIDirector = CreateDefaultSubobject<UAIDirectorComponent>(TEXT("AIDirector"));
    QuestManager = CreateDefaultSubobject<UQuestManagerComponent>(TEXT("QuestManager"));

    // Initialize default values
    bDebugModeEnabled = true;
    InteractionRange = 300.0f;
    
    // Set default input keys
    DebugToggleKey = EKeys::F1;
    QuestLogToggleKey = EKeys::J;
    InteractKey = EKeys::E;
    
    // Initialize player stats
    PlayerStats.Level = 1;
    PlayerStats.Experience = 0;
    PlayerStats.LightSidePoints = 0;
    PlayerStats.DarkSidePoints = 0;
    PlayerStats.Credits = 1000;
    
    // Initialize widget pointers
    DialogueWidget = nullptr;
    QuestLogWidget = nullptr;
    DebugWidget = nullptr;
    CampaignLoader = nullptr;
    CurrentInteractable = nullptr;
}

void AAIDMPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Get campaign loader subsystem
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        CampaignLoader = GameInstance->GetSubsystem<UCampaignLoaderSubsystem>();
    }
    
    // Initialize AIDM components
    InitializeAIDMComponents();
    
    // Create UI widgets
    CreateUIWidgets();
    
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Initialized"));
}

void AAIDMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind AIDM input actions
    PlayerInputComponent->BindAction("DebugToggle", IE_Pressed, this, &AAIDMPlayerCharacter::OnDebugToggle);
    PlayerInputComponent->BindAction("QuestLogToggle", IE_Pressed, this, &AAIDMPlayerCharacter::OnQuestLogToggle);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AAIDMPlayerCharacter::OnInteract);
    
    // Bind key events as fallback
    PlayerInputComponent->BindKey(DebugToggleKey, IE_Pressed, this, &AAIDMPlayerCharacter::OnDebugToggle);
    PlayerInputComponent->BindKey(QuestLogToggleKey, IE_Pressed, this, &AAIDMPlayerCharacter::OnQuestLogToggle);
    PlayerInputComponent->BindKey(InteractKey, IE_Pressed, this, &AAIDMPlayerCharacter::OnInteract);
}

void AAIDMPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update interaction detection
    UpdateInteractable();
}

void AAIDMPlayerCharacter::InitializeAIDMComponents()
{
    if (AIDirector && QuestManager)
    {
        // Bind AI Director events
        AIDirector->OnCampaignLoaded.AddDynamic(this, &AAIDMPlayerCharacter::OnCampaignLoaded);
        AIDirector->OnPlanetChanged.AddDynamic(this, &AAIDMPlayerCharacter::OnPlanetChanged);
        AIDirector->OnLayoutChanged.AddDynamic(this, &AAIDMPlayerCharacter::OnLayoutChanged);
        
        // Bind Quest Manager events
        QuestManager->OnQuestStarted.AddDynamic(this, &AAIDMPlayerCharacter::OnQuestStarted);
        QuestManager->OnQuestCompleted.AddDynamic(this, &AAIDMPlayerCharacter::OnQuestCompleted);
        
        UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: AIDM components initialized"));
    }
}

bool AAIDMPlayerCharacter::LoadAndStartCampaign(const FString& CampaignFilePath)
{
    if (!CampaignLoader || !AIDirector)
    {
        UE_LOG(LogTemp, Error, TEXT("AIDMPlayerCharacter: Missing required components for campaign loading"));
        return false;
    }
    
    // Load campaign
    if (!CampaignLoader->LoadCampaign(CampaignFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("AIDMPlayerCharacter: Failed to load campaign: %s"), *CampaignFilePath);
        return false;
    }
    
    // Initialize AI Director with campaign
    if (!AIDirector->InitializeWithCampaign(CampaignFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("AIDMPlayerCharacter: Failed to initialize AI Director with campaign"));
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Successfully loaded and started campaign: %s"), *CampaignFilePath);
    return true;
}

void AAIDMPlayerCharacter::InteractWithNPC(const FNPCData& NPCData)
{
    if (DialogueWidget && QuestManager)
    {
        DialogueWidget->StartDialogue(NPCData, QuestManager);
        OnNPCInteraction(NPCData);
        
        UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Started dialogue with %s"), *NPCData.Name);
    }
}

void AAIDMPlayerCharacter::CompleteQuestObjective(const FString& QuestID, int32 ObjectiveIndex)
{
    if (QuestManager)
    {
        bool bSuccess = QuestManager->UpdateQuestObjective(QuestID, ObjectiveIndex, 1);
        
        if (bSuccess)
        {
            // Award experience for completing objective
            AddExperience(50);
            
            UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Completed objective %d for quest %s"), 
                   ObjectiveIndex, *QuestID);
        }
    }
}

void AAIDMPlayerCharacter::AddExperience(int32 Amount)
{
    int32 OldLevel = PlayerStats.Level;
    PlayerStats.Experience += Amount;
    
    // Simple leveling formula: 1000 XP per level
    int32 NewLevel = (PlayerStats.Experience / 1000) + 1;
    
    if (NewLevel > OldLevel)
    {
        PlayerStats.Level = NewLevel;
        OnPlayerLevelUp(NewLevel);
        
        UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Level up! New level: %d"), NewLevel);
    }
    
    OnExperienceGained(Amount, TEXT("Quest"));
}

void AAIDMPlayerCharacter::AddAlignmentPoints(int32 LightPoints, int32 DarkPoints)
{
    EPlayerAlignment OldAlignment = GetCurrentAlignment();
    
    PlayerStats.LightSidePoints += LightPoints;
    PlayerStats.DarkSidePoints += DarkPoints;
    
    EPlayerAlignment NewAlignment = GetCurrentAlignment();
    
    if (NewAlignment != OldAlignment)
    {
        OnAlignmentChanged(NewAlignment);
        
        UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Alignment changed to %s"), 
               *UEnum::GetValueAsString(NewAlignment));
    }
}

void AAIDMPlayerCharacter::AddCredits(int32 Amount)
{
    PlayerStats.Credits += Amount;
    
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Added %d credits. Total: %d"), 
           Amount, PlayerStats.Credits);
}

EPlayerAlignment AAIDMPlayerCharacter::GetCurrentAlignment() const
{
    int32 TotalPoints = PlayerStats.LightSidePoints + PlayerStats.DarkSidePoints;
    
    if (TotalPoints == 0)
    {
        return EPlayerAlignment::Neutral;
    }
    
    float LightRatio = static_cast<float>(PlayerStats.LightSidePoints) / static_cast<float>(TotalPoints);
    
    if (LightRatio > 0.6f)
    {
        return EPlayerAlignment::Light;
    }
    else if (LightRatio < 0.4f)
    {
        return EPlayerAlignment::Dark;
    }
    else
    {
        return EPlayerAlignment::Neutral;
    }
}

void AAIDMPlayerCharacter::ToggleDebugPanel()
{
    if (DebugWidget)
    {
        DebugWidget->ToggleDebugWidget();
    }
}

void AAIDMPlayerCharacter::ToggleQuestLog()
{
    if (QuestLogWidget)
    {
        QuestLogWidget->ToggleQuestLog();
        QuestLogWidget->RefreshQuestLog(QuestManager);
    }
}

void AAIDMPlayerCharacter::TravelToPlanet(int32 PlanetIndex)
{
    if (AIDirector)
    {
        AIDirector->ChangeToPlanet(PlanetIndex);
    }
}

void AAIDMPlayerCharacter::TravelToLayout(const FString& LayoutName)
{
    if (AIDirector)
    {
        AIDirector->ChangeToLayout(LayoutName);
    }
}

void AAIDMPlayerCharacter::CreateUIWidgets()
{
    if (!GetWorld() || !GetWorld()->GetFirstPlayerController())
    {
        return;
    }
    
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    
    // Create dialogue widget
    if (DialogueWidgetClass && !DialogueWidget)
    {
        DialogueWidget = CreateWidget<UDialogueWidget>(PC, DialogueWidgetClass);
        if (DialogueWidget)
        {
            DialogueWidget->AddToViewport();
        }
    }
    
    // Create quest log widget
    if (QuestLogWidgetClass && !QuestLogWidget)
    {
        QuestLogWidget = CreateWidget<UQuestLogWidget>(PC, QuestLogWidgetClass);
        if (QuestLogWidget)
        {
            QuestLogWidget->AddToViewport();
        }
    }
    
    // Create debug widget
    if (DebugWidgetClass && !DebugWidget && bDebugModeEnabled)
    {
        DebugWidget = CreateWidget<UAIDMDebugWidget>(PC, DebugWidgetClass);
        if (DebugWidget)
        {
            DebugWidget->AddToViewport();
            DebugWidget->InitializeDebugWidget(CampaignLoader, AIDirector, QuestManager);
        }
    }
}

void AAIDMPlayerCharacter::UpdateInteractable()
{
    AActor* NewInteractable = FindNearestInteractable();
    
    if (NewInteractable != CurrentInteractable)
    {
        CurrentInteractable = NewInteractable;
        
        // Update interaction UI here if needed
        if (CurrentInteractable)
        {
            FString InteractionText = GetCustomInteractionText(CurrentInteractable);
            if (InteractionText.IsEmpty())
            {
                InteractionText = TEXT("Press E to interact");
            }
            
            // Display interaction prompt (would typically be done through UI)
            UE_LOG(LogTemp, VeryVerbose, TEXT("Can interact with: %s"), *CurrentInteractable->GetName());
        }
    }
}

AActor* AAIDMPlayerCharacter::FindNearestInteractable()
{
    if (!GetWorld())
    {
        return nullptr;
    }
    
    FVector PlayerLocation = GetActorLocation();
    AActor* NearestActor = nullptr;
    float NearestDistance = InteractionRange;
    
    // Find all actors within interaction range
    for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        AActor* Actor = *ActorItr;
        
        if (Actor && Actor != this && IsActorInteractable(Actor))
        {
            float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
            
            if (Distance < NearestDistance)
            {
                NearestDistance = Distance;
                NearestActor = Actor;
            }
        }
    }
    
    return NearestActor;
}

bool AAIDMPlayerCharacter::IsActorInteractable(AActor* Actor) const
{
    if (!Actor)
    {
        return false;
    }
    
    // Check if actor has an "Interactable" tag or component
    // This is a simplified check - in a real implementation you'd have a proper interface
    return Actor->Tags.Contains(TEXT("NPC")) || 
           Actor->Tags.Contains(TEXT("Interactable")) ||
           Actor->GetName().Contains(TEXT("NPC"));
}

void AAIDMPlayerCharacter::OnDebugToggle()
{
    if (bDebugModeEnabled)
    {
        ToggleDebugPanel();
    }
}

void AAIDMPlayerCharacter::OnQuestLogToggle()
{
    ToggleQuestLog();
}

void AAIDMPlayerCharacter::OnInteract()
{
    if (CurrentInteractable)
    {
        // Check if it's an NPC (simplified check)
        if (CurrentInteractable->Tags.Contains(TEXT("NPC")))
        {
            // Create dummy NPC data for testing
            FNPCData TestNPC;
            TestNPC.Name = CurrentInteractable->GetName();
            TestNPC.Species = TEXT("Human");
            TestNPC.Role = TEXT("Civilian");
            TestNPC.DialogueStyle = TEXT("Friendly");
            TestNPC.Backstory = TEXT("A test NPC for AIDM integration.");
            
            InteractWithNPC(TestNPC);
        }
        else
        {
            // Generic interaction
            UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Interacted with %s"), *CurrentInteractable->GetName());
        }
    }
}

// Event handlers
void AAIDMPlayerCharacter::OnQuestStarted(const FActiveQuest& Quest)
{
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Quest started: %s"), *Quest.QuestData.Title);
    
    // Refresh quest log if it's open
    if (QuestLogWidget)
    {
        QuestLogWidget->RefreshQuestLog(QuestManager);
    }
}

void AAIDMPlayerCharacter::OnQuestCompleted(const FActiveQuest& Quest)
{
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Quest completed: %s"), *Quest.QuestData.Title);
    
    // Award experience and credits for quest completion
    AddExperience(100);
    AddCredits(500);
    
    // Refresh quest log
    if (QuestLogWidget)
    {
        QuestLogWidget->RefreshQuestLog(QuestManager);
    }
}

void AAIDMPlayerCharacter::OnCampaignLoaded(const FCampaignPlan& Campaign)
{
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Campaign loaded: %s"), *Campaign.Config.StorySeed);
    
    // Set initial planet and layout
    if (Campaign.Planets.Num() > 0)
    {
        FPlanetData FirstPlanet = Campaign.Planets[0];
        if (FirstPlanet.Layouts.Num() > 0)
        {
            OnAreaEntered(FirstPlanet, FirstPlanet.Layouts[0]);
        }
    }
}

void AAIDMPlayerCharacter::OnPlanetChanged(int32 OldPlanetIndex, int32 NewPlanetIndex)
{
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Planet changed from %d to %d"), 
           OldPlanetIndex, NewPlanetIndex);
    
    if (CampaignLoader && CampaignLoader->IsCampaignLoaded())
    {
        FPlanetData NewPlanet = CampaignLoader->GetPlanetData(NewPlanetIndex);
        if (NewPlanet.Layouts.Num() > 0)
        {
            OnAreaEntered(NewPlanet, NewPlanet.Layouts[0]);
        }
    }
}

void AAIDMPlayerCharacter::OnLayoutChanged(const FString& OldLayout, const FString& NewLayout)
{
    UE_LOG(LogTemp, Log, TEXT("AIDMPlayerCharacter: Layout changed from %s to %s"), 
           *OldLayout, *NewLayout);
    
    if (AIDirector && AIDirector->IsInitialized())
    {
        FPlanetData CurrentPlanet = AIDirector->GetCurrentPlanetData();
        FMapLayout CurrentLayout = AIDirector->GetCurrentLayoutData();
        OnAreaEntered(CurrentPlanet, CurrentLayout);
    }
}
