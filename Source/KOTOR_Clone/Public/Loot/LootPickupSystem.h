// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "AIDM/LootGeneratorComponent.h"
#include "LootPickupSystem.generated.h"

/**
 * Loot rarity types
 */
UENUM(BlueprintType)
enum class ELootRarity : uint8
{
    Common              UMETA(DisplayName = "Common"),
    Uncommon            UMETA(DisplayName = "Uncommon"),
    Rare                UMETA(DisplayName = "Rare"),
    Epic                UMETA(DisplayName = "Epic"),
    Legendary           UMETA(DisplayName = "Legendary"),
    Artifact            UMETA(DisplayName = "Artifact")
};

/**
 * Loot item data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FLootItemData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    FString ItemID;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    FString ItemName;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    FString ItemType; // "weapon", "armor", "consumable", "misc"

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    ELootRarity Rarity;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    int32 Quantity;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    int32 Value; // Credits value

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    UTexture2D* ItemIcon;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    UStaticMesh* ItemMesh;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    FLinearColor RarityColor;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Item")
    TMap<FString, FString> ItemProperties; // Additional properties

    FLootItemData()
    {
        ItemID = TEXT("");
        ItemName = TEXT("Unknown Item");
        ItemType = TEXT("misc");
        Rarity = ELootRarity::Common;
        Quantity = 1;
        Value = 10;
        Description = TEXT("");
        ItemIcon = nullptr;
        ItemMesh = nullptr;
        RarityColor = FLinearColor::White;
    }
};

/**
 * Loot container data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FLootContainerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    FString ContainerID;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    FString ContainerName;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    TArray<FLootItemData> ContainedItems;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    int32 Credits; // Credits in container

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    bool bIsLocked;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    int32 LockDifficulty; // 1-20 difficulty

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    bool bIsTrapped;

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    FString TrapType; // Type of trap

    UPROPERTY(BlueprintReadWrite, Category = "Loot Container")
    bool bHasBeenLooted;

    FLootContainerData()
    {
        ContainerID = TEXT("");
        ContainerName = TEXT("Container");
        Credits = 0;
        bIsLocked = false;
        LockDifficulty = 10;
        bIsTrapped = false;
        TrapType = TEXT("");
        bHasBeenLooted = false;
    }
};

/**
 * Loot events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLootPickedUp, const FLootItemData&, LootItem, AActor*, Collector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLootContainerOpened, const FLootContainerData&, Container);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLootNotification, const FString&, NotificationText, ELootRarity, Rarity);

/**
 * Loot Pickup Actor - Individual loot item that can be picked up
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ALootPickupActor : public AActor
{
    GENERATED_BODY()

public:
    ALootPickupActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Initialize loot pickup
     * @param LootData Data for this loot item
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Pickup")
    void InitializeLootPickup(const FLootItemData& LootData);

    /**
     * Pick up loot
     * @param Collector Actor collecting the loot
     * @return True if loot was picked up successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Pickup")
    bool PickupLoot(AActor* Collector);

    /**
     * Get loot data
     * @return Current loot data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loot Pickup")
    FLootItemData GetLootData() const { return LootData; }

    /**
     * Set loot data
     * @param NewLootData New loot data
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Pickup")
    void SetLootData(const FLootItemData& NewLootData);

    /**
     * Check if loot can be picked up
     * @param Collector Actor attempting to collect
     * @return True if loot can be collected
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loot Pickup")
    bool CanPickupLoot(AActor* Collector) const;

    // Event delegate
    UPROPERTY(BlueprintAssignable, Category = "Loot Events")
    FOnLootPickedUp OnLootPickedUp;

protected:
    // Loot data
    UPROPERTY(BlueprintReadOnly, Category = "Loot Data")
    FLootItemData LootData;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* LootMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* PickupSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* LootInfoWidget;

    // Visual effects
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UParticleSystemComponent* RarityEffect;

    // Pickup settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Settings")
    bool bAutoPickup; // Whether to auto-pickup on overlap

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Settings")
    float PickupRange; // Range for pickup interaction

private:
    // Helper methods
    void UpdateLootAppearance();
    void SetupRarityEffects();

    // Overlap events
    UFUNCTION()
    void OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                   bool bFromSweep, const FHitResult& SweepResult);

public:
    /**
     * Blueprint implementable events
     */

    /**
     * Called when loot is picked up (for custom effects)
     * @param LootItem The picked up loot item
     * @param Collector Actor that collected the loot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Loot Events")
    void OnLootPickedUpEvent(const FLootItemData& LootItem, AActor* Collector);

    /**
     * Called to customize loot appearance (override in Blueprint)
     * @param LootData The loot data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Loot Events")
    void CustomizeLootAppearance(const FLootItemData& LootData);
};

/**
 * Loot Chest Actor - Container with multiple loot items
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API ALootChestActor : public AActor
{
    GENERATED_BODY()

public:
    ALootChestActor();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Initialize loot chest
     * @param ContainerData Data for this container
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    void InitializeLootChest(const FLootContainerData& ContainerData);

    /**
     * Open chest
     * @param Opener Actor opening the chest
     * @return True if chest was opened successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    bool OpenChest(AActor* Opener);

    /**
     * Close chest
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    void CloseChest();

    /**
     * Attempt to unlock chest
     * @param Unlocker Actor attempting to unlock
     * @param SkillBonus Skill bonus for unlocking
     * @return True if unlock was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    bool AttemptUnlock(AActor* Unlocker, int32 SkillBonus = 0);

    /**
     * Disarm trap
     * @param Disarmer Actor attempting to disarm
     * @param SkillBonus Skill bonus for disarming
     * @return True if disarm was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    bool DisarmTrap(AActor* Disarmer, int32 SkillBonus = 0);

    /**
     * Take item from chest
     * @param ItemIndex Index of item to take
     * @param Taker Actor taking the item
     * @return True if item was taken successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    bool TakeItem(int32 ItemIndex, AActor* Taker);

    /**
     * Take all items from chest
     * @param Taker Actor taking all items
     * @return Number of items taken
     */
    UFUNCTION(BlueprintCallable, Category = "Loot Chest")
    int32 TakeAllItems(AActor* Taker);

    /**
     * Get container data
     * @return Current container data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loot Chest")
    FLootContainerData GetContainerData() const { return ContainerData; }

    /**
     * Check if chest is open
     * @return True if chest is currently open
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loot Chest")
    bool IsOpen() const { return bIsOpen; }

    /**
     * Check if chest can be opened
     * @param Opener Actor attempting to open
     * @return True if chest can be opened
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loot Chest")
    bool CanOpenChest(AActor* Opener) const;

    // Event delegate
    UPROPERTY(BlueprintAssignable, Category = "Loot Events")
    FOnLootContainerOpened OnLootContainerOpened;

protected:
    // Container data
    UPROPERTY(BlueprintReadOnly, Category = "Container Data")
    FLootContainerData ContainerData;

    // State
    UPROPERTY(BlueprintReadOnly, Category = "Container State")
    bool bIsOpen;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ChestMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* InteractionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* ChestInfoWidget;

    // Loot generator
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    ULootGeneratorComponent* LootGenerator;

private:
    // Helper methods
    void UpdateChestAppearance();
    void TriggerTrap(AActor* Victim);

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
     * Called when chest is opened (for custom effects)
     * @param ContainerData The container data
     * @param Opener Actor that opened the chest
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Loot Events")
    void OnChestOpenedEvent(const FLootContainerData& ContainerData, AActor* Opener);

    /**
     * Called when trap is triggered (for custom trap effects)
     * @param TrapType Type of trap triggered
     * @param Victim Actor that triggered the trap
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Loot Events")
    void OnTrapTriggeredEvent(const FString& TrapType, AActor* Victim);

    /**
     * Called to customize chest appearance (override in Blueprint)
     * @param ContainerData The container data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Loot Events")
    void CustomizeChestAppearance(const FLootContainerData& ContainerData);
};
