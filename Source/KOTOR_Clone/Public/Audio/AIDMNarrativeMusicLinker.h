// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Audio/ProceduralMusicSubsystemV2.h"
#include "Audio/RuntimeMusicLayeringComponent.h"
#include "AIDMNarrativeMusicLinker.generated.h"

/**
 * AIDM narrative tag types
 */
UENUM(BlueprintType)
enum class EAIDMNarrativeTag : uint8
{
    // Story progression tags
    Introduction        UMETA(DisplayName = "Introduction"),
    RisingAction        UMETA(DisplayName = "Rising Action"),
    Climax              UMETA(DisplayName = "Climax"),
    FallingAction       UMETA(DisplayName = "Falling Action"),
    Resolution          UMETA(DisplayName = "Resolution"),
    
    // Character development tags
    CharacterMeet       UMETA(DisplayName = "Character Meet"),
    CharacterDeath      UMETA(DisplayName = "Character Death"),
    CharacterBetrayal   UMETA(DisplayName = "Character Betrayal"),
    CharacterRedemption UMETA(DisplayName = "Character Redemption"),
    CharacterRomance    UMETA(DisplayName = "Character Romance"),
    
    // Quest tags
    QuestStart          UMETA(DisplayName = "Quest Start"),
    QuestProgress       UMETA(DisplayName = "Quest Progress"),
    QuestComplete       UMETA(DisplayName = "Quest Complete"),
    QuestFail           UMETA(DisplayName = "Quest Fail"),
    QuestTwist          UMETA(DisplayName = "Quest Twist"),
    
    // Moral choice tags
    LightSideChoice     UMETA(DisplayName = "Light Side Choice"),
    DarkSideChoice      UMETA(DisplayName = "Dark Side Choice"),
    NeutralChoice       UMETA(DisplayName = "Neutral Choice"),
    MoralDilemma        UMETA(DisplayName = "Moral Dilemma"),
    
    // Discovery tags
    SecretRevealed      UMETA(DisplayName = "Secret Revealed"),
    ArtifactFound       UMETA(DisplayName = "Artifact Found"),
    LocationDiscovered  UMETA(DisplayName = "Location Discovered"),
    TruthUncovered      UMETA(DisplayName = "Truth Uncovered"),
    
    // Conflict tags
    CombatStart         UMETA(DisplayName = "Combat Start"),
    CombatVictory       UMETA(DisplayName = "Combat Victory"),
    CombatDefeat        UMETA(DisplayName = "Combat Defeat"),
    BossEncounter       UMETA(DisplayName = "Boss Encounter"),
    
    // Emotional tags
    Joy                 UMETA(DisplayName = "Joy"),
    Sadness             UMETA(DisplayName = "Sadness"),
    Fear                UMETA(DisplayName = "Fear"),
    Hope                UMETA(DisplayName = "Hope"),
    Despair             UMETA(DisplayName = "Despair"),
    Tension             UMETA(DisplayName = "Tension"),
    Relief              UMETA(DisplayName = "Relief")
};

