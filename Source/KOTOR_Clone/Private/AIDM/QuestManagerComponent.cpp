// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIDM/QuestManagerComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UQuestManagerComponent::UQuestManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    NextQuestID = 1;
    bDebugMode = false;
}

void UQuestManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Initialized"));
}

FString UQuestManagerComponent::StartQuest(const FQuestData& QuestData, const FString& QuestGiverName, 
                                          int32 PlanetIndex, const FString& LayoutName)
{
    // Create new active quest
    FActiveQuest NewQuest;
    NewQuest.QuestID = GenerateQuestID();
    NewQuest.QuestData = QuestData;
    NewQuest.State = EQuestState::Active;
    NewQuest.StartTime = GetWorld()->GetTimeSeconds();
    NewQuest.QuestGiverName = QuestGiverName;
    NewQuest.PlanetIndex = PlanetIndex;
    NewQuest.LayoutName = LayoutName;
    
    // Create default objectives based on quest type
    CreateDefaultObjectives(NewQuest);
    
    // Add to active quests
    ActiveQuests.Add(NewQuest);
    
    // Broadcast events
    OnQuestStarted.Broadcast(NewQuest);
    OnQuestStartedEvent(NewQuest);
    
    if (bDebugMode)
    {
        LogQuestEvent(TEXT("STARTED"), NewQuest);
    }
    
    return NewQuest.QuestID;
}

bool UQuestManagerComponent::CompleteQuest(const FString& QuestID)
{
    FActiveQuest* Quest = FindActiveQuest(QuestID);
    if (!Quest)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Quest %s not found or not active"), *QuestID);
        return false;
    }
    
    // Update quest state
    Quest->State = EQuestState::Completed;
    Quest->CompletionTime = GetWorld()->GetTimeSeconds();
    
    // Move to completed quests
    CompletedQuests.Add(*Quest);
    ActiveQuests.RemoveAll([QuestID](const FActiveQuest& Q) { return Q.QuestID == QuestID; });
    
    // Broadcast events
    OnQuestCompleted.Broadcast(*Quest);
    OnQuestCompletedEvent(*Quest);
    
    if (bDebugMode)
    {
        LogQuestEvent(TEXT("COMPLETED"), *Quest);
    }
    
    return true;
}

bool UQuestManagerComponent::FailQuest(const FString& QuestID)
{
    FActiveQuest* Quest = FindActiveQuest(QuestID);
    if (!Quest)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Quest %s not found or not active"), *QuestID);
        return false;
    }
    
    // Update quest state
    Quest->State = EQuestState::Failed;
    Quest->CompletionTime = GetWorld()->GetTimeSeconds();
    
    // Move to failed quests
    FailedQuests.Add(*Quest);
    ActiveQuests.RemoveAll([QuestID](const FActiveQuest& Q) { return Q.QuestID == QuestID; });
    
    // Broadcast events
    OnQuestFailed.Broadcast(*Quest);
    
    if (bDebugMode)
    {
        LogQuestEvent(TEXT("FAILED"), *Quest);
    }
    
    return true;
}

bool UQuestManagerComponent::UpdateQuestObjective(const FString& QuestID, int32 ObjectiveIndex, int32 Progress)
{
    FActiveQuest* Quest = FindActiveQuest(QuestID);
    if (!Quest)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Quest %s not found or not active"), *QuestID);
        return false;
    }
    
    if (ObjectiveIndex < 0 || ObjectiveIndex >= Quest->Objectives.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManagerComponent: Invalid objective index %d for quest %s"), 
               ObjectiveIndex, *QuestID);
        return false;
    }
    
    FQuestObjective& Objective = Quest->Objectives[ObjectiveIndex];
    
    // Don't update if already completed
    if (Objective.bIsCompleted)
    {
        return false;
    }
    
    // Update progress
    Objective.CurrentProgress = FMath::Min(Objective.CurrentProgress + Progress, Objective.RequiredProgress);
    
    // Check if objective is now completed
    if (Objective.CurrentProgress >= Objective.RequiredProgress)
    {
        Objective.bIsCompleted = true;
        
        if (bDebugMode)
        {
            UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Objective %d completed for quest %s"), 
                   ObjectiveIndex, *QuestID);
        }
    }
    
    // Broadcast objective updated event
    OnQuestObjectiveUpdated.Broadcast(QuestID, ObjectiveIndex);
    OnQuestObjectiveUpdatedEvent(*Quest, ObjectiveIndex);
    
    // Check if all objectives are completed
    if (AreAllObjectivesCompleted(*Quest))
    {
        CompleteQuest(QuestID);
    }
    
    return true;
}

