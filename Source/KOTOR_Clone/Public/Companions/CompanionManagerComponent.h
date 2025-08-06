// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/QuestManagerComponent.h"
#include "CompanionManagerComponent.generated.h"

/**
 * Companion combat roles
 */
UENUM(BlueprintType)
enum class ECompanionRole : uint8
{
    Tank        UMETA(DisplayName = "Tank"),
    DPS         UMETA(DisplayName = "DPS"),
    Healer      UMETA(DisplayName = "Healer"),
    Support     UMETA(DisplayName = "Support"),
    Utility     UMETA(DisplayName = "Utility"),
    Balanced    UMETA(DisplayName = "Balanced")
};

/**
 * Companion loyalty levels
 */
UENUM(BlueprintType)
enum class ECompanionLoyalty : uint8
{
    Hostile     UMETA(DisplayName = "Hostile"),
    Unfriendly  UMETA(DisplayName = "Unfriendly"),
    Neutral     UMETA(DisplayName = "Neutral"),
    Friendly    UMETA(DisplayName = "Friendly"),
    Loyal       UMETA(DisplayName = "Loyal"),
    Devoted     UMETA(DisplayName = "Devoted")
};

/**
 * Companion combat orders
 */
UENUM(BlueprintType)
enum class ECompanionOrder : uint8
{
    Follow      UMETA(DisplayName = "Follow"),
    Stay        UMETA(DisplayName = "Stay"),
    Attack      UMETA(DisplayName = "Attack Target"),
    Defend      UMETA(DisplayName = "Defend"),
    Heal        UMETA(DisplayName = "Heal Party"),
    Retreat     UMETA(DisplayName = "Retreat"),
    Free        UMETA(DisplayName = "Act Freely")
};

