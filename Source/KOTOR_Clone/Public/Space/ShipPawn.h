// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "ShipPawn.generated.h"

/**
 * Ship types
 */
UENUM(BlueprintType)
enum class EShipType : uint8
{
    Fighter         UMETA(DisplayName = "Fighter"),
    Freighter       UMETA(DisplayName = "Freighter"),
    Corvette        UMETA(DisplayName = "Corvette"),
    Cruiser         UMETA(DisplayName = "Cruiser"),
    Battleship      UMETA(DisplayName = "Battleship"),
    Custom          UMETA(DisplayName = "Custom")
};

/**
 * Ship faction
 */
UENUM(BlueprintType)
enum class EShipFaction : uint8
{
    Republic        UMETA(DisplayName = "Republic"),
    Sith            UMETA(DisplayName = "Sith Empire"),
    Neutral         UMETA(DisplayName = "Neutral"),
    Pirate          UMETA(DisplayName = "Pirates"),
    Mercenary       UMETA(DisplayName = "Mercenaries"),
    Unknown         UMETA(DisplayName = "Unknown")
};

/**
 * Ship weapon data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FShipWeapon
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    FString WeaponName;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    float Damage;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    float FireRate; // Shots per second

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    float Range;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    float EnergyConsumption;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    FString WeaponType; // "laser", "ion", "missile", "torpedo"

    UPROPERTY(BlueprintReadWrite, Category = "Ship Weapon")
    bool bIsActive;

    FShipWeapon()
    {
        WeaponName = TEXT("Basic Laser");
        Damage = 10.0f;
        FireRate = 2.0f;
        Range = 1000.0f;
        EnergyConsumption = 5.0f;
        WeaponType = TEXT("laser");
        bIsActive = true;
    }
};

/**
 * Ship upgrade data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FShipUpgrade
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    FString UpgradeName;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    FString UpgradeType; // "hull", "shields", "engines", "weapons", "crew"

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    TMap<FString, float> StatModifiers; // Stat name -> modifier value

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    int32 Cost;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    FString Rarity; // "common", "uncommon", "rare", "legendary"

    UPROPERTY(BlueprintReadWrite, Category = "Ship Upgrade")
    bool bIsInstalled;

    FShipUpgrade()
    {
        UpgradeName = TEXT("Basic Upgrade");
        UpgradeType = TEXT("hull");
        Description = TEXT("A basic ship upgrade");
        Cost = 100;
        Rarity = TEXT("common");
        bIsInstalled = false;
    }
};

/**
 * Ship stats
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FShipStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float MaxHullPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float CurrentHullPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float MaxShieldPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float CurrentShieldPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float ShieldRegenRate;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float MaxEnergy;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float CurrentEnergy;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float EnergyRegenRate;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float MaxSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float Acceleration;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    float TurnRate;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    int32 CrewCapacity;

    UPROPERTY(BlueprintReadWrite, Category = "Ship Stats")
    int32 CargoCapacity;

    FShipStats()
    {
        MaxHullPoints = 100.0f;
        CurrentHullPoints = 100.0f;
        MaxShieldPoints = 50.0f;
        CurrentShieldPoints = 50.0f;
        ShieldRegenRate = 5.0f;
        MaxEnergy = 100.0f;
        CurrentEnergy = 100.0f;
        EnergyRegenRate = 10.0f;
        MaxSpeed = 1000.0f;
        Acceleration = 500.0f;
        TurnRate = 90.0f;
        CrewCapacity = 4;
        CargoCapacity = 100;
    }
};

/**
 * Ship events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShipDamaged, float, DamageAmount, const FString&, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShipUpgradeInstalled, const FShipUpgrade&, Upgrade);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFired, const FShipWeapon&, Weapon);

/**
 * Ship Pawn - Player and AI controlled spacecraft
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AShipPawn : public APawn
{
    GENERATED_BODY()

public:
    AShipPawn();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
    /**
     * Initialize ship with data
     * @param ShipData Ship configuration data
     */
    UFUNCTION(BlueprintCallable, Category = "Ship")
    void InitializeShip(const FString& ShipData);

    /**
     * Fire primary weapons
     * @param TargetLocation Location to fire at
     */
    UFUNCTION(BlueprintCallable, Category = "Ship Combat")
    void FirePrimaryWeapons(const FVector& TargetLocation);

    /**
     * Fire secondary weapons
     * @param Target Target actor to fire at
     */
    UFUNCTION(BlueprintCallable, Category = "Ship Combat")
    void FireSecondaryWeapons(AActor* Target);

    /**
     * Take damage
     * @param DamageAmount Amount of damage to take
     * @param DamageType Type of damage
     * @param DamageSource Source of the damage
     */
    UFUNCTION(BlueprintCallable, Category = "Ship Combat")
    void TakeDamage(float DamageAmount, const FString& DamageType, AActor* DamageSource);

    /**
     * Repair hull
     * @param RepairAmount Amount to repair
     */
    UFUNCTION(BlueprintCallable, Category = "Ship")
    void RepairHull(float RepairAmount);

    /**
     * Recharge shields
     * @param RechargeAmount Amount to recharge
     */
    UFUNCTION(BlueprintCallable, Category = "Ship")
    void RechargeShields(float RechargeAmount);

    /**
     * Install ship upgrade
     * @param Upgrade Upgrade to install
     * @return True if upgrade was installed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Ship")
    bool InstallUpgrade(const FShipUpgrade& Upgrade);

    /**
     * Remove ship upgrade
     * @param UpgradeName Name of upgrade to remove
     * @return True if upgrade was removed
     */
    UFUNCTION(BlueprintCallable, Category = "Ship")
    bool RemoveUpgrade(const FString& UpgradeName);

    /**
     * Get ship stats
     * @return Current ship stats
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ship")
    FShipStats GetShipStats() const { return ShipStats; }

    /**
     * Get installed upgrades
     * @return Array of installed upgrades
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ship")
    TArray<FShipUpgrade> GetInstalledUpgrades() const;

    /**
     * Get ship weapons
     * @return Array of ship weapons
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ship")
    TArray<FShipWeapon> GetShipWeapons() const { return ShipWeapons; }

    /**
     * Check if ship is destroyed
     * @return True if ship is destroyed
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ship")
    bool IsDestroyed() const { return bIsDestroyed; }

    /**
     * Get ship type
     * @return Ship type
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ship")
    EShipType GetShipType() const { return ShipType; }

    /**
     * Get ship faction
     * @return Ship faction
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ship")
    EShipFaction GetShipFaction() const { return ShipFaction; }

    /**
     * Set AI target
     * @param Target Target to attack
     */
    UFUNCTION(BlueprintCallable, Category = "Ship AI")
    void SetAITarget(AActor* Target);

    /**
     * Enable/disable AI control
     * @param bEnable Whether to enable AI
     */
    UFUNCTION(BlueprintCallable, Category = "Ship AI")
    void SetAIEnabled(bool bEnable);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Ship Events")
    FOnShipDamaged OnShipDamaged;

    UPROPERTY(BlueprintAssignable, Category = "Ship Events")
    FOnShipDestroyed OnShipDestroyed;

    UPROPERTY(BlueprintAssignable, Category = "Ship Events")
    FOnShipUpgradeInstalled OnShipUpgradeInstalled;

    UPROPERTY(BlueprintAssignable, Category = "Ship Events")
    FOnWeaponFired OnWeaponFired;