FActiveQuest UQuestManagerComponent::GetActiveQuest(const FString& QuestID) const
{
    for (const FActiveQuest& Quest : ActiveQuests)
    {
        if (Quest.QuestID == QuestID)
        {
            return Quest;
        }
    }
    
    return FActiveQuest();
}

TArray<FActiveQuest> UQuestManagerComponent::GetActiveQuests() const
{
    return ActiveQuests;
}

TArray<FActiveQuest> UQuestManagerComponent::GetCompletedQuests() const
{
    return CompletedQuests;
}

bool UQuestManagerComponent::IsQuestActive(const FString& QuestID) const
{
    return FindActiveQuest(QuestID) != nullptr;
}

bool UQuestManagerComponent::IsQuestCompleted(const FString& QuestID) const
{
    for (const FActiveQuest& Quest : CompletedQuests)
    {
        if (Quest.QuestID == QuestID)
        {
            return true;
        }
    }
    
    return false;
}

float UQuestManagerComponent::GetQuestCompletionPercentage(const FString& QuestID) const
{
    const FActiveQuest* Quest = FindActiveQuest(QuestID);
    if (!Quest)
    {
        // Check if it's completed
        if (IsQuestCompleted(QuestID))
        {
            return 1.0f;
        }
        return 0.0f;
    }
    
    if (Quest->Objectives.Num() == 0)
    {
        return 0.0f;
    }
    
    int32 CompletedObjectives = 0;
    int32 TotalProgress = 0;
    int32 MaxProgress = 0;
    
    for (const FQuestObjective& Objective : Quest->Objectives)
    {
        if (Objective.bIsCompleted)
        {
            CompletedObjectives++;
        }
        
        TotalProgress += Objective.CurrentProgress;
        MaxProgress += Objective.RequiredProgress;
    }
    
    if (MaxProgress == 0)
    {
        return 0.0f;
    }
    
    return static_cast<float>(TotalProgress) / static_cast<float>(MaxProgress);
}

void UQuestManagerComponent::ClearAllQuests()
{
    ActiveQuests.Empty();
    CompletedQuests.Empty();
    FailedQuests.Empty();
    NextQuestID = 1;
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Cleared all quests"));
    }
}

