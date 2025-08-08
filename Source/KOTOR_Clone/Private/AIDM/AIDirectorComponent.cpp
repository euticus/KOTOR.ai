// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIDM/AIDirectorComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

UAIDirectorComponent::UAIDirectorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    
    // Initialize default values
    bIsInitialized = false;
    CurrentPlanetIndex = -1;
    CurrentLayoutName = TEXT("");
    
    // Auto-spawn settings
    bAutoSpawnOnLayoutChange = true;
    bAutoSpawnNPCs = true;
    bAutoSpawnEnemies = true;
    bAutoSpawnLoot = true;
    
    // Debug settings
    bDebugMode = false;
    bShowSpawnPointDebug = false;
    
    CampaignLoader = nullptr;
}

void UAIDirectorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the campaign loader subsystem
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        CampaignLoader = GameInstance->GetSubsystem<UCampaignLoaderSubsystem>();
    }
    
    if (!CampaignLoader)
    {
        UE_LOG(LogTemp, Error, TEXT("AIDirectorComponent: Failed to get CampaignLoaderSubsystem"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Successfully initialized"));
    }
}

void UAIDirectorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearAllSpawnedContent();
    Super::EndPlay(EndPlayReason);
}

void UAIDirectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Debug drawing
    if (bDebugMode && bShowSpawnPointDebug)
    {
        DrawSpawnPointDebug();
    }
}

bool UAIDirectorComponent::InitializeWithCampaign(const FString& CampaignFilePath)
{
    if (!CampaignLoader)
    {
        UE_LOG(LogTemp, Error, TEXT("AIDirectorComponent: CampaignLoader is null"));
        return false;
    }
    
    // Load the campaign
    if (!CampaignLoader->LoadCampaign(CampaignFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("AIDirectorComponent: Failed to load campaign from %s"), *CampaignFilePath);
        return false;
    }
    
    // Initialize state
    bIsInitialized = true;
    CurrentPlanetIndex = 0; // Start with first planet
    
    // Get the first planet's first layout
    const FCampaignPlan& Campaign = CampaignLoader->GetCurrentCampaign();
    if (Campaign.Planets.Num() > 0 && Campaign.Planets[0].Layouts.Num() > 0)
    {
        CurrentLayoutName = Campaign.Planets[0].Layouts[0].Name;
    }
    
    // Broadcast campaign loaded event
    OnCampaignLoaded.Broadcast(Campaign);
    
    UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Successfully initialized with campaign: %s"), 
           *Campaign.Config.StorySeed);
    
    // Auto-spawn content if enabled
    if (bAutoSpawnOnLayoutChange)
    {
        SpawnContentForCurrentLayout();
    }
    
    return true;
}

bool UAIDirectorComponent::ChangeToPlanet(int32 PlanetIndex)
{
    if (!bIsInitialized || !CampaignLoader)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIDirectorComponent: Not initialized"));
        return false;
    }
    
    const FCampaignPlan& Campaign = CampaignLoader->GetCurrentCampaign();
    if (PlanetIndex < 0 || PlanetIndex >= Campaign.Planets.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("AIDirectorComponent: Invalid planet index %d"), PlanetIndex);
        return false;
    }
    
    int32 OldPlanetIndex = CurrentPlanetIndex;
    CurrentPlanetIndex = PlanetIndex;
    
    // Set to first layout of new planet
    const FPlanetData& Planet = Campaign.Planets[PlanetIndex];
    if (Planet.Layouts.Num() > 0)
    {
        FString OldLayout = CurrentLayoutName;
        CurrentLayoutName = Planet.Layouts[0].Name;
        
        // Clear old content
        ClearAllSpawnedContent();
        
        // Broadcast events
        OnPlanetChanged.Broadcast(OldPlanetIndex, CurrentPlanetIndex);
        OnLayoutChanged.Broadcast(OldLayout, CurrentLayoutName);
        
        // Call blueprint events
        OnPlanetChangedEvent(OldPlanetIndex, CurrentPlanetIndex);
        OnLayoutChangedEvent(OldLayout, CurrentLayoutName);
        
        // Auto-spawn content if enabled
        if (bAutoSpawnOnLayoutChange)
        {
            SpawnContentForCurrentLayout();
        }
        
        UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Changed to planet %d (%s), layout %s"), 
               PlanetIndex, *Planet.Name, *CurrentLayoutName);
        
        return true;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("AIDirectorComponent: Planet %d has no layouts"), PlanetIndex);
    return false;
}

