// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "LiveCodingManager.generated.h"

/**
 * Live coding events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLiveCodingStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLiveCodingCompleted, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLiveCodingFailed, const FString&, ErrorMessage);

/**
 * Live Coding Manager - Manages hot reload and live coding functionality
 */
UCLASS(BlueprintType)
class KOTOR_CLONE_API ULiveCodingManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Enable live coding
     * @param bEnabled Whether to enable live coding
     */
    UFUNCTION(BlueprintCallable, Category = "Live Coding")
    void SetLiveCodingEnabled(bool bEnabled);

    /**
     * Check if live coding is enabled
     * @return True if live coding is enabled
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Live Coding")
    bool IsLiveCodingEnabled() const;

    /**
     * Trigger live coding compile
     * @return True if compile was triggered successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Live Coding")
    bool TriggerLiveCodingCompile();

    /**
     * Set auto-start live coding
     * @param bAutoStart Whether to auto-start live coding
     */
    UFUNCTION(BlueprintCallable, Category = "Live Coding")
    void SetAutoStartLiveCoding(bool bAutoStart);

    /**
     * Get live coding status
     * @return Current live coding status
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Live Coding")
    FString GetLiveCodingStatus() const;

    /**
     * Force hot reload
     * @return True if hot reload was triggered
     */
    UFUNCTION(BlueprintCallable, Category = "Live Coding")
    bool ForceHotReload();

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Live Coding Events")
    FOnLiveCodingStarted OnLiveCodingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Live Coding Events")
    FOnLiveCodingCompleted OnLiveCodingCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Live Coding Events")
    FOnLiveCodingFailed OnLiveCodingFailed;

protected:
    // Live coding settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Coding Settings")
    bool bLiveCodingEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Coding Settings")
    bool bAutoStartLiveCoding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Coding Settings")
    bool bShowLiveCodingConsole;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Coding Settings")
    float CompileTimeout; // Timeout for compilation in seconds

private:
    // Helper methods
    void SetupLiveCodingSettings();
    void OnLiveCodingCompileStarted();
    void OnLiveCodingCompileFinished(bool bSuccess);

public:
    /**
     * Blueprint implementable events for custom live coding logic
     */

    /**
     * Called when live coding starts (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Live Coding Events")
    void OnLiveCodingStartedEvent();

    /**
     * Called when live coding completes (for custom processing)
     * @param bSuccess Whether compilation was successful
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Live Coding Events")
    void OnLiveCodingCompletedEvent(bool bSuccess);

    /**
     * Called when live coding fails (for custom error handling)
     * @param ErrorMessage Error message from compilation
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Live Coding Events")
    void OnLiveCodingFailedEvent(const FString& ErrorMessage);
};

// Live coding macros for conditional compilation
#if WITH_LIVE_CODING && KOTOR_FAST_BUILD
    #define KOTOR_LIVE_CODING_ENABLED 1
    #define KOTOR_LIVE_CODING_FUNCTION(FunctionName) \
        UFUNCTION(BlueprintCallable, Category = "Live Coding", meta = (CallInEditor = "true")) \
        void FunctionName()

    #define KOTOR_LIVE_CODING_PROPERTY(PropertyType, PropertyName) \
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Coding", meta = (AllowPrivateAccess = "true")) \
        PropertyType PropertyName

    #define KOTOR_LIVE_CODING_LOG(LogCategory, Verbosity, Format, ...) \
        UE_LOG(LogCategory, Verbosity, TEXT("[LIVE CODING] ") Format, ##__VA_ARGS__)
#else
    #define KOTOR_LIVE_CODING_ENABLED 0
    #define KOTOR_LIVE_CODING_FUNCTION(FunctionName) \
        void FunctionName()

    #define KOTOR_LIVE_CODING_PROPERTY(PropertyType, PropertyName) \
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Coding") \
        PropertyType PropertyName

    #define KOTOR_LIVE_CODING_LOG(LogCategory, Verbosity, Format, ...)
#endif

// Live coding helper macros
#define KOTOR_MARK_FOR_LIVE_CODING() \
    UFUNCTION(BlueprintCallable, Category = "Live Coding", meta = (CallInEditor = "true")) \
    void MarkForLiveCoding() { /* This function helps with live coding detection */ }

#define KOTOR_LIVE_CODING_CHECKPOINT(CheckpointName) \
    KOTOR_LIVE_CODING_LOG(LogTemp, Warning, TEXT("Live Coding Checkpoint: %s"), TEXT(#CheckpointName))

// Development-only live coding features
#if KOTOR_DEVELOPMENT_BUILD
    #define KOTOR_DEV_LIVE_CODING_FUNCTION(FunctionName) \
        UFUNCTION(BlueprintCallable, Category = "Dev Live Coding", meta = (CallInEditor = "true", DevelopmentOnly = "true")) \
        void FunctionName()

    #define KOTOR_DEV_LIVE_CODING_PROPERTY(PropertyType, PropertyName) \
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dev Live Coding", meta = (DevelopmentOnly = "true")) \
        PropertyType PropertyName
#else
    #define KOTOR_DEV_LIVE_CODING_FUNCTION(FunctionName) \
        void FunctionName()

    #define KOTOR_DEV_LIVE_CODING_PROPERTY(PropertyType, PropertyName) \
        PropertyType PropertyName
#endif
