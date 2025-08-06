// Copyright Epic Games, Inc. All Rights Reserved.

#include "Audio/VoiceSynthesisComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Engine/Engine.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/Base64.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

UVoiceSynthesisComponent::UVoiceSynthesisComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Initialize default settings
    CurrentProvider = ETTSProvider::MockTTS;
    APIKey = TEXT("");
    APIEndpoint = TEXT("");
    MasterVolume = 1.0f;
    bEnableAudioCaching = true;
    MaxCacheSize = 100;
    bIsPlaying = false;
    CurrentRequestID = TEXT("");
    
    // Initialize default voice profiles
    InitializeDefaultVoiceProfiles();
    
    // Initialize species and role mappings
    InitializeVoiceMappings();
}

void UVoiceSynthesisComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Create default audio component
    if (AActor* Owner = GetOwner())
    {
        DefaultAudioComponent = Owner->CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceAudioComponent"));
        if (DefaultAudioComponent)
        {
            DefaultAudioComponent->SetVolumeMultiplier(MasterVolume);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("VoiceSynthesisComponent: Initialized with provider %s"), 
           *UEnum::GetValueAsString(CurrentProvider));
}

void UVoiceSynthesisComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopVoicePlayback();
    ClearAudioCache();
    Super::EndPlay(EndPlayReason);
}

void UVoiceSynthesisComponent::InitializeVoiceSystem(ETTSProvider Provider, const FString& InAPIKey)
{
    CurrentProvider = Provider;
    APIKey = InAPIKey;
    
    // Set provider-specific endpoints
    switch (Provider)
    {
        case ETTSProvider::ElevenLabs:
            APIEndpoint = TEXT("https://api.elevenlabs.io/v1/text-to-speech/");
            break;
        case ETTSProvider::AzureSpeech:
            APIEndpoint = TEXT("https://[region].tts.speech.microsoft.com/cognitiveservices/v1");
            break;
        case ETTSProvider::OpenTTS:
            APIEndpoint = TEXT("http://localhost:5500/api/tts");
            break;
        case ETTSProvider::LocalTTS:
            APIEndpoint = TEXT("http://localhost:8080/tts");
            break;
        case ETTSProvider::MockTTS:
        default:
            APIEndpoint = TEXT("mock://tts");
            break;
    }
    
    UE_LOG(LogTemp, Log, TEXT("VoiceSynthesisComponent: Initialized %s provider"), 
           *UEnum::GetValueAsString(Provider));
}

FString UVoiceSynthesisComponent::SynthesizeSpeech(const FTTSRequest& Request)
{
    FString RequestID = GenerateRequestID();
    
    // Store request for tracking
    FTTSRequest StoredRequest = Request;
    StoredRequest.RequestID = RequestID;
    ActiveRequests.Add(RequestID, StoredRequest);
    
    // Check cache first
    if (bEnableAudioCaching && Request.bCacheAudio)
    {
        FString CacheKey = GenerateCacheKey(Request.Text, Request.VoiceProfile);
        if (USoundWave* CachedAudio = GetFromCache(CacheKey))
        {
            OnVoiceSynthesisComplete.Broadcast(RequestID, CachedAudio);
            return RequestID;
        }
    }
    
    // Broadcast synthesis started event
    OnVoiceSynthesisStarted(RequestID, Request.Text);
    
    // Route to appropriate provider
    switch (CurrentProvider)
    {
        case ETTSProvider::ElevenLabs:
            SynthesizeWithElevenLabs(StoredRequest);
            break;
        case ETTSProvider::AzureSpeech:
            SynthesizeWithAzureSpeech(StoredRequest);
            break;
        case ETTSProvider::OpenTTS:
            SynthesizeWithOpenTTS(StoredRequest);
            break;
        case ETTSProvider::LocalTTS:
            SynthesizeWithLocalTTS(StoredRequest);
            break;
        case ETTSProvider::MockTTS:
        default:
            SynthesizeWithMockTTS(StoredRequest);
            break;
    }
    
    return RequestID;
}

