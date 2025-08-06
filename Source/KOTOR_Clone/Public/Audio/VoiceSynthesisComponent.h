// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Engine/Engine.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "VoiceSynthesisComponent.generated.h"

/**
 * TTS Provider types
 */
UENUM(BlueprintType)
enum class ETTSProvider : uint8
{
    ElevenLabs      UMETA(DisplayName = "ElevenLabs"),
    AzureSpeech     UMETA(DisplayName = "Azure Speech"),
    OpenTTS         UMETA(DisplayName = "OpenTTS"),
    LocalTTS        UMETA(DisplayName = "Local TTS"),
    MockTTS         UMETA(DisplayName = "Mock TTS (Testing)")
};

/**
 * Voice profile data for NPCs
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FVoiceProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString VoiceID;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString VoiceName;

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString Gender; // "Male", "Female", "Neutral"

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString Accent; // "American", "British", "Robotic", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    FString Personality; // "Authoritative", "Friendly", "Menacing", etc.

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    float Pitch; // 0.5 to 2.0

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    float Speed; // 0.5 to 2.0

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    float Stability; // 0.0 to 1.0 (ElevenLabs specific)

    UPROPERTY(BlueprintReadWrite, Category = "Voice")
    float Clarity; // 0.0 to 1.0 (ElevenLabs specific)

    FVoiceProfile()
    {
        VoiceID = TEXT("default");
        VoiceName = TEXT("Default Voice");
        Gender = TEXT("Neutral");
        Accent = TEXT("American");
        Personality = TEXT("Friendly");
        Pitch = 1.0f;
        Speed = 1.0f;
        Stability = 0.75f;
        Clarity = 0.75f;
    }
};

/**
 * TTS request data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FTTSRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "TTS")
    FString Text;

    UPROPERTY(BlueprintReadWrite, Category = "TTS")
    FVoiceProfile VoiceProfile;

    UPROPERTY(BlueprintReadWrite, Category = "TTS")
    FString RequestID;

    UPROPERTY(BlueprintReadWrite, Category = "TTS")
    bool bCacheAudio;

    FTTSRequest()
    {
        Text = TEXT("");
        RequestID = TEXT("");
        bCacheAudio = true;
    }
};

/**
 * Voice synthesis events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoiceSynthesisComplete, const FString&, RequestID, USoundWave*, GeneratedAudio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoiceSynthesisError, const FString&, RequestID, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoicePlaybackStarted, const FString&, RequestID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoicePlaybackFinished, const FString&, RequestID);

/**
 * Voice Synthesis Component - Handles AI-generated voice for NPC dialogue
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UVoiceSynthesisComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVoiceSynthesisComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    /**
     * Initialize the voice synthesis system
     * @param Provider TTS provider to use
     * @param APIKey API key for the provider (if required)
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    void InitializeVoiceSystem(ETTSProvider Provider, const FString& APIKey = TEXT(""));

    /**
     * Synthesize speech from text
     * @param Request TTS request data
     * @return Request ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    FString SynthesizeSpeech(const FTTSRequest& Request);

    /**
     * Play synthesized audio
     * @param RequestID The request ID from synthesis
     * @param AudioComponent Audio component to play through (optional)
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    void PlaySynthesizedAudio(const FString& RequestID, UAudioComponent* AudioComponent = nullptr);

    /**
     * Stop current voice playback
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    void StopVoicePlayback();

    /**
     * Get voice profile for NPC
     * @param NPCData NPC data containing voice information
     * @return Voice profile for the NPC
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    FVoiceProfile GetVoiceProfileForNPC(const FNPCData& NPCData);

    /**
     * Speak NPC dialogue line
     * @param NPCData NPC speaking the line
     * @param DialogueText Text to speak
     * @param AudioComponent Audio component to play through
     * @return Request ID for tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    FString SpeakNPCDialogue(const FNPCData& NPCData, const FString& DialogueText, 
                            UAudioComponent* AudioComponent = nullptr);

    /**
     * Check if audio is cached for a specific text and voice
     * @param Text The text to check
     * @param VoiceProfile Voice profile to check
     * @return True if audio is cached
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice Synthesis")
    bool IsAudioCached(const FString& Text, const FVoiceProfile& VoiceProfile) const;

    /**
     * Clear audio cache
     */
    UFUNCTION(BlueprintCallable, Category = "Voice Synthesis")
    void ClearAudioCache();

    /**
     * Get available voice profiles
     * @return Array of available voice profiles
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice Synthesis")
    TArray<FVoiceProfile> GetAvailableVoiceProfiles() const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Voice Events")
    FOnVoiceSynthesisComplete OnVoiceSynthesisComplete;

    UPROPERTY(BlueprintAssignable, Category = "Voice Events")
    FOnVoiceSynthesisError OnVoiceSynthesisError;

    UPROPERTY(BlueprintAssignable, Category = "Voice Events")
    FOnVoicePlaybackStarted OnVoicePlaybackStarted;

    UPROPERTY(BlueprintAssignable, Category = "Voice Events")
    FOnVoicePlaybackFinished OnVoicePlaybackFinished;

protected:
    // TTS Provider settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    ETTSProvider CurrentProvider;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    FString APIKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    FString APIEndpoint;

    // Audio settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    float MasterVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    bool bEnableAudioCaching;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    int32 MaxCacheSize; // Maximum number of cached audio files

    // Voice profiles
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    TArray<FVoiceProfile> VoiceProfiles;

    // Default voice mappings by species/role
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    TMap<FString, FString> SpeciesVoiceMapping; // Species -> VoiceID

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice Synthesis")
    TMap<FString, FString> RoleVoiceMapping; // Role -> VoiceID

    // Audio cache
    UPROPERTY()
    TMap<FString, USoundWave*> AudioCache;

    // Active requests
    UPROPERTY()
    TMap<FString, FTTSRequest> ActiveRequests;

    // Audio component for playback
    UPROPERTY()
    UAudioComponent* DefaultAudioComponent;

    // Current playback state
    UPROPERTY(BlueprintReadOnly, Category = "Voice Synthesis")
    bool bIsPlaying;

    UPROPERTY(BlueprintReadOnly, Category = "Voice Synthesis")
    FString CurrentRequestID;

private:
    // TTS Provider implementations
    void SynthesizeWithElevenLabs(const FTTSRequest& Request);
    void SynthesizeWithAzureSpeech(const FTTSRequest& Request);
    void SynthesizeWithOpenTTS(const FTTSRequest& Request);
    void SynthesizeWithLocalTTS(const FTTSRequest& Request);
    void SynthesizeWithMockTTS(const FTTSRequest& Request);

    // Helper methods
    FString GenerateRequestID();
    FString GenerateCacheKey(const FString& Text, const FVoiceProfile& VoiceProfile);
    USoundWave* CreateSoundWaveFromData(const TArray<uint8>& AudioData);
    void OnAudioPlaybackFinished();

    // HTTP request handling
    void HandleHTTPResponse(class FHttpRequestPtr Request, class FHttpResponsePtr Response, bool bWasSuccessful, FString RequestID);

    // Audio cache management
    void AddToCache(const FString& CacheKey, USoundWave* SoundWave);
    USoundWave* GetFromCache(const FString& CacheKey);
    void CleanupCache();

public:
    /**
     * Blueprint implementable events for custom voice logic
     */

    /**
     * Called when voice synthesis starts (for custom logic)
     * @param RequestID The request ID
     * @param Text The text being synthesized
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Voice Events")
    void OnVoiceSynthesisStarted(const FString& RequestID, const FString& Text);

    /**
     * Called to get custom voice profile for NPC (override in Blueprint)
     * @param NPCData The NPC data
     * @return Custom voice profile
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Voice Events")
    FVoiceProfile GetCustomVoiceProfile(const FNPCData& NPCData);

    /**
     * Called to process text before synthesis (override in Blueprint)
     * @param OriginalText The original dialogue text
     * @param NPCData The NPC speaking
     * @return Processed text for synthesis
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Voice Events")
    FString ProcessTextForSynthesis(const FString& OriginalText, const FNPCData& NPCData);
};
