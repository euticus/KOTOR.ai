// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimMontage.h"
#include "Animation/ProceduralPerformanceComponentV2.h"
#include "AnimationTagMap.generated.h"

/**
 * Animation tag entry
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FAnimationTagEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    FString AnimationTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    UAnimMontage* AnimationMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    float BlendInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    float BlendOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    float PlayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    bool bLooping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    int32 Priority; // Higher priority overrides lower

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    TArray<FString> RequiredTags; // Tags that must be present

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Tag Entry")
    TArray<FString> ExcludedTags; // Tags that must not be present

    FAnimationTagEntry()
    {
        AnimationTag = TEXT("");
        AnimationMontage = nullptr;
        BlendInTime = 0.25f;
        BlendOutTime = 0.25f;
        PlayRate = 1.0f;
        bLooping = false;
        Priority = 1;
    }
};

/**
 * Emotion-tone mapping
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FEmotionToneMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion Tone Mapping")
    EPerformanceEmotion Emotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion Tone Mapping")
    EPerformanceTone Tone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion Tone Mapping")
    FString ResultingTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion Tone Mapping")
    float IntensityMultiplier; // Multiplier for intensity

    FEmotionToneMapping()
    {
        Emotion = EPerformanceEmotion::Neutral;
        Tone = EPerformanceTone::Normal;
        ResultingTag = TEXT("neutral_normal");
        IntensityMultiplier = 1.0f;
    }
};

/**
 * Fallback animation data
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FFallbackAnimationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback Animation")
    UAnimMontage* DefaultIdleMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback Animation")
    UAnimMontage* DefaultTalkingMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback Animation")
    UAnimMontage* DefaultEmotionalMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback Animation")
    float DefaultBlendTime;

    FFallbackAnimationData()
    {
        DefaultIdleMontage = nullptr;
        DefaultTalkingMontage = nullptr;
        DefaultEmotionalMontage = nullptr;
        DefaultBlendTime = 0.25f;
    }
};

/**
 * Animation Tag Map - Maps emotion + tone combinations to AnimMontages
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UAnimationTagMap : public UDataAsset
{
    GENERATED_BODY()

public:
    UAnimationTagMap();

    /**
     * Get animation montage for emotion and tone
     * @param Emotion Performance emotion
     * @param Tone Performance tone
     * @param IntensityLevel Intensity level (0.0 to 1.0)
     * @return Animation tag entry for the combination
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    FAnimationTagEntry GetAnimationForEmotionTone(EPerformanceEmotion Emotion, EPerformanceTone Tone, float IntensityLevel = 0.5f) const;

    /**
     * Get animation montage by tag
     * @param AnimationTag Tag to search for
     * @return Animation tag entry for the tag
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    FAnimationTagEntry GetAnimationByTag(const FString& AnimationTag) const;

    /**
     * Add animation tag entry
     * @param TagEntry Tag entry to add
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    void AddAnimationTagEntry(const FAnimationTagEntry& TagEntry);

    /**
     * Remove animation tag entry
     * @param AnimationTag Tag to remove
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    void RemoveAnimationTagEntry(const FString& AnimationTag);

    /**
     * Load from JSON file
     * @param FilePath Path to JSON file
     * @return True if loaded successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    bool LoadFromJSON(const FString& FilePath);

    /**
     * Save to JSON file
     * @param FilePath Path to save JSON file
     * @return True if saved successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    bool SaveToJSON(const FString& FilePath) const;

    /**
     * Generate tag from emotion and tone
     * @param Emotion Performance emotion
     * @param Tone Performance tone
     * @return Generated animation tag
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Tag Map")
    FString GenerateTag(EPerformanceEmotion Emotion, EPerformanceTone Tone) const;

    /**
     * Get all available tags
     * @return Array of all animation tags
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Tag Map")
    TArray<FString> GetAllTags() const;

    /**
     * Check if tag exists
     * @param AnimationTag Tag to check
     * @return True if tag exists
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Tag Map")
    bool HasTag(const FString& AnimationTag) const;

    /**
     * Get fallback animation
     * @param FallbackType Type of fallback needed
     * @return Fallback animation montage
     */
    UFUNCTION(BlueprintCallable, Category = "Animation Tag Map")
    UAnimMontage* GetFallbackAnimation(const FString& FallbackType) const;

protected:
    // Animation tag entries
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mappings")
    TArray<FAnimationTagEntry> AnimationTagEntries;

    // Emotion-tone mappings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion Tone Mappings")
    TArray<FEmotionToneMapping> EmotionToneMappings;

    // Fallback animations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback Animations")
    FFallbackAnimationData FallbackAnimations;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Map Settings")
    bool bUsePrioritySystem; // Whether to use priority for tag selection

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Map Settings")
    bool bAllowFallbacks; // Whether to use fallback animations

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Map Settings")
    float IntensityThreshold; // Threshold for high intensity animations

private:
    // Helper methods
    void InitializeDefaultMappings();
    FString EmotionToString(EPerformanceEmotion Emotion) const;
    FString ToneToString(EPerformanceTone Tone) const;
    EPerformanceEmotion StringToEmotion(const FString& EmotionString) const;
    EPerformanceTone StringToTone(const FString& ToneString) const;
    FAnimationTagEntry* FindBestMatch(const TArray<FAnimationTagEntry*>& Candidates, float IntensityLevel) const;
    bool MatchesRequirements(const FAnimationTagEntry& Entry, const TArray<FString>& AvailableTags) const;

public:
    /**
     * Blueprint implementable events for custom tag map logic
     */

    /**
     * Called to customize tag generation (override in Blueprint)
     * @param Emotion Performance emotion
     * @param Tone Performance tone
     * @param IntensityLevel Intensity level
     * @return Custom animation tag
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Tag Map Events")
    FString CustomizeTagGeneration(EPerformanceEmotion Emotion, EPerformanceTone Tone, float IntensityLevel);

    /**
     * Called to validate animation selection (override in Blueprint)
     * @param TagEntry Selected tag entry
     * @param Emotion Original emotion
     * @param Tone Original tone
     * @return True if selection is valid
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Tag Map Events")
    bool ValidateAnimationSelection(const FAnimationTagEntry& TagEntry, EPerformanceEmotion Emotion, EPerformanceTone Tone);

    /**
     * Called when fallback animation is used (override in Blueprint)
     * @param RequestedTag Tag that was requested
     * @param FallbackType Type of fallback used
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Tag Map Events")
    void OnFallbackAnimationUsed(const FString& RequestedTag, const FString& FallbackType);

#if WITH_EDITOR
    // Editor-only functions for asset management
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    void ValidateAsset();
#endif
};