FString UQuestManagerComponent::SaveQuestData() const
{
    TSharedPtr<FJsonObject> SaveObject = MakeShareable(new FJsonObject);
    
    // Save next quest ID
    SaveObject->SetNumberField(TEXT("next_quest_id"), NextQuestID);
    
    // Save active quests
    TArray<TSharedPtr<FJsonValue>> ActiveQuestsArray;
    for (const FActiveQuest& Quest : ActiveQuests)
    {
        TSharedPtr<FJsonObject> QuestObject = MakeShareable(new FJsonObject);
        QuestObject->SetStringField(TEXT("quest_id"), Quest.QuestID);
        QuestObject->SetStringField(TEXT("title"), Quest.QuestData.Title);
        QuestObject->SetStringField(TEXT("description"), Quest.QuestData.Description);
        QuestObject->SetStringField(TEXT("quest_type"), Quest.QuestData.QuestType);
        QuestObject->SetNumberField(TEXT("state"), static_cast<int32>(Quest.State));
        QuestObject->SetNumberField(TEXT("start_time"), Quest.StartTime);
        QuestObject->SetStringField(TEXT("quest_giver"), Quest.QuestGiverName);
        QuestObject->SetNumberField(TEXT("planet_index"), Quest.PlanetIndex);
        QuestObject->SetStringField(TEXT("layout_name"), Quest.LayoutName);
        
        // Save objectives
        TArray<TSharedPtr<FJsonValue>> ObjectivesArray;
        for (const FQuestObjective& Objective : Quest.Objectives)
        {
            TSharedPtr<FJsonObject> ObjectiveObject = MakeShareable(new FJsonObject);
            ObjectiveObject->SetStringField(TEXT("description"), Objective.Description);
            ObjectiveObject->SetBoolField(TEXT("is_completed"), Objective.bIsCompleted);
            ObjectiveObject->SetBoolField(TEXT("is_optional"), Objective.bIsOptional);
            ObjectiveObject->SetNumberField(TEXT("current_progress"), Objective.CurrentProgress);
            ObjectiveObject->SetNumberField(TEXT("required_progress"), Objective.RequiredProgress);
            
            ObjectivesArray.Add(MakeShareable(new FJsonValueObject(ObjectiveObject)));
        }
        QuestObject->SetArrayField(TEXT("objectives"), ObjectivesArray);
        
        ActiveQuestsArray.Add(MakeShareable(new FJsonValueObject(QuestObject)));
    }
    SaveObject->SetArrayField(TEXT("active_quests"), ActiveQuestsArray);
    
    // Save completed quests (simplified)
    TArray<TSharedPtr<FJsonValue>> CompletedQuestsArray;
    for (const FActiveQuest& Quest : CompletedQuests)
    {
        TSharedPtr<FJsonObject> QuestObject = MakeShareable(new FJsonObject);
        QuestObject->SetStringField(TEXT("quest_id"), Quest.QuestID);
        QuestObject->SetStringField(TEXT("title"), Quest.QuestData.Title);
        QuestObject->SetNumberField(TEXT("completion_time"), Quest.CompletionTime);
        
        CompletedQuestsArray.Add(MakeShareable(new FJsonValueObject(QuestObject)));
    }
    SaveObject->SetArrayField(TEXT("completed_quests"), CompletedQuestsArray);
    
    // Serialize to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(SaveObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UQuestManagerComponent::LoadQuestData(const FString& SaveData)
{
    if (SaveData.IsEmpty())
    {
        return false;
    }
    
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(SaveData);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("QuestManagerComponent: Failed to parse quest save data"));
        return false;
    }
    
    // Clear existing data
    ClearAllQuests();
    
    // Load next quest ID
    JsonObject->TryGetNumberField(TEXT("next_quest_id"), NextQuestID);
    
    // Load active quests (basic implementation)
    const TArray<TSharedPtr<FJsonValue>>* ActiveQuestsArray;
    if (JsonObject->TryGetArrayField(TEXT("active_quests"), ActiveQuestsArray))
    {
        for (const auto& QuestValue : *ActiveQuestsArray)
        {
            const TSharedPtr<FJsonObject>* QuestObject;
            if (QuestValue->TryGetObject(QuestObject))
            {
                FActiveQuest Quest;
                (*QuestObject)->TryGetStringField(TEXT("quest_id"), Quest.QuestID);
                (*QuestObject)->TryGetStringField(TEXT("title"), Quest.QuestData.Title);
                (*QuestObject)->TryGetStringField(TEXT("description"), Quest.QuestData.Description);
                (*QuestObject)->TryGetStringField(TEXT("quest_type"), Quest.QuestData.QuestType);
                
                int32 StateValue;
                if ((*QuestObject)->TryGetNumberField(TEXT("state"), StateValue))
                {
                    Quest.State = static_cast<EQuestState>(StateValue);
                }
                
                (*QuestObject)->TryGetNumberField(TEXT("start_time"), Quest.StartTime);
                (*QuestObject)->TryGetStringField(TEXT("quest_giver"), Quest.QuestGiverName);
                (*QuestObject)->TryGetNumberField(TEXT("planet_index"), Quest.PlanetIndex);
                (*QuestObject)->TryGetStringField(TEXT("layout_name"), Quest.LayoutName);
                
                // Create default objectives for loaded quest
                CreateDefaultObjectives(Quest);
                
                ActiveQuests.Add(Quest);
            }
        }
    }
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Loaded %d active quests"), ActiveQuests.Num());
    }
    
    return true;
}

FString UQuestManagerComponent::GenerateQuestID()
{
    FString QuestID = FString::Printf(TEXT("QUEST_%04d"), NextQuestID);
    NextQuestID++;
    return QuestID;
}