bool UAIDirectorComponent::ChangeToLayout(const FString& LayoutName)
{
    if (!bIsInitialized || !CampaignLoader)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIDirectorComponent: Not initialized"));
        return false;
    }
    
    // Verify layout exists on current planet
    const FPlanetData CurrentPlanet = GetCurrentPlanetData();
    bool bLayoutFound = false;
    
    for (const FMapLayout& Layout : CurrentPlanet.Layouts)
    {
        if (Layout.Name == LayoutName)
        {
            bLayoutFound = true;
            break;
        }
    }
    
    if (!bLayoutFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIDirectorComponent: Layout %s not found on current planet"), *LayoutName);
        return false;
    }
    
    FString OldLayout = CurrentLayoutName;
    CurrentLayoutName = LayoutName;
    
    // Clear old content
    ClearAllSpawnedContent();
    
    // Broadcast events
    OnLayoutChanged.Broadcast(OldLayout, CurrentLayoutName);
    OnLayoutChangedEvent(OldLayout, CurrentLayoutName);
    
    // Auto-spawn content if enabled
    if (bAutoSpawnOnLayoutChange)
    {
        SpawnContentForCurrentLayout();
    }
    
    UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Changed to layout %s"), *LayoutName);
    
    return true;
}

void UAIDirectorComponent::RegisterSpawnPoint(const FAIDirectorSpawnData& SpawnPoint)
{
    RegisteredSpawnPoints.Add(SpawnPoint);
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Registered spawn point for %s at %s"), 
               *SpawnPoint.SpawnType, *SpawnPoint.Location.ToString());
    }
}

void UAIDirectorComponent::SpawnContentForCurrentLayout(bool bForceRespawn)
{
    if (!bIsInitialized || CurrentLayoutName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("AIDirectorComponent: Cannot spawn content - not properly initialized"));
        return;
    }
    
    // Clear existing content if force respawn
    if (bForceRespawn)
    {
        ClearAllSpawnedContent();
    }
    
    // Spawn different types of content
    if (bAutoSpawnNPCs)
    {
        SpawnNPCsForLayout();
    }
    
    if (bAutoSpawnEnemies)
    {
        SpawnEnemiesForLayout();
    }
    
    if (bAutoSpawnLoot)
    {
        SpawnLootForLayout();
    }
    
    SpawnQuestsForLayout();
    
    if (bDebugMode)
    {
        LogDirectorState();
    }
}

void UAIDirectorComponent::ClearAllSpawnedContent()
{
    // Destroy all spawned actors
    for (AActor* Actor : SpawnedActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    
    SpawnedActors.Empty();
    
    // Reset spawn point occupation
    for (FAIDirectorSpawnData& SpawnPoint : RegisteredSpawnPoints)
    {
        SpawnPoint.bIsOccupied = false;
        SpawnPoint.SpawnedActor = nullptr;
    }
    
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Cleared all spawned content"));
    }
}

FPlanetData UAIDirectorComponent::GetCurrentPlanetData() const
{
    if (!bIsInitialized || !CampaignLoader)
    {
        return FPlanetData();
    }
    
    return CampaignLoader->GetPlanetData(CurrentPlanetIndex);
}

FMapLayout UAIDirectorComponent::GetCurrentLayoutData() const
{
    const FPlanetData Planet = GetCurrentPlanetData();
    
    for (const FMapLayout& Layout : Planet.Layouts)
    {
        if (Layout.Name == CurrentLayoutName)
        {
            return Layout;
        }
    }
    
    return FMapLayout();
}

TArray<FAIDirectorSpawnData> UAIDirectorComponent::GetSpawnPointsByType(const FString& SpawnType) const
{
    TArray<FAIDirectorSpawnData> Result;
    
    for (const FAIDirectorSpawnData& SpawnPoint : RegisteredSpawnPoints)
    {
        if (SpawnPoint.SpawnType == SpawnType && SpawnPoint.LayoutName == CurrentLayoutName)
        {
            Result.Add(SpawnPoint);
        }
    }
    
    return Result;
}

void UAIDirectorComponent::SpawnNPCsForLayout()
{
    // Get NPCs for current location
    TArray<FNPCData> NPCs = CampaignLoader->GetNPCsForLocation(CurrentPlanetIndex, CurrentLayoutName);
    
    for (const FNPCData& NPCData : NPCs)
    {
        FAIDirectorSpawnData* SpawnPoint = FindAvailableSpawnPoint(TEXT("NPC"), CurrentLayoutName);
        if (SpawnPoint)
        {
            // Try blueprint implementation first
            APawn* SpawnedNPC = OnSpawnNPC(NPCData, *SpawnPoint);
            
            if (!SpawnedNPC && DefaultNPCClass)
            {
                // Fallback to default spawning
                TSubclassOf<APawn> NPCClass = GetNPCClassForSpecies(NPCData.Species);
                if (!NPCClass)
                {
                    NPCClass = DefaultNPCClass;
                }
                
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                
                SpawnedNPC = GetWorld()->SpawnActor<APawn>(NPCClass, SpawnPoint->Location, SpawnPoint->Rotation, SpawnParams);
            }
            
            if (SpawnedNPC)
            {
                SpawnPoint->bIsOccupied = true;
                SpawnPoint->SpawnedActor = SpawnedNPC;
                SpawnedActors.Add(SpawnedNPC);
                
                SetupSpawnedActor(SpawnedNPC, TEXT("NPC"), &NPCData);
                OnContentSpawned.Broadcast(SpawnedNPC);
                
                if (bDebugMode)
                {
                    UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Spawned NPC %s"), *NPCData.Name);
                }
            }
        }
    }
}

