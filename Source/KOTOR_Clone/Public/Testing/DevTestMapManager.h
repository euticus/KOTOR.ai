// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "NPCs/ProceduralNPCActor.h"
#include "Enemies/EnemyActor.h"
#include "Loot/LootPickupSystem.h"
#include "Meditation/MeditationShrineActor.h"
#include "DevTestMapManager.generated.h"

/**
 * Test zone types
 */
UENUM(BlueprintType)
enum class ETestZoneType : uint8
{
    Dialogue            UMETA(DisplayName = "Dialogue Test"),
    Combat              UMETA(DisplayName = "Combat Test"),
    Loot                UMETA(DisplayName = "Loot Test"),
    Meditation          UMETA(DisplayName = "Meditation Test"),
    GalaxyMap           UMETA(DisplayName = "Galaxy Map Test"),
    PhotoMode           UMETA(DisplayName = "Photo Mode Test"),
    NPCInteraction      UMETA(DisplayName = "NPC Interaction Test"),
    QuestSystem         UMETA(DisplayName = "Quest System Test"),
    ForceVision         UMETA(DisplayName = "Force Vision Test"),
    SystemIntegration   UMETA(DisplayName = "System Integration Test")
};

/**
 * Test zone data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FTestZoneData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    ETestZoneType ZoneType;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    FString ZoneName;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    FVector Location;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    FVector Extent;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    TArray<FString> TestInstructions;

    UPROPERTY(BlueprintReadWrite, Category = "Test Zone")
    TMap<FString, FString> TestParameters;

    FTestZoneData()
    {
        ZoneType = ETestZoneType::Dialogue;
        ZoneName = TEXT("Test Zone");
        Description = TEXT("A test zone for development");
        Location = FVector::ZeroVector;
        Extent = FVector(500.0f, 500.0f, 200.0f);
        bIsActive = true;
    }
};

/**
 * Test zone events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTestZoneEntered, ETestZoneType, ZoneType, APawn*, EnteringPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTestZoneExited, ETestZoneType, ZoneType, APawn*, ExitingPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTestExecuted, ETestZoneType, ZoneType, const FString&, TestName, bool, bSuccess);

/**
 * Test Zone Actor - Individual test zone in the dev map
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ATestZoneActor : public AActor
{
    GENERATED_BODY()

public:
    ATestZoneActor();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize test zone
     * @param ZoneData Data for this test zone
     */
    UFUNCTION(BlueprintCallable, Category = "Test Zone")
    void InitializeTestZone(const FTestZoneData& ZoneData);

    /**
     * Execute test
     * @param TestName Name of test to execute
     * @param TestPawn Pawn executing the test
     * @return True if test was executed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Test Zone")
    bool ExecuteTest(const FString& TestName, APawn* TestPawn);

    /**
     * Get zone data
     * @return Current zone data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Zone")
    FTestZoneData GetZoneData() const { return ZoneData; }

    /**
     * Set zone active
     * @param bActive Whether zone should be active
     */
    UFUNCTION(BlueprintCallable, Category = "Test Zone")
    void SetZoneActive(bool bActive);

    // Event delegate
    UPROPERTY(BlueprintAssignable, Category = "Test Zone Events")
    FOnTestExecuted OnTestExecuted;

protected:
    // Zone data
    UPROPERTY(BlueprintReadOnly, Category = "Zone Data")
    FTestZoneData ZoneData;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* ZoneTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ZoneMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* ZoneLabel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* InstructionsText;

    // Test actors
    UPROPERTY(BlueprintReadOnly, Category = "Test Actors")
    TArray<AActor*> SpawnedTestActors;

