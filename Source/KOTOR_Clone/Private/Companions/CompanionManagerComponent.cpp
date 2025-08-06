// Copyright Epic Games, Inc. All Rights Reserved.

#include "Companions/CompanionManagerComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UCompanionManagerComponent::UCompanionManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    
    // Initialize default settings
    MaxPartySize = 2; // Player + 2 companions (KOTOR style)
    CommentaryFrequency = 30.0f; // 30 seconds between automatic commentary
    bEnableAutomaticCommentary = true;
    
    CampaignLoaderRef = nullptr;
    QuestManagerRef = nullptr;
}

void UCompanionManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Initialized"));
}

void UCompanionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update companion AI
    for (FActiveCompanion& Companion : ActiveCompanions)
    {
        if (Companion.bIsInParty && Companion.bIsAlive)
        {
            UpdateCompanionAI(Companion, DeltaTime);
        }
    }
}

void UCompanionManagerComponent::InitializeCompanionSystem(UCampaignLoaderSubsystem* CampaignLoader, UQuestManagerComponent* QuestManager)
{
    CampaignLoaderRef = CampaignLoader;
    QuestManagerRef = QuestManager;
    
    // Load companions from campaign data
    LoadCompanionsFromCampaign();
    
    UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Initialized with %d available companions"), 
           AvailableCompanions.Num());
}

bool UCompanionManagerComponent::RecruitCompanion(const FString& CompanionName)
{
    // Check if companion is already recruited
    if (IsCompanionRecruited(CompanionName))
    {
        UE_LOG(LogTemp, Warning, TEXT("CompanionManagerComponent: %s is already recruited"), *CompanionName);
        return false;
    }
    
    // Find companion in available companions
    FCompanionData* FoundCompanion = nullptr;
    for (FCompanionData& Companion : AvailableCompanions)
    {
        if (Companion.Name == CompanionName)
        {
            FoundCompanion = &Companion;
            break;
        }
    }
    
    if (!FoundCompanion)
    {
        UE_LOG(LogTemp, Warning, TEXT("CompanionManagerComponent: Companion %s not found"), *CompanionName);
        return false;
    }
    
    // Create active companion
    FActiveCompanion NewActiveCompanion;
    NewActiveCompanion.CompanionData = *FoundCompanion;
    NewActiveCompanion.bIsRecruited = true;
    NewActiveCompanion.Loyalty = ECompanionLoyalty::Friendly; // Start as friendly
    NewActiveCompanion.LoyaltyPoints = 50; // Neutral starting loyalty
    
    ActiveCompanions.Add(NewActiveCompanion);
    
    // Broadcast recruitment event
    OnCompanionRecruited.Broadcast(NewActiveCompanion);
    
    UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Recruited %s"), *CompanionName);
    
    return true;
}

bool UCompanionManagerComponent::AddCompanionToParty(const FString& CompanionName)
{
    // Check if companion is recruited
    if (!IsCompanionRecruited(CompanionName))
    {
        UE_LOG(LogTemp, Warning, TEXT("CompanionManagerComponent: %s is not recruited"), *CompanionName);
        return false;
    }
    
    // Check if companion is already in party
    if (IsCompanionInParty(CompanionName))
    {
        UE_LOG(LogTemp, Warning, TEXT("CompanionManagerComponent: %s is already in party"), *CompanionName);
        return false;
    }
    
    // Check party size limit
    TArray<FActiveCompanion> CurrentParty = GetPartyMembers();
    if (CurrentParty.Num() >= MaxPartySize)
    {
        UE_LOG(LogTemp, Warning, TEXT("CompanionManagerComponent: Party is full (%d/%d)"), 
               CurrentParty.Num(), MaxPartySize);
        return false;
    }
    
    // Find and add companion to party
    FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    if (Companion)
    {
        Companion->bIsInParty = true;
        Companion->CurrentOrder = ECompanionOrder::Follow;
        
        // Spawn companion pawn
        SpawnCompanionPawn(*Companion);
        
        UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Added %s to party"), *CompanionName);
        return true;
    }
    
    return false;
}

bool UCompanionManagerComponent::RemoveCompanionFromParty(const FString& CompanionName)
{
    FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    if (Companion && Companion->bIsInParty)
    {
        Companion->bIsInParty = false;
        
        // Despawn companion pawn
        DespawnCompanionPawn(*Companion);
        
        UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Removed %s from party"), *CompanionName);
        return true;
    }
    
    return false;
}

