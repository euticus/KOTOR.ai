// Copyright Epic Games, Inc. All Rights Reserved.

#include "KOTORGameMode.h"
#include "AI/AIDirectorSubsystem.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Core/GameStartupSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

AKOTORGameMode::AKOTORGameMode()
{
	// Set default values
	bKOTORAISystemsInitialized = false;
	DefaultStartupMap = TEXT("TestLevel");
	bAutoInitializeKOTORAI = true;

	// Set default pawn and controller classes
	// These would be set to actual KOTOR.ai classes in a real implementation
	DefaultPawnClass = nullptr; // Will use engine default for now
	PlayerControllerClass = nullptr; // Will use engine default for now
}

void AKOTORGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("🎮 KOTOR.ai Game Mode: BeginPlay"));

	// Auto-initialize KOTOR.ai systems if enabled
	if (bAutoInitializeKOTORAI)
	{
		InitializeKOTORAISystems();
	}
}

void AKOTORGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogTemp, Warning, TEXT("🎮 KOTOR.ai Game Mode: InitGame - Map: %s"), *MapName);
}

void AKOTORGameMode::InitializeKOTORAISystems()
{
	if (bKOTORAISystemsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("🎮 KOTOR.ai systems already initialized"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("🚀 Initializing KOTOR.ai systems..."));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ No world available for KOTOR.ai initialization"));
		return;
	}

	// Initialize Game Startup Subsystem
	if (UGameStartupSubsystem* StartupSubsystem = World->GetSubsystem<UGameStartupSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ Game Startup Subsystem found"));
		StartupSubsystem->InitializeGameStartup();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Game Startup Subsystem not found"));
	}

	// Initialize AI Director Subsystem
	if (UAIDirectorSubsystem* AIDirector = World->GetSubsystem<UAIDirectorSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ AI Director Subsystem found"));
		// Initialize AI Director here
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ AI Director Subsystem not found"));
	}

	// Initialize Procedural Music Subsystem
	if (UProceduralMusicSubsystemV2* MusicSubsystem = World->GetSubsystem<UProceduralMusicSubsystemV2>())
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ Procedural Music Subsystem found"));
		// Initialize music system here
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Procedural Music Subsystem not found"));
	}

	bKOTORAISystemsInitialized = true;
	UE_LOG(LogTemp, Warning, TEXT("🎉 KOTOR.ai systems initialization complete!"));
}

UAIDirectorSubsystem* AKOTORGameMode::GetAIDirector() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UAIDirectorSubsystem>();
	}
	return nullptr;
}

UProceduralMusicSubsystemV2* AKOTORGameMode::GetMusicSubsystem() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UProceduralMusicSubsystemV2>();
	}
	return nullptr;
}