private:
    // Helper methods
    void SetupZoneAppearance();
    void SpawnTestActors();
    void CleanupTestActors();

    // Zone-specific test implementations
    bool ExecuteDialogueTest(APawn* TestPawn);
    bool ExecuteCombatTest(APawn* TestPawn);
    bool ExecuteLootTest(APawn* TestPawn);
    bool ExecuteMeditationTest(APawn* TestPawn);
    bool ExecuteGalaxyMapTest(APawn* TestPawn);
    bool ExecutePhotoModeTest(APawn* TestPawn);
    bool ExecuteNPCInteractionTest(APawn* TestPawn);
    bool ExecuteQuestSystemTest(APawn* TestPawn);
    bool ExecuteForceVisionTest(APawn* TestPawn);
    bool ExecuteSystemIntegrationTest(APawn* TestPawn);

    // Overlap events
    UFUNCTION()
    void OnZoneTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                  bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnZoneTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when test zone is entered
     * @param EnteringPawn Pawn entering the zone
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Test Zone Events")
    void OnTestZoneEnteredEvent(APawn* EnteringPawn);

    /**
     * Called when test is executed
     * @param TestName Name of executed test
     * @param TestPawn Pawn executing the test
     * @param bSuccess Whether test was successful
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Test Zone Events")
    void OnTestExecutedEvent(const FString& TestName, APawn* TestPawn, bool bSuccess);

    /**
     * Called to execute custom test (override in Blueprint)
     * @param TestName Name of test to execute
     * @param TestPawn Pawn executing the test
     * @return True if test was handled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Test Zone Events")
    bool ExecuteCustomTest(const FString& TestName, APawn* TestPawn);
};

/**
 * Dev Test Map Manager - Manages the developer testing playground
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ADevTestMapManager : public AActor
{
    GENERATED_BODY()

public:
    ADevTestMapManager();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize test map
     */
    UFUNCTION(BlueprintCallable, Category = "Dev Test Map")
    void InitializeTestMap();

    /**
     * Create test zone
     * @param ZoneData Data for the test zone
     * @return Created test zone actor
     */
    UFUNCTION(BlueprintCallable, Category = "Dev Test Map")
    ATestZoneActor* CreateTestZone(const FTestZoneData& ZoneData);

    /**
     * Get test zone by type
     * @param ZoneType Type of zone to find
     * @return Test zone actor of specified type
     */
    UFUNCTION(BlueprintCallable, Category = "Dev Test Map")
    ATestZoneActor* GetTestZoneByType(ETestZoneType ZoneType);

    /**
     * Execute all tests
     * @param TestPawn Pawn to execute tests with
     * @return Number of successful tests
     */
    UFUNCTION(BlueprintCallable, Category = "Dev Test Map")
    int32 ExecuteAllTests(APawn* TestPawn);

    /**
     * Reset all test zones
     */
    UFUNCTION(BlueprintCallable, Category = "Dev Test Map")
    void ResetAllTestZones();

    /**
     * Get test results
     * @return Map of test results
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dev Test Map")
    TMap<FString, bool> GetTestResults() const { return TestResults; }

    /**
     * Enable auto-testing
     * @param bEnabled Whether to enable auto-testing
     * @param TestInterval Interval between tests
     */
    UFUNCTION(BlueprintCallable, Category = "Dev Test Map")
    void EnableAutoTesting(bool bEnabled, float TestInterval = 5.0f);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Test Map Events")
    FOnTestZoneEntered OnTestZoneEntered;

    UPROPERTY(BlueprintAssignable, Category = "Test Map Events")
    FOnTestZoneExited OnTestZoneExited;

    UPROPERTY(BlueprintAssignable, Category = "Test Map Events")
    FOnTestExecuted OnTestExecuted;

protected:
    // Test zones
    UPROPERTY(BlueprintReadOnly, Category = "Test Zones")
    TArray<ATestZoneActor*> TestZones;

    // Test results
    UPROPERTY(BlueprintReadOnly, Category = "Test Results")
    TMap<FString, bool> TestResults;

    // Auto-testing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Testing")
    bool bAutoTestingEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Testing")
    float AutoTestInterval;

    // Test zone class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Zone Settings")
    TSubclassOf<ATestZoneActor> TestZoneClass;

    // Timer handle
    FTimerHandle AutoTestTimer;

private:
    // Helper methods
    void CreateDefaultTestZones();
    FTestZoneData CreateTestZoneData(ETestZoneType ZoneType, const FVector& Location);
    void LayoutTestZones();

    // Auto-test timer callback
    UFUNCTION()
    void OnAutoTestTimer();

    // Event handlers
    UFUNCTION()
    void OnTestZoneEnteredHandler(ETestZoneType ZoneType, APawn* EnteringPawn);

    UFUNCTION()
    void OnTestZoneExitedHandler(ETestZoneType ZoneType, APawn* ExitingPawn);

    UFUNCTION()
    void OnTestExecutedHandler(ETestZoneType ZoneType, const FString& TestName, bool bSuccess);

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when test map is initialized
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Test Map Events")
    void OnTestMapInitializedEvent();

    /**
     * Called when all tests complete
     * @param SuccessfulTests Number of successful tests
     * @param TotalTests Total number of tests
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Test Map Events")
    void OnAllTestsCompletedEvent(int32 SuccessfulTests, int32 TotalTests);

    /**
     * Called to create custom test zone (override in Blueprint)
     * @param ZoneType Type of zone to create
     * @param Location Location for the zone
     * @return Custom test zone data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Test Map Events")
    FTestZoneData CreateCustomTestZone(ETestZoneType ZoneType, const FVector& Location);
};
