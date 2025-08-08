// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MissingComponents.generated.h"

/**
 * Generic key/value pair replacement structure
 * Used to replace nested container types that are not UHT-compatible
 * (e.g. TMap<FString, TMap<FString,float>>) by flattening to
 * TArray<FKeyValuePair>.
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FKeyValuePair
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "KeyValue")
    FString Key;

    UPROPERTY(BlueprintReadWrite, Category = "KeyValue")
    float Value;

    FKeyValuePair()
        : Key(TEXT(""))
        , Value(0.0f)
    {}

    FKeyValuePair(const FString& InKey, float InValue)
        : Key(InKey)
        , Value(InValue)
    {}
};

/**
 * Voice provider types
 */
UENUM(BlueprintType)
enum class EVoiceProvider : uint8
{
    None                UMETA(DisplayName = "None"),
    Elevenlabs          UMETA(DisplayName = "ElevenLabs"),
    Azure               UMETA(DisplayName = "Azure TTS"),
    Amazon              UMETA(DisplayName = "Amazon Polly"),
    Google              UMETA(DisplayName = "Google Cloud TTS"),
    Local               UMETA(DisplayName = "Local TTS")
};

/**
 * Voice synthesis modes
 */
UENUM(BlueprintType)
enum class EVoiceSynthesisMode : uint8
{
    Realtime            UMETA(DisplayName = "Realtime"),
    Cached              UMETA(DisplayName = "Cached"),
    Streaming           UMETA(DisplayName = "Streaming"),
    Hybrid              UMETA(DisplayName = "Hybrid")
};

/**
 * Voice character data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FVoiceCharacterData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString CharacterID;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString VoiceID;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    EVoiceProvider Provider;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    float SpeakingRate;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    float PitchMultiplier;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    TMap<FString, float> EmotionSettings;

    FVoiceCharacterData()
        : CharacterID(TEXT(""))
        , VoiceID(TEXT(""))
        , Provider(EVoiceProvider::None)
        , SpeakingRate(1.0f)
        , PitchMultiplier(1.0f)
    {
    }
};

/**
 * Player vote for decision system
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlayerVote
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    FString PlayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    FString OptionID;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    float Weight;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    float Timestamp;

    FPlayerVote()
        : PlayerID(TEXT(""))
        , OptionID(TEXT(""))
        , Weight(1.0f)
        , Timestamp(0.0f)
    {
    }
};

/**
 * Decision session data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FDecisionSession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    FString DecisionContext;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    TArray<FString> Options;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    TArray<FPlayerVote> Votes;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    float StartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    float EndTime;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "Decision")
    FString WinningOption;

    FDecisionSession()
        : SessionID(TEXT(""))
        , DecisionContext(TEXT(""))
        , StartTime(0.0f)
        , EndTime(0.0f)
        , bIsActive(false)
        , WinningOption(TEXT(""))
    {
    }
};

/**
 * Loot Generator Component - Generates loot based on parameters
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTOR_CLONE_API ULootGeneratorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULootGeneratorComponent();

    // Basic functions to satisfy references
    UFUNCTION(BlueprintCallable, Category = "Loot")
    void GenerateLoot(int32 Level, const FString& LootTable);

    UFUNCTION(BlueprintCallable, Category = "Loot")
    void SpawnLoot(const FVector& Location);

protected:
    virtual void BeginPlay() override;
};

/**
 * Multiplayer Decision Sync Component - Synchronizes decision making in multiplayer
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTOR_CLONE_API UMultiplayerDecisionSyncComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMultiplayerDecisionSyncComponent();

    // Basic functions to satisfy references
    UFUNCTION(BlueprintCallable, Category = "Decision")
    FDecisionSession StartDecisionSession(const FString& Context, const TArray<FString>& Options);

    UFUNCTION(BlueprintCallable, Category = "Decision")
    void SubmitVote(const FString& SessionID, const FString& OptionID);

    UFUNCTION(BlueprintCallable, Category = "Decision")
    FString GetWinningOption(const FString& SessionID);

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    TArray<FDecisionSession> ActiveSessions;
};

/**
 * AI Director Subsystem - Manages AI-driven gameplay
 */
UCLASS()
class KOTOR_CLONE_API UAIDirectorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Initialize and shutdown
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Basic functions to satisfy references
    UFUNCTION(BlueprintCallable, Category = "AI Director")
    void ProcessPlayerAction(const FString& ActionType, const FString& Context);

    UFUNCTION(BlueprintCallable, Category = "AI Director")
    void GenerateResponse(const FString& Trigger);
};

/**
 * Campaign Session Manager - Manages multiplayer campaign sessions
 */
UCLASS()
class KOTOR_CLONE_API UCampaignSessionManager : public UObject
{
    GENERATED_BODY()

public:
    UCampaignSessionManager();

    // Basic functions to satisfy references
    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    FString CreateSession(const FString& CampaignID);

    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    bool JoinSession(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "Campaign Session")
    void LeaveSession();

    UPROPERTY()
    FString CurrentSessionID;
};

/**
 * Player Start Component - Defines player spawn locations
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTOR_CLONE_API UPlayerStartComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UPlayerStartComponent();

    // Basic functions to satisfy references
    UFUNCTION(BlueprintCallable, Category = "Player Start")
    FTransform GetStartTransform() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Start")
    int32 PlayerIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Start")
    bool bEnabled;
};