void UVoiceSynthesisComponent::PlaySynthesizedAudio(const FString& RequestID, UAudioComponent* AudioComponent)
{
    // Find the request
    if (!ActiveRequests.Contains(RequestID))
    {
        UE_LOG(LogTemp, Warning, TEXT("VoiceSynthesisComponent: Request %s not found"), *RequestID);
        return;
    }
    
    const FTTSRequest& Request = ActiveRequests[RequestID];
    
    // Get cached audio
    FString CacheKey = GenerateCacheKey(Request.Text, Request.VoiceProfile);
    USoundWave* SoundWave = GetFromCache(CacheKey);
    
    if (!SoundWave)
    {
        UE_LOG(LogTemp, Warning, TEXT("VoiceSynthesisComponent: No audio available for request %s"), *RequestID);
        return;
    }
    
    // Use provided audio component or default
    UAudioComponent* TargetAudioComponent = AudioComponent ? AudioComponent : DefaultAudioComponent;
    
    if (TargetAudioComponent)
    {
        // Stop current playback
        StopVoicePlayback();
        
        // Set up new playback
        TargetAudioComponent->SetSound(SoundWave);
        TargetAudioComponent->Play();
        
        bIsPlaying = true;
        CurrentRequestID = RequestID;
        
        OnVoicePlaybackStarted.Broadcast(RequestID);
        
        UE_LOG(LogTemp, Log, TEXT("VoiceSynthesisComponent: Playing audio for request %s"), *RequestID);
    }
}

void UVoiceSynthesisComponent::StopVoicePlayback()
{
    if (DefaultAudioComponent && bIsPlaying)
    {
        DefaultAudioComponent->Stop();
        
        if (!CurrentRequestID.IsEmpty())
        {
            OnVoicePlaybackFinished.Broadcast(CurrentRequestID);
        }
        
        bIsPlaying = false;
        CurrentRequestID = TEXT("");
    }
}

FVoiceProfile UVoiceSynthesisComponent::GetVoiceProfileForNPC(const FNPCData& NPCData)
{
    // Try custom voice profile first
    FVoiceProfile CustomProfile = GetCustomVoiceProfile(NPCData);
    if (!CustomProfile.VoiceID.Equals(TEXT("default")))
    {
        return CustomProfile;
    }
    
    // Check species mapping
    if (SpeciesVoiceMapping.Contains(NPCData.Species))
    {
        FString VoiceID = SpeciesVoiceMapping[NPCData.Species];
        for (const FVoiceProfile& Profile : VoiceProfiles)
        {
            if (Profile.VoiceID == VoiceID)
            {
                return Profile;
            }
        }
    }
    
    // Check role mapping
    if (RoleVoiceMapping.Contains(NPCData.Role))
    {
        FString VoiceID = RoleVoiceMapping[NPCData.Role];
        for (const FVoiceProfile& Profile : VoiceProfiles)
        {
            if (Profile.VoiceID == VoiceID)
            {
                return Profile;
            }
        }
    }
    
    // Return default voice profile
    if (VoiceProfiles.Num() > 0)
    {
        return VoiceProfiles[0];
    }
    
    return FVoiceProfile();
}

FString UVoiceSynthesisComponent::SpeakNPCDialogue(const FNPCData& NPCData, const FString& DialogueText, 
                                                  UAudioComponent* AudioComponent)
{
    // Process text for synthesis
    FString ProcessedText = ProcessTextForSynthesis(DialogueText, NPCData);
    if (ProcessedText.IsEmpty())
    {
        ProcessedText = DialogueText;
    }
    
    // Get voice profile for NPC
    FVoiceProfile VoiceProfile = GetVoiceProfileForNPC(NPCData);
    
    // Create TTS request
    FTTSRequest Request;
    Request.Text = ProcessedText;
    Request.VoiceProfile = VoiceProfile;
    Request.bCacheAudio = true;
    
    // Synthesize speech
    FString RequestID = SynthesizeSpeech(Request);
    
    // If using mock TTS, play immediately
    if (CurrentProvider == ETTSProvider::MockTTS)
    {
        // Small delay to simulate processing
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, RequestID, AudioComponent]()
        {
            PlaySynthesizedAudio(RequestID, AudioComponent);
        }, 0.5f, false);
    }
    
    return RequestID;
}