void UAIDirectorComponent::SpawnEnemiesForLayout()
{
    // Get enemies for current planet
    TArray<FCampaignEnemyData> Enemies = CampaignLoader->GetEnemiesForPlanet(CurrentPlanetIndex);
    
    // Limit number of enemies spawned (for performance)
    int32 MaxEnemies = FMath::Min(Enemies.Num(), 5);
    
    for (int32 i = 0; i < MaxEnemies; i++)
    {
        FAIDirectorSpawnData* SpawnPoint = FindAvailableSpawnPoint(TEXT("Enemy"), CurrentLayoutName);
        if (SpawnPoint && i < Enemies.Num())
        {
            const FCampaignEnemyData& EnemyData = Enemies[i];
            
            // Try blueprint implementation first
            APawn* SpawnedEnemy = OnSpawnEnemy(EnemyData, *SpawnPoint);
            
            if (!SpawnedEnemy && DefaultEnemyClass)
            {
                // Fallback to default spawning
                TSubclassOf<APawn> EnemyClass = GetEnemyClassForSpecies(EnemyData.Species);
                if (!EnemyClass)
                {
                    EnemyClass = DefaultEnemyClass;
                }
                
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                
                SpawnedEnemy = GetWorld()->SpawnActor<APawn>(EnemyClass, SpawnPoint->Location, SpawnPoint->Rotation, SpawnParams);
            }
            
            if (SpawnedEnemy)
            {
                SpawnPoint->bIsOccupied = true;
                SpawnPoint->SpawnedActor = SpawnedEnemy;
                SpawnedActors.Add(SpawnedEnemy);
                
                SetupSpawnedActor(SpawnedEnemy, TEXT("Enemy"), &EnemyData);
                OnContentSpawned.Broadcast(SpawnedEnemy);
                
                if (bDebugMode)
                {
                    UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Spawned Enemy %s"), *EnemyData.Name);
                }
            }
        }
    }
}

void UAIDirectorComponent::SpawnLootForLayout()
{
    // For now, spawn some basic loot at loot spawn points
    TArray<FAIDirectorSpawnData> LootSpawnPoints = GetSpawnPointsByType(TEXT("Loot"));

    for (FAIDirectorSpawnData& SpawnPoint : LootSpawnPoints)
    {
        if (!SpawnPoint.bIsOccupied)
        {
            // Create a basic loot item for testing
            FLootItem TestLoot;
            TestLoot.Name = TEXT("Test Loot");
            TestLoot.Rarity = TEXT("Common");
            TestLoot.ItemType = TEXT("Credits");

            // Try blueprint implementation first
            AActor* SpawnedLoot = OnSpawnLoot(TestLoot, SpawnPoint);

            if (!SpawnedLoot && DefaultLootClass)
            {
                // Fallback to default spawning
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                SpawnedLoot = GetWorld()->SpawnActor<AActor>(DefaultLootClass, SpawnPoint.Location, SpawnPoint.Rotation, SpawnParams);
            }

            if (SpawnedLoot)
            {
                SpawnPoint.bIsOccupied = true;
                SpawnPoint.SpawnedActor = SpawnedLoot;
                SpawnedActors.Add(SpawnedLoot);

                SetupSpawnedActor(SpawnedLoot, TEXT("Loot"), &TestLoot);
                OnContentSpawned.Broadcast(SpawnedLoot);

                if (bDebugMode)
                {
                    UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Spawned Loot %s"), *TestLoot.Name);
                }
            }
        }
    }
}

void UAIDirectorComponent::SpawnQuestsForLayout()
{
    // Quest spawning logic - for now just log available quests
    const FPlanetData Planet = GetCurrentPlanetData();

    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("AIDirectorComponent: Available quests for %s:"), *Planet.Name);
        UE_LOG(LogTemp, Log, TEXT("  Main Quest: %s"), *Planet.MainQuest);

        for (const FString& SideQuest : Planet.SideQuests)
        {
            UE_LOG(LogTemp, Log, TEXT("  Side Quest: %s"), *SideQuest);
        }
    }
}

