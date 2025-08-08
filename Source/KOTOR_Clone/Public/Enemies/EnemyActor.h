// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "AIDM/LootGeneratorComponent.h"
#include "EnemyActor.generated.h"

/**
 * Enemy types
 */
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
    Humanoid            UMETA(DisplayName = "Humanoid"),
    Beast               UMETA(DisplayName = "Beast"),
    Droid               UMETA(DisplayName = "Droid"),
    Sith                UMETA(DisplayName = "Sith"),
    Jedi                UMETA(DisplayName = "Jedi"),
    Alien               UMETA(DisplayName = "Alien"),
    Undead              UMETA(DisplayName = "Undead"),
    Construct           UMETA(DisplayName = "Construct"),
    Elemental           UMETA(DisplayName = "Elemental"),
    Boss                UMETA(DisplayName = "Boss")
};

/**
 * Challenge rating data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FChallengeRating
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    float ChallengeRating; // 0.125 to 30+

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 ExperienceValue; // XP awarded for defeating

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 ProficiencyBonus; // Proficiency bonus for this CR

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 ArmorClass; // Suggested AC for this CR

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 HitPoints; // Suggested HP for this CR

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 AttackBonus; // Suggested attack bonus

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 DamagePerRound; // Suggested damage per round

    UPROPERTY(BlueprintReadWrite, Category = "Challenge Rating")
    int32 SaveDC; // Suggested save DC

    FChallengeRating()
    {
        ChallengeRating = 1.0f;
        ExperienceValue = 200;
        ProficiencyBonus = 2;
        ArmorClass = 13;
        HitPoints = 20;
        AttackBonus = 3;
        DamagePerRound = 6;
        SaveDC = 11;
    }
};

/**
 * Enemy stats
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FEnemyStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Strength;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Dexterity;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Constitution;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Intelligence;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Wisdom;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Charisma;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 MaxHitPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 CurrentHitPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 ArmorClass;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Stats")
    int32 Speed;

    FEnemyStats()
    {
        Strength = 10;
        Dexterity = 10;
        Constitution = 10;
        Intelligence = 10;
        Wisdom = 10;
        Charisma = 10;
        MaxHitPoints = 20;
        CurrentHitPoints = 20;
        ArmorClass = 10;
        Speed = 30;
    }
};

/**
 * Enemy data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FEnemyData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    FString EnemyID;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    FString DisplayName;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    EEnemyType EnemyType;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    FChallengeRating ChallengeRating;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    FEnemyStats Stats;

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    TArray<FString> Abilities; // Special abilities

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    TArray<FString> Resistances; // Damage resistances

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    TArray<FString> Immunities; // Damage immunities

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    TArray<FString> Vulnerabilities; // Damage vulnerabilities

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    FString LootTable; // Loot table ID

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    FString Faction; // Enemy faction

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Data")
    bool bIsBoss; // Whether this is a boss enemy

    FEnemyData()
    {
        EnemyID = TEXT("");
        DisplayName = TEXT("Enemy");
        EnemyType = EEnemyType::Humanoid;
        LootTable = TEXT("default");
        Faction = TEXT("hostile");
        bIsBoss = false;
    }
};

/**
 * Enemy events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AEnemyActor*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDamaged, AEnemyActor*, Enemy, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyAbilityUsed, const FString&, AbilityName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLootDropped, AEnemyActor*, Enemy, const TArray<FString>&, DroppedItems);

/**
 * Enemy Actor - Scalable enemy with CR system and loot drops
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API AEnemyActor : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * Initialize enemy with data
     * @param EnemyData Data to initialize the enemy with
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void InitializeEnemy(const FEnemyData& TargetEnemyData);

    /**
     * Scale enemy to challenge rating
     * @param TargetCR Target challenge rating
     * @param PlayerLevel Player level for scaling context
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ScaleToChallenge(float TargetCR, int32 PlayerLevel = 1);

    /**
     * Take damage
     * @param DamageAmount Amount of damage to take
     * @param DamageType Type of damage
     * @param DamageSource Source of damage
     * @return Actual damage taken
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    float TakeDamage(float DamageAmount, const FString& DamageType, AActor* DamageSource);

    /**
     * Heal enemy
     * @param HealAmount Amount to heal
     * @return Actual amount healed
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    float Heal(float HealAmount);

    /**
     * Use ability
     * @param AbilityName Name of ability to use
     * @param Target Target for the ability
     * @return True if ability was used successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool UseAbility(const FString& AbilityName, AActor* Target = nullptr);

    /**
     * Die and drop loot
     * @param Killer Actor that killed this enemy
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void Die(AActor* Killer = nullptr);

    /**
     * Drop loot
     * @param LootRecipient Actor to receive loot
     * @return Array of dropped item IDs
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    TArray<FString> DropLoot(AActor* LootRecipient = nullptr);

    /**
     * Get current health percentage
     * @return Health percentage (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
    float GetHealthPercentage() const;

    /**
     * Check if enemy is alive
     * @return True if enemy has health remaining
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
    bool IsAlive() const { return EnemyData.Stats.CurrentHitPoints > 0; }

    /**
     * Get enemy data
     * @return Current enemy data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
    FEnemyData GetEnemyData() const { return EnemyData; }

    /**
     * Set enemy faction
     * @param NewFaction New faction for the enemy
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void SetEnemyFaction(const FString& NewFaction);

    /**
     * Add ability to enemy
     * @param AbilityName Name of ability to add
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void AddAbility(const FString& AbilityName);

    /**
     * Remove ability from enemy
     * @param AbilityName Name of ability to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void RemoveAbility(const FString& AbilityName);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Enemy Events")
    FOnEnemyDeath OnEnemyDeath;

    UPROPERTY(BlueprintAssignable, Category = "Enemy Events")
    FOnEnemyDamaged OnEnemyDamaged;

    UPROPERTY(BlueprintAssignable, Category = "Enemy Events")
    FOnEnemyAbilityUsed OnEnemyAbilityUsed;

    UPROPERTY(BlueprintAssignable, Category = "Enemy Events")
    FOnLootDropped OnLootDropped;

protected:
    // Enemy data
    UPROPERTY(BlueprintReadOnly, Category = "Enemy Data")
    FEnemyData EnemyData;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* HealthBarWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    ULootGeneratorComponent* LootGenerator;

    // Challenge rating table
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge Rating")
    TMap<float, FChallengeRating> ChallengeRatingTable;

    // Ability cooldowns
    UPROPERTY(BlueprintReadOnly, Category = "Abilities")
    TMap<FString, float> AbilityCooldowns;

    // Death state
    UPROPERTY(BlueprintReadOnly, Category = "Death")
    bool bIsDead;

    UPROPERTY(BlueprintReadOnly, Category = "Death")
    float DeathTime;

private:
    // Helper methods
    void LoadChallengeRatingTable();
    FChallengeRating GetChallengeRatingData(float CR) const;
    void ApplyChallengeRatingStats(const FChallengeRating& CRData);
    void UpdateHealthBar();
    void SetupLootGenerator();
    float CalculateDamageReduction(const FString& DamageType) const;
    bool IsAbilityOnCooldown(const FString& AbilityName) const;
    void StartAbilityCooldown(const FString& AbilityName, float CooldownTime);

public:
    /**
     * Blueprint implementable events for custom enemy logic
     */

    /**
     * Called when enemy is initialized (for custom setup)
     * @param InitializedEnemyData The enemy data used for initialization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Events")
    void OnEnemyInitialized(const FEnemyData& InitializedEnemyData);

    /**
     * Called when enemy takes damage (for custom effects)
     * @param DamageAmount Amount of damage taken
     * @param DamageType Type of damage
     * @param DamageSource Source of damage
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Events")
    void OnEnemyDamagedEvent(float DamageAmount, const FString& DamageType, AActor* DamageSource);

    /**
     * Called when enemy dies (for custom death effects)
     * @param Killer Actor that killed this enemy
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Events")
    void OnEnemyDeathEvent(AActor* Killer);

    /**
     * Called to execute custom ability (override in Blueprint)
     * @param AbilityName Name of ability to execute
     * @param Target Target for the ability
     * @return True if ability was executed successfully
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Events")
    bool ExecuteCustomAbility(const FString& AbilityName, AActor* Target);

    /**
     * Called to customize enemy appearance (override in Blueprint)
     * @param EnemyData The enemy data for customization
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Events")
    void CustomizeEnemyAppearance(const FEnemyData& TargetEnemyData);

    /**
     * Called when loot is dropped (for custom loot effects)
     * @param DroppedItems Array of dropped item IDs
     * @param LootRecipient Actor receiving the loot
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Events")
    void OnLootDroppedEvent(const TArray<FString>& DroppedItems, AActor* LootRecipient);
};