bool UVoiceSynthesisComponent::IsAudioCached(const FString& Text, const FVoiceProfile& VoiceProfile) const
{
    FString CacheKey = GenerateCacheKey(Text, VoiceProfile);
    return AudioCache.Contains(CacheKey);
}

void UVoiceSynthesisComponent::ClearAudioCache()
{
    AudioCache.Empty();
    UE_LOG(LogTemp, Log, TEXT("VoiceSynthesisComponent: Audio cache cleared"));
}

TArray<FVoiceProfile> UVoiceSynthesisComponent::GetAvailableVoiceProfiles() const
{
    return VoiceProfiles;
}

void UVoiceSynthesisComponent::SynthesizeWithMockTTS(const FTTSRequest& Request)
{
    // Mock TTS for testing - creates a simple beep sound
    UE_LOG(LogTemp, Log, TEXT("VoiceSynthesisComponent: Mock TTS synthesis for: %s"), *Request.Text);
    
    // Create a simple sine wave sound for testing
    USoundWave* MockSoundWave = NewObject<USoundWave>();
    if (MockSoundWave)
    {
        // Set basic properties
        MockSoundWave->SetSampleRate(44100);
        MockSoundWave->NumChannels = 1;
        MockSoundWave->Duration = 2.0f; // 2 second duration
        
        // Generate simple sine wave data
        int32 SampleCount = 44100 * 2; // 2 seconds at 44.1kHz
        TArray<int16> SampleData;
        SampleData.SetNum(SampleCount);
        
        for (int32 i = 0; i < SampleCount; i++)
        {
            float Time = static_cast<float>(i) / 44100.0f;
            float Frequency = 440.0f; // A4 note
            float Amplitude = 0.3f;
            SampleData[i] = static_cast<int16>(Amplitude * 32767.0f * FMath::Sin(2.0f * PI * Frequency * Time));
        }
        
        // Set the raw PCM data
        MockSoundWave->RawPCMData = reinterpret_cast<uint8*>(SampleData.GetData());
        MockSoundWave->RawPCMDataSize = SampleData.Num() * sizeof(int16);
        
        // Cache the audio
        if (bEnableAudioCaching && Request.bCacheAudio)
        {
            FString CacheKey = GenerateCacheKey(Request.Text, Request.VoiceProfile);
            AddToCache(CacheKey, MockSoundWave);
        }
        
        // Broadcast completion
        OnVoiceSynthesisComplete.Broadcast(Request.RequestID, MockSoundWave);
    }
    else
    {
        OnVoiceSynthesisError.Broadcast(Request.RequestID, TEXT("Failed to create mock sound wave"));
    }
}

void UVoiceSynthesisComponent::SynthesizeWithElevenLabs(const FTTSRequest& Request)
{
    // ElevenLabs API implementation
    if (APIKey.IsEmpty())
    {
        OnVoiceSynthesisError.Broadcast(Request.RequestID, TEXT("ElevenLabs API key not set"));
        return;
    }
    
    // Create HTTP request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    
    // Set URL
    FString URL = APIEndpoint + Request.VoiceProfile.VoiceID;
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("POST"));
    
    // Set headers
    HttpRequest->SetHeader(TEXT("Accept"), TEXT("audio/mpeg"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("xi-api-key"), APIKey);
    
    // Create JSON payload
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(TEXT("text"), Request.Text);
    
    TSharedPtr<FJsonObject> VoiceSettings = MakeShareable(new FJsonObject);
    VoiceSettings->SetNumberField(TEXT("stability"), Request.VoiceProfile.Stability);
    VoiceSettings->SetNumberField(TEXT("similarity_boost"), Request.VoiceProfile.Clarity);
    JsonObject->SetObjectField(TEXT("voice_settings"), VoiceSettings);
    
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    HttpRequest->SetContentAsString(OutputString);
    
    // Bind response handler
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UVoiceSynthesisComponent::HandleHTTPResponse, Request.RequestID);
    
    // Send request
    HttpRequest->ProcessRequest();
    
    UE_LOG(LogTemp, Log, TEXT("VoiceSynthesisComponent: ElevenLabs request sent for: %s"), *Request.Text);
}

