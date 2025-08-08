// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "Narrative/NarrativeMemoryComponent.h"
#include "Character/MutationManagerComponent.h"
#include "MeditationSystem.generated.h"

/**
 * Vision types
 */
UENUM(BlueprintType)
enum class EForceVisionType : uint8
{
    Flashback           UMETA(DisplayName = "Flashback Memory"),
    Prophecy            UMETA(DisplayName = "Prophetic Vision"),
    Hallucination       UMETA(DisplayName = "Hallucination"),
    MoralDilemma        UMETA(DisplayName = "Moral Dilemma"),
    PastLife            UMETA(DisplayName = "Past Life Memory"),
    FutureEcho          UMETA(DisplayName = "Future Echo"),
    SpiritContact       UMETA(DisplayName = "Spirit Contact"),
    ForceEcho           UMETA(DisplayName = "Force Echo"),
    DarkVision          UMETA(DisplayName = "Dark Side Vision"),
    LightVision         UMETA(DisplayName = "Light Side Vision")
};

/**
 * Meditation state
 */
UENUM(BlueprintType)
enum class EMeditationState : uint8
{
    Inactive            UMETA(DisplayName = "Inactive"),
    Entering            UMETA(DisplayName = "Entering Meditation"),
    Meditating          UMETA(DisplayName = "Meditating"),
    VisionActive        UMETA(DisplayName = "Vision Active"),
    Exiting             UMETA(DisplayName = "Exiting Meditation")
};

/**
 * Force vision data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FForceVision
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString VisionID;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    EForceVisionType VisionType;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString Title;

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString Description; // LLM-generated vision content

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TArray<FString> DialogueLines; // Vision dialogue

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TArray<FString> Speakers; // Who speaks each line

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TArray<FString> VisualElements; // Visual descriptions

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TArray<FString> AudioElements; // Sound effects and music

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    float Duration; // Vision duration in seconds

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TMap<FString, FString> Choices; // Player choices in vision

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    TMap<FString, float> AlignmentEffects; // Choice -> alignment change

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    FString TriggerContext; // What triggered this vision

    UPROPERTY(BlueprintReadWrite, Category = "Force Vision")
    bool bIsInteractive; // Whether player can make choices

    FForceVision()
    {
        VisionID = TEXT("");
        VisionType = EForceVisionType::Flashback;
        Title = TEXT("Force Vision");
        Description = TEXT("");
        Duration = 30.0f;
        TriggerContext = TEXT("");
        bIsInteractive = false;
    }
};

/**
 * Meditation spot data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FMeditationSpotData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    FString SpotID;

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    FString SpotName;

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    FVector Location;

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    FString PlanetName;

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    TArray<EForceVisionType> PreferredVisionTypes; // Vision types this spot favors

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    float ForceResonance; // 0.0 to 1.0, higher = stronger visions

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    FString Alignment; // "light", "dark", "neutral"

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    bool bIsDiscovered; // Whether player has found this spot

    UPROPERTY(BlueprintReadWrite, Category = "Meditation Spot")
    int32 TimesUsed; // How many times player has meditated here

    FMeditationSpotData()
    {
        SpotID = TEXT("");
        SpotName = TEXT("Meditation Spot");
        Description = TEXT("");
        Location = FVector::ZeroVector;
        PlanetName = TEXT("");
        ForceResonance = 0.5f;
        Alignment = TEXT("neutral");
        bIsDiscovered = false;
        TimesUsed = 0;
    }
};

/**
 * Meditation events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeditationStarted, const FMeditationSpotData&, MeditationSpot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeditationEnded, const FMeditationSpotData&, MeditationSpot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnForceVisionTriggered, const FForceVision&, Vision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVisionChoiceMade, const FString&, ChoiceID, float, AlignmentChange);

/**
 * Meditation Spot Actor - Interactable meditation location
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AMeditationSpot : public AActor
{
    GENERATED_BODY()

public:
    AMeditationSpot();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Activate meditation spot
     * @param PlayerCharacter Player activating the spot
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Spot")
    void ActivateMeditationSpot(APawn* PlayerCharacter);

    /**
     * Get meditation spot data
     * @return Meditation spot data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation Spot")
    FMeditationSpotData GetMeditationSpotData() const { return SpotData; }

    /**
     * Set meditation spot data
     * @param NewSpotData New spot data
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Spot")
    void SetMeditationSpotData(const FMeditationSpotData& NewSpotData);

protected:
    // Meditation spot data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meditation Spot")
    FMeditationSpotData SpotData;

    // Visual components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* MeditationMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* InteractionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UParticleSystemComponent* ForceAura;

    // Interaction events
    UFUNCTION()
    void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                        bool bFromSweep, const FHitResult& SweepResult);

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when meditation spot is activated
     * @param PlayerCharacter Player activating the spot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Spot Events")
    void OnMeditationSpotActivated(APawn* PlayerCharacter);

    /**
     * Called to customize spot appearance based on alignment
     * @param Alignment Current alignment of the spot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Spot Events")
    void UpdateSpotAppearance(const FString& Alignment);
};

/**
 * Meditation System - Manages meditation spots and Force visions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UMeditationSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UMeditationSystem();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize meditation system
     * @param CampaignLoader Campaign loader for context
     * @param NarrativeMemory Narrative memory for vision content
     * @param MutationManager Mutation manager for corruption effects
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    void InitializeMeditationSystem(UCampaignLoaderSubsystem* CampaignLoader,
                                   UNarrativeMemoryComponent* NarrativeMemory,
                                   UMutationManagerComponent* MutationManager);

    /**
     * Start meditation session
     * @param MeditationSpot Meditation spot being used
     * @return True if meditation started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    bool StartMeditation(AMeditationSpot* MeditationSpot);

    /**
     * End meditation session
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    void EndMeditation();

    /**
     * Generate Force vision
     * @param VisionType Type of vision to generate
     * @param Context Context for vision generation
     * @param MeditationSpot Spot where meditation is occurring
     * @return Generated Force vision
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    FForceVision GenerateForceVision(EForceVisionType VisionType, const FString& Context, 
                                    const FMeditationSpotData& MeditationSpot);

    /**
     * Trigger Force vision
     * @param Vision Vision to trigger
     * @return True if vision started successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    bool TriggerForceVision(const FForceVision& Vision);

    /**
     * Make vision choice
     * @param ChoiceID ID of the choice made
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    void MakeVisionChoice(const FString& ChoiceID);

    /**
     * Get current meditation state
     * @return Current meditation state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation System")
    EMeditationState GetMeditationState() const { return CurrentMeditationState; }

    /**
     * Get current vision
     * @return Currently active vision
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation System")
    FForceVision GetCurrentVision() const { return CurrentVision; }

    /**
     * Get discovered meditation spots
     * @return Array of discovered meditation spots
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation System")
    TArray<FMeditationSpotData> GetDiscoveredMeditationSpots() const;

    /**
     * Register meditation spot
     * @param MeditationSpot Spot to register
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    void RegisterMeditationSpot(AMeditationSpot* MeditationSpot);

    /**
     * Set atmospheric camera mode
     * @param bEnabled Whether to enable atmospheric camera
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation System")
    void SetAtmosphericCameraMode(bool bEnabled);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Meditation Events")
    FOnMeditationStarted OnMeditationStarted;

    UPROPERTY(BlueprintAssignable, Category = "Meditation Events")
    FOnMeditationEnded OnMeditationEnded;

    UPROPERTY(BlueprintAssignable, Category = "Meditation Events")
    FOnForceVisionTriggered OnForceVisionTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Meditation Events")
    FOnVisionChoiceMade OnVisionChoiceMade;

protected:
    // Meditation state
    UPROPERTY(BlueprintReadOnly, Category = "Meditation System")
    EMeditationState CurrentMeditationState;

    UPROPERTY(BlueprintReadOnly, Category = "Meditation System")
    FForceVision CurrentVision;

    UPROPERTY(BlueprintReadOnly, Category = "Meditation System")
    AMeditationSpot* CurrentMeditationSpot;

    // Discovered spots
    UPROPERTY(BlueprintReadOnly, Category = "Meditation System")
    TArray<AMeditationSpot*> RegisteredMeditationSpots;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UNarrativeMemoryComponent* NarrativeMemoryRef;

    UPROPERTY()
    UMutationManagerComponent* MutationManagerRef;

    // Camera management
    UPROPERTY()
    ACameraActor* AtmosphericCamera;

    UPROPERTY()
    AActor* OriginalViewTarget;

    // Vision templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision Templates")
    /*  Flat list of vision prompt templates.
        (UPROPERTY does not support TMap<Key, TArray<Value>> serialization, so
        we store all prompts in a single array and filter by VisionType at
        runtime inside the meditation system.) */
    TArray<FString> VisionPromptTemplates;

    // Timer handles
    FTimerHandle VisionTimer;
    FTimerHandle MeditationTimer;