protected:
    // Ship components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ShipMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UFloatingPawnMovement* MovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UParticleSystemComponent* EngineEffects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAudioComponent* EngineAudio;

    // Ship data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
    EShipType ShipType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
    EShipFaction ShipFaction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
    FString ShipName;

    UPROPERTY(BlueprintReadOnly, Category = "Ship")
    FShipStats ShipStats;

    UPROPERTY(BlueprintReadOnly, Category = "Ship")
    TArray<FShipWeapon> ShipWeapons;

    UPROPERTY(BlueprintReadOnly, Category = "Ship")
    TArray<FShipUpgrade> InstalledUpgrades;

    // Combat state
    UPROPERTY(BlueprintReadOnly, Category = "Ship Combat")
    bool bIsDestroyed;

    UPROPERTY(BlueprintReadOnly, Category = "Ship Combat")
    float LastFireTime;

    // AI
    UPROPERTY(BlueprintReadOnly, Category = "Ship AI")
    bool bAIEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Ship AI")
    AActor* AITarget;

    UPROPERTY(BlueprintReadOnly, Category = "Ship AI")
    float AIUpdateInterval;

    UPROPERTY(BlueprintReadOnly, Category = "Ship AI")
    float LastAIUpdate;

    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UParticleSystem* WeaponFireEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UParticleSystem* ExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    USoundBase* WeaponFireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    USoundBase* ExplosionSound;

private:
    // Input handling
    void MoveForward(float Value);
    void MoveRight(float Value);
    void MoveUp(float Value);
    void TurnRight(float Value);
    void TurnUp(float Value);
    void FirePrimary();
    void FireSecondary();

    // Helper methods
    void UpdateShipSystems(float DeltaTime);
    void UpdateAI(float DeltaTime);
    void ApplyUpgradeModifiers();
    void SpawnWeaponEffect(const FVector& Location, const FRotator& Rotation);
    void DestroyShip();

    // Timer handles
    FTimerHandle ShieldRegenTimer;
    FTimerHandle EnergyRegenTimer;

public:
    /**
     * Blueprint implementable events for custom ship logic
     */

    /**
     * Called when ship takes damage (for custom effects)
     * @param DamageAmount Amount of damage taken
     * @param DamageType Type of damage
     * @param DamageSource Source of damage
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Ship Events")
    void OnShipDamagedEvent(float DamageAmount, const FString& DamageType, AActor* DamageSource);

    /**
     * Called when ship is destroyed (for custom death sequence)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Ship Events")
    void OnShipDestroyedEvent();

    /**
     * Called to update AI behavior (override in Blueprint)
     * @param DeltaTime Time since last update
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Ship Events")
    void UpdateCustomAI(float DeltaTime);

    /**
     * Called when weapon is fired (for custom effects)
     * @param Weapon The weapon that was fired
     * @param FireLocation Location where weapon was fired from
     * @param TargetLocation Location weapon was aimed at
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Ship Events")
    void OnWeaponFiredEvent(const FShipWeapon& Weapon, const FVector& FireLocation, const FVector& TargetLocation);
};