void UCompanionManagerComponent::GiveCompanionOrder(const FString& CompanionName, ECompanionOrder Order, AActor* Target)
{
    FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    if (Companion && Companion->bIsInParty)
    {
        Companion->CurrentOrder = Order;
        
        // Broadcast order event
        OnCompanionOrderGiven.Broadcast(*Companion, Order);
        OnCompanionOrderReceived(*Companion, Order, Target);
        
        UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Gave order %s to %s"), 
               *UEnum::GetValueAsString(Order), *CompanionName);
    }
}

void UCompanionManagerComponent::GiveOrderToAllCompanions(ECompanionOrder Order, AActor* Target)
{
    TArray<FActiveCompanion> PartyMembers = GetPartyMembers();
    
    for (const FActiveCompanion& Companion : PartyMembers)
    {
        GiveCompanionOrder(Companion.CompanionData.Name, Order, Target);
    }
}

void UCompanionManagerComponent::AdjustCompanionLoyalty(const FString& CompanionName, int32 LoyaltyChange, const FString& Reason)
{
    FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    if (!Companion)
    {
        return;
    }
    
    ECompanionLoyalty OldLoyalty = Companion->Loyalty;
    Companion->LoyaltyPoints += LoyaltyChange;
    
    // Clamp loyalty points
    Companion->LoyaltyPoints = FMath::Clamp(Companion->LoyaltyPoints, -100, 100);
    
    // Update loyalty level
    ECompanionLoyalty NewLoyalty = CalculateLoyaltyLevel(Companion->LoyaltyPoints);
    
    if (NewLoyalty != OldLoyalty)
    {
        Companion->Loyalty = NewLoyalty;
        
        // Record relationship history
        Companion->RelationshipHistory.Add(Reason, static_cast<float>(LoyaltyChange));
        
        // Broadcast loyalty change event
        OnCompanionLoyaltyChanged.Broadcast(*Companion);
        OnCompanionLoyaltyChangedEvent(*Companion, OldLoyalty, NewLoyalty);
        
        UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: %s loyalty changed from %s to %s (Reason: %s)"), 
               *CompanionName, *UEnum::GetValueAsString(OldLoyalty), 
               *UEnum::GetValueAsString(NewLoyalty), *Reason);
    }
}

void UCompanionManagerComponent::TriggerCompanionCommentary(const FString& EventType, const FString& Context)
{
    if (!bEnableAutomaticCommentary)
    {
        return;
    }
    
    TArray<FActiveCompanion> PartyMembers = GetPartyMembers();
    
    for (FActiveCompanion& Companion : PartyMembers)
    {
        if (ShouldCompanionComment(Companion, EventType))
        {
            // Try custom commentary first
            FString Commentary = GenerateCustomCommentary(Companion, EventType, Context);
            
            if (Commentary.IsEmpty())
            {
                // Generate default commentary
                Commentary = GenerateCompanionCommentary(Companion, EventType, Context);
            }
            
            if (!Commentary.IsEmpty())
            {
                Companion.LastCommentaryTime = GetWorld()->GetTimeSeconds();
                OnCompanionCommentary.Broadcast(Companion, Commentary);
                
                UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: %s says: %s"), 
                       *Companion.CompanionData.Name, *Commentary);
            }
        }
    }
}

FActiveCompanion UCompanionManagerComponent::GetActiveCompanion(const FString& CompanionName) const
{
    const FActiveCompanion* Found = FindActiveCompanion(CompanionName);
    return Found ? *Found : FActiveCompanion();
}

TArray<FActiveCompanion> UCompanionManagerComponent::GetRecruitedCompanions() const
{
    TArray<FActiveCompanion> Recruited;
    
    for (const FActiveCompanion& Companion : ActiveCompanions)
    {
        if (Companion.bIsRecruited)
        {
            Recruited.Add(Companion);
        }
    }
    
    return Recruited;
}

TArray<FActiveCompanion> UCompanionManagerComponent::GetPartyMembers() const
{
    TArray<FActiveCompanion> PartyMembers;
    
    for (const FActiveCompanion& Companion : ActiveCompanions)
    {
        if (Companion.bIsInParty && Companion.bIsAlive)
        {
            PartyMembers.Add(Companion);
        }
    }
    
    return PartyMembers;
}

bool UCompanionManagerComponent::IsCompanionRecruited(const FString& CompanionName) const
{
    const FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    return Companion && Companion->bIsRecruited;
}