TSubclassOf<APawn> UAIDirectorComponent::GetNPCClassForSpecies(const FString& Species) const
{
    if (const TSubclassOf<APawn>* FoundClass = NPCClassesBySpecies.Find(Species))
    {
        return *FoundClass;
    }

    return DefaultNPCClass;
}

TSubclassOf<APawn> UAIDirectorComponent::GetEnemyClassForSpecies(const FString& Species) const
{
    if (const TSubclassOf<APawn>* FoundClass = EnemyClassesBySpecies.Find(Species))
    {
        return *FoundClass;
    }

    return DefaultEnemyClass;
}

FAIDirectorSpawnData* UAIDirectorComponent::FindAvailableSpawnPoint(const FString& SpawnType, const FString& LayoutName)
{
    for (FAIDirectorSpawnData& SpawnPoint : RegisteredSpawnPoints)
    {
        if (SpawnPoint.SpawnType == SpawnType &&
            SpawnPoint.LayoutName == LayoutName &&
            !SpawnPoint.bIsOccupied)
        {
            return &SpawnPoint;
        }
    }

    return nullptr;
}

void UAIDirectorComponent::SetupSpawnedActor(AActor* SpawnedActor, const FString& ActorType, const void* Data)
{
    if (!SpawnedActor)
    {
        return;
    }

    // Set actor name for debugging
    if (ActorType == TEXT("NPC") && Data)
    {
        const FNPCData* NPCData = static_cast<const FNPCData*>(Data);
        SpawnedActor->SetActorLabel(FString::Printf(TEXT("NPC_%s"), *NPCData->Name));
    }
    else if (ActorType == TEXT("Enemy") && Data)
    {
        const FCampaignEnemyData* EnemyData = static_cast<const FCampaignEnemyData*>(Data);
        SpawnedActor->SetActorLabel(FString::Printf(TEXT("Enemy_%s"), *EnemyData->Name));
    }
    else if (ActorType == TEXT("Loot") && Data)
    {
        const FLootItem* LootData = static_cast<const FLootItem*>(Data);
        SpawnedActor->SetActorLabel(FString::Printf(TEXT("Loot_%s"), *LootData->Name));
    }

    // Additional setup can be added here (components, properties, etc.)
}

void UAIDirectorComponent::DrawSpawnPointDebug() const
{
    if (!GetWorld())
    {
        return;
    }

    for (const FAIDirectorSpawnData& SpawnPoint : RegisteredSpawnPoints)
    {
        if (SpawnPoint.LayoutName == CurrentLayoutName)
        {
            FColor DebugColor = FColor::Green;

            if (SpawnPoint.bIsOccupied)
            {
                DebugColor = FColor::Red;
            }
            else if (SpawnPoint.SpawnType == TEXT("Enemy"))
            {
                DebugColor = FColor::Orange;
            }
            else if (SpawnPoint.SpawnType == TEXT("Loot"))
            {
                DebugColor = FColor::Yellow;
            }
            else if (SpawnPoint.SpawnType == TEXT("Quest"))
            {
                DebugColor = FColor::Purple;
            }

            // Draw sphere at spawn point
            DrawDebugSphere(GetWorld(), SpawnPoint.Location, 50.0f, 12, DebugColor, false, -1.0f, 0, 2.0f);

            // Draw text label
            DrawDebugString(GetWorld(), SpawnPoint.Location + FVector(0, 0, 100),
                          FString::Printf(TEXT("%s\n%s"), *SpawnPoint.SpawnType, *SpawnPoint.LayoutName),
                          nullptr, DebugColor, 0.0f);
        }
    }
}

void UAIDirectorComponent::LogDirectorState() const
{
    UE_LOG(LogTemp, Log, TEXT("=== AI Director State ==="));
    UE_LOG(LogTemp, Log, TEXT("Initialized: %s"), bIsInitialized ? TEXT("Yes") : TEXT("No"));
    UE_LOG(LogTemp, Log, TEXT("Current Planet: %d"), CurrentPlanetIndex);
    UE_LOG(LogTemp, Log, TEXT("Current Layout: %s"), *CurrentLayoutName);
    UE_LOG(LogTemp, Log, TEXT("Registered Spawn Points: %d"), RegisteredSpawnPoints.Num());
    UE_LOG(LogTemp, Log, TEXT("Spawned Actors: %d"), SpawnedActors.Num());

    const FPlanetData Planet = GetCurrentPlanetData();
    UE_LOG(LogTemp, Log, TEXT("Planet Name: %s"), *Planet.Name);
    UE_LOG(LogTemp, Log, TEXT("Planet Biome: %s"), *Planet.Biome);
    UE_LOG(LogTemp, Log, TEXT("========================"));
}