/**
 * Narrative music mapping
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNarrativeMusicMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    FString MappingID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    EAIDMNarrativeTag NarrativeTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    EMusicTone TargetTone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    FString BlendPresetID; // Blend preset to apply

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    TArray<FString> StemsToActivate; // Specific stems to activate

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    TArray<FString> StemsToDeactivate; // Specific stems to deactivate

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    TMap<FString, float> StemVolumeOverrides; // Volume overrides for specific stems

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    float TransitionTime; // Time to transition to this mapping

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    float Priority; // Priority of this mapping (0.0 to 1.0)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    float Duration; // How long this mapping should last (-1 for indefinite)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    bool bOverrideCurrent; // Whether to override current music

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Narrative Music Mapping")
    TArray<FString> Prerequisites; // Conditions that must be met

    FNarrativeMusicMapping()
    {
        MappingID = TEXT("");
        NarrativeTag = EAIDMNarrativeTag::Introduction;
        TargetTone = EMusicTone::Neutral;
        BlendPresetID = TEXT("");
        TransitionTime = 3.0f;
        Priority = 0.5f;
        Duration = -1.0f;
        bOverrideCurrent = false;
    }
};

/**
 * Narrative context data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FNarrativeContextData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    EAIDMNarrativeTag CurrentTag;

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    FString ContextID;

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    float PlayerMorality; // -1.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    float StoryProgression; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    float EmotionalIntensity; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    TArray<FString> ActiveCharacters; // Characters involved in current scene

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    TArray<FString> ActiveTags; // All currently active narrative tags

    UPROPERTY(BlueprintReadWrite, Category = "Narrative Context")
    TMap<FString, FString> CustomData; // Additional context data

    FNarrativeContextData()
    {
        CurrentTag = EAIDMNarrativeTag::Introduction;
        ContextID = TEXT("");
        PlayerMorality = 0.0f;
        StoryProgression = 0.0f;
        EmotionalIntensity = 0.5f;
    }
};

/**
 * Narrative music events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNarrativeTagTriggered, EAIDMNarrativeTag, NarrativeTag, const FNarrativeContextData&, ContextData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNarrativeMusicChanged, const FString&, MappingID, const FNarrativeMusicMapping&, Mapping);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNarrativeContextChanged, const FNarrativeContextData&, NewContext);

/**
 * AIDM Narrative Music Linker - Connect narrative tags to music blend presets and stem switching
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAIDMNarrativeMusicLinker : public UActorComponent
{
    GENERATED_BODY()

public:
    UAIDMNarrativeMusicLinker();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /**
     * Initialize narrative music linker
     * @param MusicSubsystem Procedural music subsystem
     * @param LayeringComponent Runtime music layering component
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void InitializeNarrativeMusicLinker(UProceduralMusicSubsystemV2* MusicSubsystem, 
                                       URuntimeMusicLayeringComponent* LayeringComponent);

    /**
     * Trigger narrative tag
     * @param NarrativeTag Tag to trigger
     * @param ContextData Narrative context data
     * @return True if tag was processed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    bool TriggerNarrativeTag(EAIDMNarrativeTag NarrativeTag, const FNarrativeContextData& ContextData);

    /**
     * Add narrative music mapping
     * @param Mapping Mapping to add
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void AddNarrativeMusicMapping(const FNarrativeMusicMapping& Mapping);

    /**
     * Remove narrative music mapping
     * @param MappingID ID of mapping to remove
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void RemoveNarrativeMusicMapping(const FString& MappingID);

    /**
     * Set narrative context
     * @param ContextData New narrative context
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void SetNarrativeContext(const FNarrativeContextData& ContextData);

    /**
     * Update player morality
     * @param NewMorality New morality value (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void UpdatePlayerMorality(float NewMorality);

    /**
     * Update story progression
     * @param NewProgression New progression value (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void UpdateStoryProgression(float NewProgression);

    /**
     * Update emotional intensity
     * @param NewIntensity New intensity value (0.0 to 1.0)
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    void UpdateEmotionalIntensity(float NewIntensity);

    /**
     * Get mapping for narrative tag
     * @param NarrativeTag Tag to get mapping for
     * @param ContextData Current narrative context
     * @return Best matching mapping
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    FNarrativeMusicMapping GetMappingForNarrativeTag(EAIDMNarrativeTag NarrativeTag, 
                                                    const FNarrativeContextData& ContextData) const;

    /**
     * Get current narrative context
     * @return Current narrative context data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM Narrative Music Linker")
    FNarrativeContextData GetCurrentNarrativeContext() const { return CurrentNarrativeContext; }

    /**
     * Check if narrative tag is active
     * @param NarrativeTag Tag to check
     * @return True if tag is currently active
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM Narrative Music Linker")
    bool IsNarrativeTagActive(EAIDMNarrativeTag NarrativeTag) const;

    /**
     * Get active narrative tags
     * @return Array of currently active narrative tags
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIDM Narrative Music Linker")
    TArray<EAIDMNarrativeTag> GetActiveNarrativeTags() const;

    /**
     * Load narrative mappings from JSON
     * @param FilePath Path to JSON file
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    bool LoadNarrativeMappingsFromJSON(const FString& FilePath);

    /**
     * Save narrative mappings to JSON
     * @param FilePath Path to save JSON file
     * @return True if saved successfully
     */
    UFUNCTION(BlueprintCallable, Category = "AIDM Narrative Music Linker")
    bool SaveNarrativeMappingsToJSON(const FString& FilePath) const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Narrative Music Events")
    FOnNarrativeTagTriggered OnNarrativeTagTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Narrative Music Events")
    FOnNarrativeMusicChanged OnNarrativeMusicChanged;

    UPROPERTY(BlueprintAssignable, Category = "Narrative Music Events")
    FOnNarrativeContextChanged OnNarrativeContextChanged;

