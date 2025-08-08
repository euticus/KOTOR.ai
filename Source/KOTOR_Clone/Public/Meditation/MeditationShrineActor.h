// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/TriggerVolume.h"
#include "Meditation/MeditationSystem.h"
#include "Visions/AlternateRealitySimulator.h"
#include "MeditationShrineActor.generated.h"

/**
 * Shrine activation states
 */
UENUM(BlueprintType)
enum class EShrineActivationState : uint8
{
    Inactive            UMETA(DisplayName = "Inactive"),
    Discovered          UMETA(DisplayName = "Discovered"),
    Activating          UMETA(DisplayName = "Activating"),
    Active              UMETA(DisplayName = "Active"),
    InUse               UMETA(DisplayName = "In Use"),
    Cooldown            UMETA(DisplayName = "Cooldown"),
    Depleted            UMETA(DisplayName = "Depleted")
};

/**
 * Shrine interaction data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FShrineInteractionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    FString InteractionID;

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    FString InteractionType; // "meditation", "vision", "what_if"

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    FString InteractionName;

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    TArray<FString> Requirements; // Requirements to use this interaction

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    int32 UseCost; // Force points or other cost

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    float CooldownTime; // Time before can be used again

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    bool bIsAvailable;

    UPROPERTY(BlueprintReadWrite, Category = "Shrine Interaction")
    float LastUsedTime;

    FShrineInteractionData()
    {
        InteractionID = TEXT("");
        InteractionType = TEXT("meditation");
        InteractionName = TEXT("Meditate");
        Description = TEXT("Enter a meditative state");
        UseCost = 0;
        CooldownTime = 0.0f;
        bIsAvailable = true;
        LastUsedTime = 0.0f;
    }
};

/**
 * Shrine events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShrineActivated, AMeditationShrineActor*, Shrine, APawn*, Activator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShrineDeactivated, AMeditationShrineActor*, Shrine, APawn*, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShrineInteractionUsed, AMeditationShrineActor*, Shrine, const FString&, InteractionType, APawn*, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShrineDiscovered, AMeditationShrineActor*, Shrine);

/**
 * Meditation Trigger Volume - Trigger volume for meditation areas
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AMeditationTriggerVolume : public ATriggerVolume
{
    GENERATED_BODY()

public:
    AMeditationTriggerVolume();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Set meditation data
     * @param MeditationData Data for this meditation area
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Trigger")
    void SetMeditationData(const FMeditationSpotData& MeditationData);

    /**
     * Get meditation data
     * @return Current meditation data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation Trigger")
    FMeditationSpotData GetMeditationData() const { return MeditationData; }

    /**
     * Check if meditation is available
     * @param User Actor attempting to meditate
     * @return True if meditation is available
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation Trigger")
    bool IsMeditationAvailable(APawn* User) const;

protected:
    // Meditation data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meditation Trigger")
    FMeditationSpotData MeditationData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meditation Trigger")
    bool bAutoTrigger; // Whether to auto-trigger meditation on enter

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meditation Trigger")
    float TriggerDelay; // Delay before auto-trigger

    // Timer handle
    FTimerHandle TriggerTimer;

    // Overlap events
    UFUNCTION()
    void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

    // Timer callback
    UFUNCTION()
    void OnTriggerTimer();

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when meditation trigger is entered
     * @param EnteringActor Actor entering the trigger
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Trigger Events")
    void OnMeditationTriggerEntered(APawn* EnteringActor);

    /**
     * Called when meditation trigger is exited
     * @param ExitingActor Actor exiting the trigger
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Meditation Trigger Events")
    void OnMeditationTriggerExited(APawn* ExitingActor);
};

/**
 * Meditation Shrine Actor - Interactive shrine for meditation and visions
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AMeditationShrineActor : public AActor
{
    GENERATED_BODY()

public:
    AMeditationShrineActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Initialize shrine
     * @param ShrineData Meditation spot data
     * @param VisionShrineData Vision shrine data (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    void InitializeShrine(const FMeditationSpotData& TargetShrineData, const FString& VisionShrineType = TEXT(""));

    /**
     * Activate shrine
     * @param Activator Actor activating the shrine
     * @return True if shrine was activated successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    bool ActivateShrine(APawn* Activator);

    /**
     * Deactivate shrine
     * @param User Actor deactivating the shrine
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    void DeactivateShrine(APawn* User = nullptr);

    /**
     * Use shrine interaction
     * @param InteractionType Type of interaction to use
     * @param User Actor using the interaction
     * @return True if interaction was used successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    bool UseShrineInteraction(const FString& InteractionType, APawn* User);

    /**
     * Get available interactions
     * @param User Actor to check interactions for
     * @return Array of available interactions
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    TArray<FShrineInteractionData> GetAvailableInteractions(APawn* User) const;

    /**
     * Add shrine interaction
     * @param InteractionData Interaction to add
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    void AddShrineInteraction(const FShrineInteractionData& InteractionData);

    /**
     * Remove shrine interaction
     * @param InteractionID ID of interaction to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    void RemoveShrineInteraction(const FString& InteractionID);

    /**
     * Get shrine activation state
     * @return Current activation state
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation Shrine")
    EShrineActivationState GetActivationState() const { return ActivationState; }

    /**
     * Check if shrine can be activated
     * @param Activator Actor attempting to activate
     * @return True if shrine can be activated
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation Shrine")
    bool CanActivateShrine(APawn* Activator) const;

    /**
     * Get shrine data
     * @return Current shrine data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Meditation Shrine")
    FMeditationSpotData GetShrineData() const { return ShrineData; }

    /**
     * Set shrine discovered
     * @param bDiscovered Whether shrine has been discovered
     */
    UFUNCTION(BlueprintCallable, Category = "Meditation Shrine")
    void SetShrineDiscovered(bool bDiscovered);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Shrine Events")
    FOnShrineActivated OnShrineActivated;

    UPROPERTY(BlueprintAssignable, Category = "Shrine Events")
    FOnShrineDeactivated OnShrineDeactivated;

    UPROPERTY(BlueprintAssignable, Category = "Shrine Events")
    FOnShrineInteractionUsed OnShrineInteractionUsed;

    UPROPERTY(BlueprintAssignable, Category = "Shrine Events")
    FOnShrineDiscovered OnShrineDiscovered;