bool UCompanionManagerComponent::IsCompanionInParty(const FString& CompanionName) const
{
    const FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    return Companion && Companion->bIsInParty && Companion->bIsAlive;
}

ECompanionLoyalty UCompanionManagerComponent::GetCompanionLoyalty(const FString& CompanionName) const
{
    const FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
    return Companion ? Companion->Loyalty : ECompanionLoyalty::Neutral;
}

FString UCompanionManagerComponent::SaveCompanionData() const
{
    TSharedPtr<FJsonObject> SaveObject = MakeShareable(new FJsonObject);
    
    // Save active companions
    TArray<TSharedPtr<FJsonValue>> CompanionsArray;
    for (const FActiveCompanion& Companion : ActiveCompanions)
    {
        TSharedPtr<FJsonObject> CompanionObject = MakeShareable(new FJsonObject);
        CompanionObject->SetStringField(TEXT("name"), Companion.CompanionData.Name);
        CompanionObject->SetBoolField(TEXT("is_recruited"), Companion.bIsRecruited);
        CompanionObject->SetBoolField(TEXT("is_in_party"), Companion.bIsInParty);
        CompanionObject->SetNumberField(TEXT("loyalty_points"), Companion.LoyaltyPoints);
        CompanionObject->SetNumberField(TEXT("loyalty"), static_cast<int32>(Companion.Loyalty));
        CompanionObject->SetNumberField(TEXT("current_order"), static_cast<int32>(Companion.CurrentOrder));
        
        // Save relationship history
        TSharedPtr<FJsonObject> HistoryObject = MakeShareable(new FJsonObject);
        for (const auto& Entry : Companion.RelationshipHistory)
        {
            HistoryObject->SetNumberField(Entry.Key, Entry.Value);
        }
        CompanionObject->SetObjectField(TEXT("relationship_history"), HistoryObject);
        
        CompanionsArray.Add(MakeShareable(new FJsonValueObject(CompanionObject)));
    }
    SaveObject->SetArrayField(TEXT("active_companions"), CompanionsArray);
    
    // Serialize to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(SaveObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UCompanionManagerComponent::LoadCompanionData(const FString& SaveData)
{
    if (SaveData.IsEmpty())
    {
        return false;
    }
    
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(SaveData);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("CompanionManagerComponent: Failed to parse companion save data"));
        return false;
    }
    
    // Load active companions
    const TArray<TSharedPtr<FJsonValue>>* CompanionsArray;
    if (JsonObject->TryGetArrayField(TEXT("active_companions"), CompanionsArray))
    {
        for (const auto& CompanionValue : *CompanionsArray)
        {
            const TSharedPtr<FJsonObject>* CompanionObject;
            if (CompanionValue->TryGetObject(CompanionObject))
            {
                FString CompanionName;
                if ((*CompanionObject)->TryGetStringField(TEXT("name"), CompanionName))
                {
                    // Find companion in available companions and recruit if needed
                    if (!IsCompanionRecruited(CompanionName))
                    {
                        RecruitCompanion(CompanionName);
                    }
                    
                    // Update companion data
                    FActiveCompanion* Companion = FindActiveCompanion(CompanionName);
                    if (Companion)
                    {
                        (*CompanionObject)->TryGetBoolField(TEXT("is_recruited"), Companion->bIsRecruited);
                        (*CompanionObject)->TryGetBoolField(TEXT("is_in_party"), Companion->bIsInParty);
                        (*CompanionObject)->TryGetNumberField(TEXT("loyalty_points"), Companion->LoyaltyPoints);
                        
                        int32 LoyaltyValue;
                        if ((*CompanionObject)->TryGetNumberField(TEXT("loyalty"), LoyaltyValue))
                        {
                            Companion->Loyalty = static_cast<ECompanionLoyalty>(LoyaltyValue);
                        }
                        
                        int32 OrderValue;
                        if ((*CompanionObject)->TryGetNumberField(TEXT("current_order"), OrderValue))
                        {
                            Companion->CurrentOrder = static_cast<ECompanionOrder>(OrderValue);
                        }
                        
                        // Load relationship history
                        const TSharedPtr<FJsonObject>* HistoryObject;
                        if ((*CompanionObject)->TryGetObjectField(TEXT("relationship_history"), HistoryObject))
                        {
                            for (const auto& Entry : (*HistoryObject)->Values)
                            {
                                double Value;
                                if (Entry.Value->TryGetNumber(Value))
                                {
                                    Companion->RelationshipHistory.Add(Entry.Key, static_cast<float>(Value));
                                }
                            }
                        }
                        
                        // Spawn companion if in party
                        if (Companion->bIsInParty)
                        {
                            SpawnCompanionPawn(*Companion);
                        }
                    }
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Loaded companion data"));
    return true;
}

// Private helper methods
FActiveCompanion* UCompanionManagerComponent::FindActiveCompanion(const FString& CompanionName)
{
    for (FActiveCompanion& Companion : ActiveCompanions)
    {
        if (Companion.CompanionData.Name == CompanionName)
        {
            return &Companion;
        }
    }
    return nullptr;
}

const FActiveCompanion* UCompanionManagerComponent::FindActiveCompanion(const FString& CompanionName) const
{
    for (const FActiveCompanion& Companion : ActiveCompanions)
    {
        if (Companion.CompanionData.Name == CompanionName)
        {
            return &Companion;
        }
    }
    return nullptr;
}

void UCompanionManagerComponent::LoadCompanionsFromCampaign()
{
    // For now, create some default companions
    // In a full implementation, this would load from campaign data
    
    AvailableCompanions.Empty();
    
    // Create default companions
    FCompanionData Bastila;
    Bastila.Name = TEXT("Bastila Shan");
    Bastila.Species = TEXT("Human");
    Bastila.Role = ECompanionRole::Support;
    Bastila.Alignment = TEXT("light");
    Bastila.Backstory = TEXT("A young Jedi with a rare gift of Battle Meditation.");
    Bastila.PersonalityTraits.Add(TEXT("Confident"));
    Bastila.PersonalityTraits.Add(TEXT("Dedicated"));
    Bastila.PersonalityTraits.Add(TEXT("Sometimes Arrogant"));
    Bastila.VoiceProfile = TEXT("human_female_01");
    AvailableCompanions.Add(Bastila);
    
    FCompanionData Carth;
    Carth.Name = TEXT("Carth Onasi");
    Carth.Species = TEXT("Human");
    Carth.Role = ECompanionRole::DPS;
    Carth.Alignment = TEXT("light");
    Carth.Backstory = TEXT("A Republic pilot with trust issues.");
    Carth.PersonalityTraits.Add(TEXT("Suspicious"));
    Carth.PersonalityTraits.Add(TEXT("Loyal"));
    Carth.PersonalityTraits.Add(TEXT("Protective"));
    Carth.VoiceProfile = TEXT("human_male_01");
    AvailableCompanions.Add(Carth);
    
    FCompanionData HK47;
    HK47.Name = TEXT("HK-47");
    HK47.Species = TEXT("Droid");
    HK47.Role = ECompanionRole::DPS;
    HK47.Alignment = TEXT("dark");
    HK47.Backstory = TEXT("An assassin droid with a sardonic personality.");
    HK47.PersonalityTraits.Add(TEXT("Sarcastic"));
    HK47.PersonalityTraits.Add(TEXT("Violent"));
    HK47.PersonalityTraits.Add(TEXT("Logical"));
    HK47.VoiceProfile = TEXT("droid_01");
    AvailableCompanions.Add(HK47);
}

void UCompanionManagerComponent::SpawnCompanionPawn(FActiveCompanion& Companion)
{
    if (Companion.CompanionPawn)
    {
        return; // Already spawned
    }
    
    // Try Blueprint implementation first
    APawn* SpawnedPawn = OnSpawnCompanion(Companion.CompanionData);
    
    if (!SpawnedPawn)
    {
        // Fallback to default spawning
        TSubclassOf<APawn> PawnClass = DefaultCompanionPawnClass;
        
        // Check for species-specific pawn class
        if (CompanionPawnClasses.Contains(Companion.CompanionData.Species))
        {
            PawnClass = CompanionPawnClasses[Companion.CompanionData.Species];
        }
        
        if (PawnClass && GetWorld())
        {
            FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(100, 0, 0);
            FRotator SpawnRotation = GetOwner()->GetActorRotation();
            
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            
            SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
        }
    }
    
    if (SpawnedPawn)
    {
        Companion.CompanionPawn = SpawnedPawn;
        SpawnedPawn->SetActorLabel(FString::Printf(TEXT("Companion_%s"), *Companion.CompanionData.Name));
        
        UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Spawned companion pawn for %s"), 
               *Companion.CompanionData.Name);
    }
}

void UCompanionManagerComponent::DespawnCompanionPawn(FActiveCompanion& Companion)
{
    if (Companion.CompanionPawn && IsValid(Companion.CompanionPawn))
    {
        Companion.CompanionPawn->Destroy();
        Companion.CompanionPawn = nullptr;
        
        UE_LOG(LogTemp, Log, TEXT("CompanionManagerComponent: Despawned companion pawn for %s"), 
               *Companion.CompanionData.Name);
    }
}

ECompanionLoyalty UCompanionManagerComponent::CalculateLoyaltyLevel(int32 LoyaltyPoints) const
{
    if (LoyaltyPoints >= 80)
        return ECompanionLoyalty::Devoted;
    else if (LoyaltyPoints >= 60)
        return ECompanionLoyalty::Loyal;
    else if (LoyaltyPoints >= 20)
        return ECompanionLoyalty::Friendly;
    else if (LoyaltyPoints >= -20)
        return ECompanionLoyalty::Neutral;
    else if (LoyaltyPoints >= -60)
        return ECompanionLoyalty::Unfriendly;
    else
        return ECompanionLoyalty::Hostile;
}

FString UCompanionManagerComponent::GenerateCompanionCommentary(const FActiveCompanion& Companion, const FString& EventType, const FString& Context)
{
    // Generate default commentary based on companion personality and event type
    FString Commentary;
    
    if (EventType == TEXT("combat_start"))
    {
        if (Companion.CompanionData.Role == ECompanionRole::Tank)
        {
            Commentary = TEXT("I'll protect you!");
        }
        else if (Companion.CompanionData.Role == ECompanionRole::DPS)
        {
            Commentary = TEXT("Time to fight!");
        }
        else if (Companion.CompanionData.Role == ECompanionRole::Healer)
        {
            Commentary = TEXT("Stay close, I'll keep you healed!");
        }
    }
    else if (EventType == TEXT("quest_complete"))
    {
        if (Companion.Loyalty >= ECompanionLoyalty::Loyal)
        {
            Commentary = TEXT("Well done! I'm proud to fight alongside you.");
        }
        else if (Companion.Loyalty <= ECompanionLoyalty::Unfriendly)
        {
            Commentary = TEXT("Hmph. I suppose that went well enough.");
        }
        else
        {
            Commentary = TEXT("Good work.");
        }
    }
    else if (EventType == TEXT("moral_choice"))
    {
        if (Context.Contains(TEXT("light")) && Companion.CompanionData.Alignment == TEXT("light"))
        {
            Commentary = TEXT("I approve of your choice.");
        }
        else if (Context.Contains(TEXT("dark")) && Companion.CompanionData.Alignment == TEXT("dark"))
        {
            Commentary = TEXT("Now that's more like it!");
        }
        else if (Context.Contains(TEXT("dark")) && Companion.CompanionData.Alignment == TEXT("light"))
        {
            Commentary = TEXT("I... I don't agree with what you just did.");
        }
    }
    
    return Commentary;
}

void UCompanionManagerComponent::UpdateCompanionAI(FActiveCompanion& Companion, float DeltaTime)
{
    // Basic AI update - in a full implementation this would be much more complex
    if (!Companion.CompanionPawn)
    {
        return;
    }
    
    // Update based on current order
    switch (Companion.CurrentOrder)
    {
        case ECompanionOrder::Follow:
            // Follow the player
            break;
        case ECompanionOrder::Stay:
            // Stay in current position
            break;
        case ECompanionOrder::Attack:
            // Attack designated target
            break;
        case ECompanionOrder::Defend:
            // Defend the player
            break;
        case ECompanionOrder::Heal:
            // Heal party members
            break;
        case ECompanionOrder::Retreat:
            // Move to safety
            break;
        case ECompanionOrder::Free:
            // Act autonomously
            break;
    }
}

bool UCompanionManagerComponent::ShouldCompanionComment(const FActiveCompanion& Companion, const FString& EventType) const
{
    // Check if enough time has passed since last commentary
    float TimeSinceLastComment = GetWorld()->GetTimeSeconds() - Companion.LastCommentaryTime;
    
    if (TimeSinceLastComment < CommentaryFrequency)
    {
        return false;
    }
    
    // Check if companion would comment on this type of event
    // This could be based on personality traits, loyalty, etc.
    
    return true;
}