private:
    // Helper methods
    FString GenerateVisionID();
    FString BuildVisionPrompt(EForceVisionType VisionType, const FString& Context, const FMeditationSpotData& Spot);
    EForceVisionType SelectVisionType(const FMeditationSpotData& Spot);
    void SetupAtmosphericCamera();
    void RestoreOriginalCamera();
    void ProcessVisionDialogue();

    // Vision generation
    FForceVision GenerateFlashbackVision(const FString& Context);
    FForceVision GeneratePropheticVision(const FString& Context);
    FForceVision GenerateMoralDilemmaVision(const FString& Context);

public:
    /**
     * Blueprint implementable events for custom meditation logic
     */

    /**
     * Called when meditation starts (for custom setup)
     * @param MeditationSpot The meditation spot being used
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Events")
    void OnMeditationStartedEvent(const FMeditationSpotData& MeditationSpot);

    /**
     * Called when Force vision is triggered (for custom effects)
     * @param Vision The triggered vision
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Events")
    void OnForceVisionTriggeredEvent(const FForceVision& Vision);

    /**
     * Called to generate custom Force vision (override in Blueprint)
     * @param VisionType Type of vision to generate
     * @param Context Context for the vision
     * @param MeditationSpot Spot where meditation occurs
     * @return Custom generated vision
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Events")
    FForceVision GenerateCustomForceVision(EForceVisionType VisionType, const FString& Context, 
                                          const FMeditationSpotData& MeditationSpot);

    /**
     * Called to setup custom atmospheric effects (override in Blueprint)
     * @param MeditationSpot The meditation spot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Events")
    void SetupCustomAtmosphericEffects(const FMeditationSpotData& MeditationSpot);
};
