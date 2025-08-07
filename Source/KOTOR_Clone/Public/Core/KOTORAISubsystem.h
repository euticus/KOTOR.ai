// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KOTORAISubsystem.generated.h"

/**
 * KOTOR.ai Main Subsystem
 * Manages all 12 phases of the KOTOR.ai framework
 */
UCLASS()
class KOTOR_CLONE_API UKOTORAISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Initialize all KOTOR.ai systems */
	UFUNCTION(BlueprintCallable, Category = "KOTOR.ai")
	void InitializeKOTORAISystems();

	/** Get initialization status */
	UFUNCTION(BlueprintCallable, Category = "KOTOR.ai")
	bool IsKOTORAIInitialized() const { return bIsInitialized; }

	/** Get current phase status */
	UFUNCTION(BlueprintCallable, Category = "KOTOR.ai")
	int32 GetActivePhases() const { return ActivePhases; }

protected:
	/** Whether KOTOR.ai is fully initialized */
	UPROPERTY(BlueprintReadOnly, Category = "KOTOR.ai")
	bool bIsInitialized;

	/** Number of active phases */
	UPROPERTY(BlueprintReadOnly, Category = "KOTOR.ai")
	int32 ActivePhases;

private:
	void InitializePhase1_PythonAIDM();
	void InitializePhase2_UE5Integration();
	void InitializePhase3_ImmersionIntelligence();
	void InitializePhase4_MultiplayerDnD();
	void InitializePhase5_EmotionalIntelligence();
	void InitializePhase6_WorldSimulation();
	void InitializePhase7_UnrealStreamlining();
	void InitializePhase8_OptimizationPipeline();
	void InitializePhase9_CinematicMultiplayer();
	void InitializePhase10_DynamicAnimationMusic();
	void InitializePhase11_AIAnimationProceduralMusic();
	void InitializePhase12_BuildLaunchStreamlining();
};