// Placeholder implementations for other providers
void UVoiceSynthesisComponent::SynthesizeWithAzureSpeech(const FTTSRequest& Request)
{
    UE_LOG(LogTemp, Warning, TEXT("VoiceSynthesisComponent: Azure Speech not implemented yet"));
    OnVoiceSynthesisError.Broadcast(Request.RequestID, TEXT("Azure Speech not implemented"));
}

void UVoiceSynthesisComponent::SynthesizeWithOpenTTS(const FTTSRequest& Request)
{
    UE_LOG(LogTemp, Warning, TEXT("VoiceSynthesisComponent: OpenTTS not implemented yet"));
    OnVoiceSynthesisError.Broadcast(Request.RequestID, TEXT("OpenTTS not implemented"));
}

void UVoiceSynthesisComponent::SynthesizeWithLocalTTS(const FTTSRequest& Request)
{
    UE_LOG(LogTemp, Warning, TEXT("VoiceSynthesisComponent: Local TTS not implemented yet"));
    OnVoiceSynthesisError.Broadcast(Request.RequestID, TEXT("Local TTS not implemented"));
}

FString UVoiceSynthesisComponent::GenerateRequestID()
{
    return FGuid::NewGuid().ToString();
}

FString UVoiceSynthesisComponent::GenerateCacheKey(const FString& Text, const FVoiceProfile& VoiceProfile)
{
    FString CombinedString = Text + VoiceProfile.VoiceID + FString::Printf(TEXT("%.2f"), VoiceProfile.Pitch);
    return FMD5::HashAnsiString(*CombinedString);
}

void UVoiceSynthesisComponent::AddToCache(const FString& CacheKey, USoundWave* SoundWave)
{
    if (AudioCache.Num() >= MaxCacheSize)
    {
        CleanupCache();
    }
    
    AudioCache.Add(CacheKey, SoundWave);
}

USoundWave* UVoiceSynthesisComponent::GetFromCache(const FString& CacheKey)
{
    if (AudioCache.Contains(CacheKey))
    {
        return AudioCache[CacheKey];
    }
    return nullptr;
}

void UVoiceSynthesisComponent::CleanupCache()
{
    // Simple cleanup - remove oldest entries
    if (AudioCache.Num() > MaxCacheSize * 0.8f)
    {
        int32 ToRemove = AudioCache.Num() - static_cast<int32>(MaxCacheSize * 0.8f);
        
        TArray<FString> Keys;
        AudioCache.GetKeys(Keys);
        
        for (int32 i = 0; i < ToRemove && i < Keys.Num(); i++)
        {
            AudioCache.Remove(Keys[i]);
        }
    }
}

void UVoiceSynthesisComponent::HandleHTTPResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, 
                                                 bool bWasSuccessful, FString RequestID)
{
    if (!ActiveRequests.Contains(RequestID))
    {
        return;
    }
    
    const FTTSRequest& TTSRequest = ActiveRequests[RequestID];
    
    if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
    {
        // Get audio data
        TArray<uint8> AudioData = Response->GetContent();
        
        // Create sound wave from data
        USoundWave* SoundWave = CreateSoundWaveFromData(AudioData);
        
        if (SoundWave)
        {
            // Cache the audio
            if (bEnableAudioCaching && TTSRequest.bCacheAudio)
            {
                FString CacheKey = GenerateCacheKey(TTSRequest.Text, TTSRequest.VoiceProfile);
                AddToCache(CacheKey, SoundWave);
            }
            
            OnVoiceSynthesisComplete.Broadcast(RequestID, SoundWave);
        }
        else
        {
            OnVoiceSynthesisError.Broadcast(RequestID, TEXT("Failed to create sound wave from response"));
        }
    }
    else
    {
        FString ErrorMessage = FString::Printf(TEXT("HTTP request failed: %d"), 
                                             Response.IsValid() ? Response->GetResponseCode() : 0);
        OnVoiceSynthesisError.Broadcast(RequestID, ErrorMessage);
    }
    
    // Clean up request
    ActiveRequests.Remove(RequestID);
}

