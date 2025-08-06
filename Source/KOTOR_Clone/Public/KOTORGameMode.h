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
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	/** Initialize KOTOR.ai systems */
	UFUNCTION(BlueprintCallable, Category = "KOTOR.ai")
	void InitializeKOTORAISystems();

	/** Get AI Director Subsystem */
	UFUNCTION(BlueprintCallable, Category = "KOTOR.ai")
	class UAIDirectorSubsystem* GetAIDirector() const;

	/** Get Procedural Music Subsystem */
	UFUNCTION(BlueprintCallable, Category = "KOTOR.ai")
	class UProceduralMusicSubsystemV2* GetMusicSubsystem() const;

protected:
	/** Whether KOTOR.ai systems have been initialized */
	UPROPERTY(BlueprintReadOnly, Category = "KOTOR.ai")
	bool bKOTORAISystemsInitialized;

	/** Default startup map for auto-loading */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KOTOR.ai")
	FString DefaultStartupMap;

	/** Enable auto-initialization of KOTOR.ai systems */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KOTOR.ai")
	bool bAutoInitializeKOTORAI;
};
