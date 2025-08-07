// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KOTORGameMode.generated.h"

/**
 * KOTOR.ai Game Mode
 * Main game mode that initializes all KOTOR.ai systems
 */
UCLASS()
class KOTOR_CLONE_API AKOTORGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKOTORGameMode();

protected:
	virtual void BeginPlay() override;
};