/**
 * Companion data structure
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FCompanionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString Species;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    ECompanionRole Role;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString Alignment; // "light", "dark", "neutral"

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString Backstory;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    TArray<FString> LikesDislikes; // Things that affect loyalty

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    TArray<FString> PersonalityTraits;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString RecruitmentQuest; // Quest ID that recruits this companion

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString PersonalQuest; // Companion's personal quest line

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    TMap<FString, int32> CombatStats; // Health, Attack, Defense, etc.

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    TArray<FString> Abilities; // Special abilities

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FString VoiceProfile; // Voice ID for TTS

    FCompanionData()
    {
        Name = TEXT("");
        Species = TEXT("Human");
        Role = ECompanionRole::Balanced;
        Alignment = TEXT("neutral");
        Backstory = TEXT("");
        RecruitmentQuest = TEXT("");
        PersonalQuest = TEXT("");
        VoiceProfile = TEXT("default");
    }
};

/**
 * Active companion instance
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FActiveCompanion
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    FCompanionData CompanionData;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    ECompanionLoyalty Loyalty;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    int32 LoyaltyPoints;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    ECompanionOrder CurrentOrder;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    bool bIsRecruited;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    bool bIsInParty;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    bool bIsAlive;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    APawn* CompanionPawn;

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    TMap<FString, float> RelationshipHistory; // Event -> Impact

    UPROPERTY(BlueprintReadWrite, Category = "Companion")
    float LastCommentaryTime;

    FActiveCompanion()
    {
        Loyalty = ECompanionLoyalty::Neutral;
        LoyaltyPoints = 0;
        CurrentOrder = ECompanionOrder::Follow;
        bIsRecruited = false;
        bIsInParty = false;
        bIsAlive = true;
        CompanionPawn = nullptr;
        LastCommentaryTime = 0.0f;
    }
};

/**
 * Companion events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompanionRecruited, const FActiveCompanion&, Companion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompanionLoyaltyChanged, const FActiveCompanion&, Companion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionCommentary, const FActiveCompanion&, Companion, const FString&, Commentary);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionOrderGiven, const FActiveCompanion&, Companion, ECompanionOrder, Order);

/**
 * Companion Manager Component - Manages party companions and their AI
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UCompanionManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCompanionManagerComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize companion system with campaign data
     * @param CampaignLoader Campaign loader for companion data
     * @param QuestManager Quest manager for recruitment tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    void InitializeCompanionSystem(UCampaignLoaderSubsystem* CampaignLoader, UQuestManagerComponent* QuestManager);

    /**
     * Recruit a companion
     * @param CompanionName Name of the companion to recruit
     * @return True if recruitment was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    bool RecruitCompanion(const FString& CompanionName);

    /**
     * Add companion to active party
     * @param CompanionName Name of the companion to add
     * @return True if companion was added to party
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    bool AddCompanionToParty(const FString& CompanionName);

    /**
     * Remove companion from active party
     * @param CompanionName Name of the companion to remove
     * @return True if companion was removed from party
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    bool RemoveCompanionFromParty(const FString& CompanionName);

    /**
     * Give order to a specific companion
     * @param CompanionName Name of the companion
     * @param Order Order to give
     * @param Target Optional target for the order
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    void GiveCompanionOrder(const FString& CompanionName, ECompanionOrder Order, AActor* Target = nullptr);

    /**
     * Give order to all companions
     * @param Order Order to give to all companions
     * @param Target Optional target for the order
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    void GiveOrderToAllCompanions(ECompanionOrder Order, AActor* Target = nullptr);

    /**
     * Adjust companion loyalty
     * @param CompanionName Name of the companion
     * @param LoyaltyChange Amount to change loyalty (positive or negative)
     * @param Reason Reason for the loyalty change
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    void AdjustCompanionLoyalty(const FString& CompanionName, int32 LoyaltyChange, const FString& Reason);

    /**
     * Trigger companion commentary
     * @param EventType Type of event that triggered commentary
     * @param Context Additional context for the commentary
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    void TriggerCompanionCommentary(const FString& EventType, const FString& Context = TEXT(""));

    /**
     * Get active companion by name
     * @param CompanionName Name of the companion
     * @return Active companion data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Manager")
    FActiveCompanion GetActiveCompanion(const FString& CompanionName) const;

    /**
     * Get all recruited companions
     * @return Array of all recruited companions
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Manager")
    TArray<FActiveCompanion> GetRecruitedCompanions() const;

    /**
     * Get current party members
     * @return Array of companions currently in the party
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Manager")
    TArray<FActiveCompanion> GetPartyMembers() const;

    /**
     * Check if companion is recruited
     * @param CompanionName Name of the companion
     * @return True if companion is recruited
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Manager")
    bool IsCompanionRecruited(const FString& CompanionName) const;

    /**
     * Check if companion is in party
     * @param CompanionName Name of the companion
     * @return True if companion is in active party
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Manager")
    bool IsCompanionInParty(const FString& CompanionName) const;

    /**
     * Get companion loyalty level
     * @param CompanionName Name of the companion
     * @return Current loyalty level
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Companion Manager")
    ECompanionLoyalty GetCompanionLoyalty(const FString& CompanionName) const;

    /**
     * Save companion data
     * @return Serialized companion data as JSON string
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    FString SaveCompanionData() const;

    /**
     * Load companion data
     * @param SaveData Serialized companion data as JSON string
     * @return True if data was loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Companion Manager")
    bool LoadCompanionData(const FString& SaveData);

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Companion Events")
    FOnCompanionRecruited OnCompanionRecruited;

    UPROPERTY(BlueprintAssignable, Category = "Companion Events")
    FOnCompanionLoyaltyChanged OnCompanionLoyaltyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Companion Events")
    FOnCompanionCommentary OnCompanionCommentary;

    UPROPERTY(BlueprintAssignable, Category = "Companion Events")
    FOnCompanionOrderGiven OnCompanionOrderGiven;

protected:
    // Available companions (loaded from campaign data)
    UPROPERTY(BlueprintReadOnly, Category = "Companion Manager")
    TArray<FCompanionData> AvailableCompanions;

    // Active companions (recruited)
    UPROPERTY(BlueprintReadOnly, Category = "Companion Manager")
    TArray<FActiveCompanion> ActiveCompanions;

    // Party settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion Manager")
    int32 MaxPartySize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion Manager")
    float CommentaryFrequency; // How often companions comment (in seconds)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion Manager")
    bool bEnableAutomaticCommentary;

    // Companion spawning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion Manager")
    TMap<FString, TSubclassOf<APawn>> CompanionPawnClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion Manager")
    TSubclassOf<APawn> DefaultCompanionPawnClass;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

private:
    // Helper methods
    FActiveCompanion* FindActiveCompanion(const FString& CompanionName);
    const FActiveCompanion* FindActiveCompanion(const FString& CompanionName) const;
    void LoadCompanionsFromCampaign();
    void SpawnCompanionPawn(FActiveCompanion& Companion);
    void DespawnCompanionPawn(FActiveCompanion& Companion);
    ECompanionLoyalty CalculateLoyaltyLevel(int32 LoyaltyPoints) const;
    FString GenerateCompanionCommentary(const FActiveCompanion& Companion, const FString& EventType, const FString& Context);
    void UpdateCompanionAI(FActiveCompanion& Companion, float DeltaTime);
    bool ShouldCompanionComment(const FActiveCompanion& Companion, const FString& EventType) const;

public:
    /**
     * Blueprint implementable events for custom companion logic
     */

    /**
     * Called when a companion should be spawned (override in Blueprint)
     * @param CompanionData The companion data
     * @return Spawned companion pawn
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Events")
    APawn* OnSpawnCompanion(const FCompanionData& CompanionData);

    /**
     * Called when companion loyalty changes (for custom logic)
     * @param Companion The companion whose loyalty changed
     * @param OldLoyalty Previous loyalty level
     * @param NewLoyalty New loyalty level
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Events")
    void OnCompanionLoyaltyChangedEvent(const FActiveCompanion& Companion, ECompanionLoyalty OldLoyalty, ECompanionLoyalty NewLoyalty);

    /**
     * Called to generate custom companion commentary (override in Blueprint)
     * @param Companion The companion
     * @param EventType Type of event
     * @param Context Event context
     * @return Custom commentary text
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Events")
    FString GenerateCustomCommentary(const FActiveCompanion& Companion, const FString& EventType, const FString& Context);

    /**
     * Called when companion receives an order (for custom AI logic)
     * @param Companion The companion
     * @param Order The order given
     * @param Target Optional target for the order
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Companion Events")
    void OnCompanionOrderReceived(const FActiveCompanion& Companion, ECompanionOrder Order, AActor* Target);
};