USoundWave* UVoiceSynthesisComponent::CreateSoundWaveFromData(const TArray<uint8>& AudioData)
{
    // This is a simplified implementation
    // In a real implementation, you'd need to properly parse the audio format
    USoundWave* SoundWave = NewObject<USoundWave>();
    if (SoundWave && AudioData.Num() > 0)
    {
        // Set basic properties (these would need to be determined from the audio format)
        SoundWave->SetSampleRate(44100);
        SoundWave->NumChannels = 1;
        SoundWave->Duration = static_cast<float>(AudioData.Num()) / (44100.0f * 2.0f); // Rough estimate
        
        // Copy audio data
        SoundWave->RawPCMData = static_cast<uint8*>(FMemory::Malloc(AudioData.Num()));
        FMemory::Memcpy(SoundWave->RawPCMData, AudioData.GetData(), AudioData.Num());
        SoundWave->RawPCMDataSize = AudioData.Num();
        
        return SoundWave;
    }
    
    return nullptr;
}

void UVoiceSynthesisComponent::InitializeDefaultVoiceProfiles()
{
    // Create default voice profiles
    VoiceProfiles.Empty();
    
    // Default human male voice
    FVoiceProfile MaleVoice;
    MaleVoice.VoiceID = TEXT("human_male_01");
    MaleVoice.VoiceName = TEXT("Human Male");
    MaleVoice.Gender = TEXT("Male");
    MaleVoice.Personality = TEXT("Authoritative");
    VoiceProfiles.Add(MaleVoice);
    
    // Default human female voice
    FVoiceProfile FemaleVoice;
    FemaleVoice.VoiceID = TEXT("human_female_01");
    FemaleVoice.VoiceName = TEXT("Human Female");
    FemaleVoice.Gender = TEXT("Female");
    FemaleVoice.Personality = TEXT("Friendly");
    VoiceProfiles.Add(FemaleVoice);
    
    // Robotic voice for droids
    FVoiceProfile DroidVoice;
    DroidVoice.VoiceID = TEXT("droid_01");
    DroidVoice.VoiceName = TEXT("Droid Voice");
    DroidVoice.Gender = TEXT("Neutral");
    DroidVoice.Accent = TEXT("Robotic");
    DroidVoice.Personality = TEXT("Mechanical");
    DroidVoice.Pitch = 0.8f;
    VoiceProfiles.Add(DroidVoice);
    
    // Menacing voice for villains
    FVoiceProfile VillainVoice;
    VillainVoice.VoiceID = TEXT("villain_01");
    VillainVoice.VoiceName = TEXT("Menacing Voice");
    VillainVoice.Gender = TEXT("Male");
    VillainVoice.Personality = TEXT("Menacing");
    VillainVoice.Pitch = 0.7f;
    VillainVoice.Speed = 0.9f;
    VoiceProfiles.Add(VillainVoice);
}

void UVoiceSynthesisComponent::InitializeVoiceMappings()
{
    // Species to voice mappings
    SpeciesVoiceMapping.Add(TEXT("Human"), TEXT("human_male_01"));
    SpeciesVoiceMapping.Add(TEXT("Twi'lek"), TEXT("human_female_01"));
    SpeciesVoiceMapping.Add(TEXT("Droid"), TEXT("droid_01"));
    SpeciesVoiceMapping.Add(TEXT("Zabrak"), TEXT("villain_01"));
    
    // Role to voice mappings
    RoleVoiceMapping.Add(TEXT("Villain"), TEXT("villain_01"));
    RoleVoiceMapping.Add(TEXT("Sith"), TEXT("villain_01"));
    RoleVoiceMapping.Add(TEXT("Droid"), TEXT("droid_01"));
    RoleVoiceMapping.Add(TEXT("Merchant"), TEXT("human_male_01"));
    RoleVoiceMapping.Add(TEXT("Civilian"), TEXT("human_female_01"));
}