protected:
    // Shrine data
    UPROPERTY(BlueprintReadOnly, Category = "Shrine Data")
    FMeditationSpotData ShrineData;

    UPROPERTY(BlueprintReadOnly, Category = "Shrine Data")
    TArray<FShrineInteractionData> AvailableInteractions;

    UPROPERTY(BlueprintReadOnly, Category = "Shrine Data")
    EShrineActivationState ActivationState;

    UPROPERTY(BlueprintReadOnly, Category = "Shrine Data")
    APawn* CurrentUser;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ShrineMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* InteractionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* ShrineInfoWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UParticleSystemComponent* ShrineAura;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAudioComponent* ShrineAmbientAudio;

    // System references
    UPROPERTY()
    UMeditationSystem* MeditationSystem;

    UPROPERTY()
    UAlternateRealitySimulator* VisionSystem;

    // Shrine settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shrine Settings")
    bool bIsDiscovered;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shrine Settings")
    bool bRequiresForceUser; // Whether only Force users can activate

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shrine Settings")
    float ActivationRange; // Range for activation

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shrine Settings")
    float CooldownDuration; // Global cooldown for shrine

    // Timer handles
    FTimerHandle CooldownTimer;
    FTimerHandle ActivationTimer;

private:
    // Helper methods
    void SetupDefaultInteractions();
    void UpdateShrineAppearance();
    void UpdateShrineAura();
    bool CheckInteractionRequirements(const FShrineInteractionData& Interaction, APawn* User) const;
    void StartCooldown();

    // Interaction events
    UFUNCTION()
    void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // Timer callbacks
    UFUNCTION()
    void OnCooldownComplete();

    UFUNCTION()
    void OnActivationComplete();

public:
    /**
     * Blueprint implementable events for custom shrine logic
     */

    /**
     * Called when shrine is activated (for custom setup)
     * @param Activator Actor that activated the shrine
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shrine Events")
    void OnShrineActivatedEvent(APawn* Activator);

    /**
     * Called when shrine interaction is used (for custom effects)
     * @param InteractionType Type of interaction used
     * @param User Actor using the interaction
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shrine Events")
    void OnShrineInteractionUsedEvent(const FString& InteractionType, APawn* User);

    /**
     * Called to customize shrine appearance (override in Blueprint)
     * @param ShrineData The shrine data for customization
     * @param ActivationState Current activation state
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shrine Events")
    void CustomizeShrineAppearance(const FMeditationSpotData& TargetShrineData, EShrineActivationState InputActivationState);

    /**
     * Called to handle custom shrine interaction (override in Blueprint)
     * @param InteractionType Type of interaction
     * @param User Actor using the interaction
     * @return True if interaction was handled
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shrine Events")
    bool HandleCustomShrineInteraction(const FString& InteractionType, APawn* User);

    /**
     * Called when shrine is discovered (for custom discovery effects)
     * @param Discoverer Actor that discovered the shrine
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shrine Events")
    void OnShrineDiscoveredEvent(APawn* Discoverer);
};
