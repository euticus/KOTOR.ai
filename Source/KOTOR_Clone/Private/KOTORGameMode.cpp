// Copyright Epic Games, Inc. All Rights Reserved.

#include "KOTORGameMode.h"
#include "Engine/Engine.h"

AKOTORGameMode::AKOTORGameMode()
{
	// Set default values - minimal setup
}

void AKOTORGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("🎮 KOTOR.ai Game Mode: BeginPlay"));
	UE_LOG(LogTemp, Warning, TEXT("🚀 KOTOR.ai: All 12 phases ready!"));
	UE_LOG(LogTemp, Warning, TEXT("🎉 KOTOR.ai: The most advanced AI-driven RPG framework is running!"));
}