FActiveQuest* UQuestManagerComponent::FindActiveQuest(const FString& QuestID)
{
    for (FActiveQuest& Quest : ActiveQuests)
    {
        if (Quest.QuestID == QuestID)
        {
            return &Quest;
        }
    }

    return nullptr;
}

const FActiveQuest* UQuestManagerComponent::FindActiveQuest(const FString& QuestID) const
{
    for (const FActiveQuest& Quest : ActiveQuests)
    {
        if (Quest.QuestID == QuestID)
        {
            return &Quest;
        }
    }

    return nullptr;
}

void UQuestManagerComponent::CreateDefaultObjectives(FActiveQuest& Quest)
{
    Quest.Objectives.Empty();

    // Create objectives based on quest type
    if (Quest.QuestData.QuestType == TEXT("fetch"))
    {
        FQuestObjective Objective;
        Objective.Description = FString::Printf(TEXT("Retrieve the required item"));
        Objective.RequiredProgress = 1;
        Quest.Objectives.Add(Objective);
    }
    else if (Quest.QuestData.QuestType == TEXT("kill"))
    {
        FQuestObjective Objective;
        Objective.Description = FString::Printf(TEXT("Eliminate the target"));
        Objective.RequiredProgress = 1;
        Quest.Objectives.Add(Objective);
    }
    else if (Quest.QuestData.QuestType == TEXT("escort"))
    {
        FQuestObjective Objective1;
        Objective1.Description = TEXT("Meet the person to escort");
        Objective1.RequiredProgress = 1;
        Quest.Objectives.Add(Objective1);

        FQuestObjective Objective2;
        Objective2.Description = TEXT("Safely escort to destination");
        Objective2.RequiredProgress = 1;
        Quest.Objectives.Add(Objective2);
    }
    else if (Quest.QuestData.QuestType == TEXT("investigate"))
    {
        FQuestObjective Objective1;
        Objective1.Description = TEXT("Gather clues");
        Objective1.RequiredProgress = 3;
        Quest.Objectives.Add(Objective1);

        FQuestObjective Objective2;
        Objective2.Description = TEXT("Solve the mystery");
        Objective2.RequiredProgress = 1;
        Quest.Objectives.Add(Objective2);
    }
    else if (Quest.QuestData.QuestType == TEXT("diplomacy"))
    {
        FQuestObjective Objective;
        Objective.Description = TEXT("Successfully negotiate");
        Objective.RequiredProgress = 1;
        Quest.Objectives.Add(Objective);
    }
    else
    {
        // Default objective
        FQuestObjective Objective;
        Objective.Description = Quest.QuestData.Description;
        Objective.RequiredProgress = 1;
        Quest.Objectives.Add(Objective);
    }
}

bool UQuestManagerComponent::AreAllObjectivesCompleted(const FActiveQuest& Quest) const
{
    for (const FQuestObjective& Objective : Quest.Objectives)
    {
        if (!Objective.bIsOptional && !Objective.bIsCompleted)
        {
            return false;
        }
    }

    return true;
}

void UQuestManagerComponent::LogQuestEvent(const FString& Event, const FActiveQuest& Quest) const
{
    UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: Quest %s - %s: %s"),
           *Event, *Quest.QuestID, *Quest.QuestData.Title);

    if (Event == TEXT("STARTED"))
    {
        UE_LOG(LogTemp, Log, TEXT("  Given by: %s"), *Quest.QuestGiverName);
        UE_LOG(LogTemp, Log, TEXT("  Location: Planet %d, %s"), Quest.PlanetIndex, *Quest.LayoutName);
        UE_LOG(LogTemp, Log, TEXT("  Type: %s"), *Quest.QuestData.QuestType);
        UE_LOG(LogTemp, Log, TEXT("  Objectives: %d"), Quest.Objectives.Num());
    }
    else if (Event == TEXT("COMPLETED"))
    {
        float Duration = Quest.CompletionTime - Quest.StartTime;
        UE_LOG(LogTemp, Log, TEXT("  Duration: %.1f seconds"), Duration);
    }
}