protected:
    // Narrative music mappings
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Mappings")
    TMap<EAIDMNarrativeTag, TArray<FNarrativeMusicMapping>> NarrativeMusicMappings;

    // Current narrative context
    UPROPERTY(BlueprintReadOnly, Category = "Narrative Context")
    FNarrativeContextData CurrentNarrativeContext;

    // Active narrative tags
    UPROPERTY(BlueprintReadOnly, Category = "Active Tags")
    TArray<EAIDMNarrativeTag> ActiveNarrativeTags;

    // Component references
    UPROPERTY()
    UProceduralMusicSubsystemV2* ProceduralMusicSubsystem;

    UPROPERTY()
    URuntimeMusicLayeringComponent* RuntimeMusicLayeringComponent;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Linker Settings")
    bool bNarrativeMusicEnabled; // Whether narrative music linking is enabled

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Linker Settings")
    float DefaultTransitionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Linker Settings")
    bool bAllowMultipleTags; // Whether multiple tags can be active simultaneously

    // Timer handle
    FTimerHandle NarrativeUpdateTimer;

private:
    // Helper methods
    void LoadDefaultNarrativeMappings();
    FNarrativeMusicMapping* FindBestMapping(EAIDMNarrativeTag NarrativeTag, const FNarrativeContextData& ContextData);
    bool CheckMappingPrerequisites(const FNarrativeMusicMapping& Mapping, const FNarrativeContextData& ContextData) const;
    void ApplyNarrativeMapping(const FNarrativeMusicMapping& Mapping);
    void UpdateNarrativeMusic();
    FString NarrativeTagToString(EAIDMNarrativeTag Tag) const;
    EAIDMNarrativeTag StringToNarrativeTag(const FString& TagString) const;

    // Timer callback
    UFUNCTION()
    void OnNarrativeUpdateTimer();

public:
    /**
     * Blueprint implementable events for custom narrative music logic
     */

    /**
     * Called when narrative tag is triggered (for custom processing)
     * @param NarrativeTag The triggered tag
     * @param ContextData The narrative context
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Music Events")
    void OnNarrativeTagTriggeredEvent(EAIDMNarrativeTag NarrativeTag, const FNarrativeContextData& ContextData);

    /**
     * Called when narrative music changes (for custom effects)
     * @param MappingID ID of the applied mapping
     * @param Mapping The mapping data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Music Events")
    void OnNarrativeMusicChangedEvent(const FString& MappingID, const FNarrativeMusicMapping& Mapping);

    /**
     * Called to customize mapping selection (override in Blueprint)
     * @param NarrativeTag The narrative tag
     * @param ContextData The narrative context
     * @param DefaultMapping Default selected mapping
     * @return Custom mapping selection
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Music Events")
    FNarrativeMusicMapping CustomizeNarrativeMusicMapping(EAIDMNarrativeTag NarrativeTag, 
                                                         const FNarrativeContextData& ContextData,
                                                         const FNarrativeMusicMapping& DefaultMapping);

    /**
     * Called to validate narrative context (override in Blueprint)
     * @param ContextData Context to validate
     * @return True if context is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Narrative Music Events")
    bool ValidateNarrativeContext(const FNarrativeContextData& ContextData);
};
