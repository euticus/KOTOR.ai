// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "AIDM/CampaignLoaderSubsystem.h"
#include "AIDM/AIDirectorComponent.h"
#include "AIDM/QuestManagerComponent.h"
#include "GalaxyMapWidget.generated.h"

/**
 * Planet status on the galaxy map
 */
UENUM(BlueprintType)
enum class EPlanetStatus : uint8
{
    Locked          UMETA(DisplayName = "Locked"),
    Available       UMETA(DisplayName = "Available"),
    Current         UMETA(DisplayName = "Current"),
    Completed       UMETA(DisplayName = "Completed"),
    Optional        UMETA(DisplayName = "Optional")
};

/**
 * Planet map data for UI display
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FPlanetMapData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    FPlanetData PlanetData;

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    EPlanetStatus Status;

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    FVector2D MapPosition; // Position on the galaxy map

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    float CompletionPercentage; // 0.0 to 1.0

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    int32 ActiveQuests; // Number of active quests on this planet

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    int32 CompletedQuests; // Number of completed quests

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    bool bHasMainQuest; // Whether this planet has main quest content

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    TArray<FString> UnlockedLayouts; // Layouts unlocked on this planet

    UPROPERTY(BlueprintReadWrite, Category = "Planet Map")
    FString LastVisitedLayout; // Last layout visited on this planet

    FPlanetMapData()
    {
        Status = EPlanetStatus::Locked;
        MapPosition = FVector2D::ZeroVector;
        CompletionPercentage = 0.0f;
        ActiveQuests = 0;
        CompletedQuests = 0;
        bHasMainQuest = false;
        LastVisitedLayout = TEXT("");
    }
};

/**
 * Travel route between planets
 */
USTRUCT(BlueprintType)
struct KOTOR_CLONE_API FTravelRoute
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Travel Route")
    int32 FromPlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Travel Route")
    int32 ToPlanetIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Travel Route")
    bool bIsUnlocked;

    UPROPERTY(BlueprintReadWrite, Category = "Travel Route")
    float TravelTime; // In-game time to travel (seconds)

    UPROPERTY(BlueprintReadWrite, Category = "Travel Route")
    FString RequiredQuest; // Quest that must be completed to unlock this route

    FTravelRoute()
    {
        FromPlanetIndex = -1;
        ToPlanetIndex = -1;
        bIsUnlocked = false;
        TravelTime = 5.0f;
        RequiredQuest = TEXT("");
    }
};

/**
 * Galaxy map events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlanetSelected, int32, PlanetIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTravelInitiated, int32, FromPlanet, int32, ToPlanet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelCompleted, int32, DestinationPlanet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGalaxyMapOpened, const TArray<FPlanetMapData>&, PlanetData);

/**
 * Galaxy Map Widget - Visual galaxy map for planet travel and progress tracking
 */
UCLASS(BlueprintType, Blueprintable)
class KOTOR_CLONE_API UGalaxyMapWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UGalaxyMapWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /**
     * Initialize galaxy map with campaign data
     * @param CampaignLoader Campaign loader for planet data
     * @param AIDirector AI Director for current state
     * @param QuestManager Quest manager for quest tracking
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void InitializeGalaxyMap(UCampaignLoaderSubsystem* CampaignLoader, 
                            UAIDirectorComponent* AIDirector, 
                            UQuestManagerComponent* QuestManager);

    /**
     * Open the galaxy map
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void OpenGalaxyMap();

    /**
     * Close the galaxy map
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void CloseGalaxyMap();

    /**
     * Select a planet on the map
     * @param PlanetIndex Index of the planet to select
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void SelectPlanet(int32 PlanetIndex);

    /**
     * Initiate travel to selected planet
     * @param DestinationPlanet Index of destination planet
     * @return True if travel was initiated
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    bool InitiateTravel(int32 DestinationPlanet);

    /**
     * Check if travel to planet is available
     * @param PlanetIndex Index of the planet
     * @return True if travel is available
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Map")
    bool IsTravelAvailable(int32 PlanetIndex) const;

    /**
     * Get planet map data
     * @param PlanetIndex Index of the planet
     * @return Planet map data
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Map")
    FPlanetMapData GetPlanetMapData(int32 PlanetIndex) const;

    /**
     * Update planet status and progress
     * @param PlanetIndex Index of the planet to update
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void UpdatePlanetProgress(int32 PlanetIndex);

    /**
     * Unlock a planet for travel
     * @param PlanetIndex Index of the planet to unlock
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void UnlockPlanet(int32 PlanetIndex);

    /**
     * Unlock a travel route between planets
     * @param FromPlanet Source planet index
     * @param ToPlanet Destination planet index
     */
    UFUNCTION(BlueprintCallable, Category = "Galaxy Map")
    void UnlockTravelRoute(int32 FromPlanet, int32 ToPlanet);

    /**
     * Get current planet index
     * @return Index of current planet
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Map")
    int32 GetCurrentPlanetIndex() const { return CurrentPlanetIndex; }

    /**
     * Get selected planet index
     * @return Index of selected planet
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Map")
    int32 GetSelectedPlanetIndex() const { return SelectedPlanetIndex; }

    /**
     * Check if currently traveling
     * @return True if travel is in progress
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Map")
    bool IsTraveling() const { return bIsTraveling; }

    /**
     * Get travel progress (0.0 to 1.0)
     * @return Current travel progress
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Galaxy Map")
    float GetTravelProgress() const;

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Galaxy Map Events")
    FOnPlanetSelected OnPlanetSelected;

    UPROPERTY(BlueprintAssignable, Category = "Galaxy Map Events")
    FOnTravelInitiated OnTravelInitiated;

    UPROPERTY(BlueprintAssignable, Category = "Galaxy Map Events")
    FOnTravelCompleted OnTravelCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Galaxy Map Events")
    FOnGalaxyMapOpened OnGalaxyMapOpened;

protected:
    // UI Components (to be bound in Blueprint)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UCanvasPanel* GalaxyMapCanvas;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UImage* GalaxyBackgroundImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UCanvasPanel* PlanetContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UCanvasPanel* RouteContainer;

    // Planet Info Panel
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* PlanetNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* PlanetDescriptionText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* PlanetStatusText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UProgressBar* PlanetProgressBar;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* QuestCountText;

    // Travel Controls
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UButton* TravelButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UButton* CloseMapButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UProgressBar* TravelProgressBar;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UI")
    class UTextBlock* TravelStatusText;

    // Planet and route data
    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    TArray<FPlanetMapData> PlanetMapData;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    TArray<FTravelRoute> TravelRoutes;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    int32 CurrentPlanetIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    int32 SelectedPlanetIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    bool bIsTraveling;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    float TravelStartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    float TravelDuration;

    UPROPERTY(BlueprintReadOnly, Category = "Galaxy Map")
    int32 TravelDestination;

    // Component references
    UPROPERTY()
    UCampaignLoaderSubsystem* CampaignLoaderRef;

    UPROPERTY()
    UAIDirectorComponent* AIDirectorRef;

    UPROPERTY()
    UQuestManagerComponent* QuestManagerRef;

    // UI settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Galaxy Map Settings")
    TSubclassOf<UUserWidget> PlanetButtonWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Galaxy Map Settings")
    TSubclassOf<UUserWidget> RouteLineWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Galaxy Map Settings")
    float MapScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Galaxy Map Settings")
    FVector2D MapCenter;

    // Planet button widgets
    UPROPERTY()
    TArray<UUserWidget*> PlanetButtons;

    // Route line widgets
    UPROPERTY()
    TArray<UUserWidget*> RouteLines;

private:
    // Helper methods
    void RefreshGalaxyMap();
    void CreatePlanetButtons();
    void CreateRouteLines();
    void UpdatePlanetButton(int32 PlanetIndex);
    void UpdateRouteLines();
    void UpdatePlanetInfoPanel();
    void UpdateTravelControls();
    void CompleteTravelSequence();
    FVector2D CalculatePlanetPosition(int32 PlanetIndex) const;
    EPlanetStatus CalculatePlanetStatus(int32 PlanetIndex) const;
    float CalculatePlanetCompletion(int32 PlanetIndex) const;
    bool IsRouteUnlocked(int32 FromPlanet, int32 ToPlanet) const;

    // Button event handlers
    UFUNCTION()
    void OnTravelButtonClicked();

    UFUNCTION()
    void OnCloseMapButtonClicked();

    UFUNCTION()
    void OnPlanetButtonClicked(int32 PlanetIndex);

public:
    /**
     * Blueprint implementable events for custom galaxy map logic
     */

    /**
     * Called when galaxy map is opened (for custom setup)
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Map Events")
    void OnGalaxyMapOpenedEvent();

    /**
     * Called when a planet is selected (for custom logic)
     * @param PlanetIndex The selected planet index
     * @param PlanetData The planet data
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Map Events")
    void OnPlanetSelectedEvent(int32 PlanetIndex, const FPlanetMapData& PlanetData);

    /**
     * Called when travel is initiated (for custom effects)
     * @param FromPlanet Source planet
     * @param ToPlanet Destination planet
     * @param TravelTime Duration of travel
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Map Events")
    void OnTravelInitiatedEvent(int32 FromPlanet, int32 ToPlanet, float TravelTime);

    /**
     * Called to create custom planet button (override in Blueprint)
     * @param PlanetIndex Index of the planet
     * @param PlanetData Planet map data
     * @return Custom planet button widget
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Map Events")
    UUserWidget* CreateCustomPlanetButton(int32 PlanetIndex, const FPlanetMapData& PlanetData);

    /**
     * Called to create custom route line (override in Blueprint)
     * @param Route The travel route data
     * @return Custom route line widget
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Map Events")
    UUserWidget* CreateCustomRouteLine(const FTravelRoute& Route);

    /**
     * Called to get custom planet position (override in Blueprint)
     * @param PlanetIndex Index of the planet
     * @param PlanetData Planet data
     * @return Custom position on the map
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Galaxy Map Events")
    FVector2D GetCustomPlanetPosition(int32 PlanetIndex, const FPlanetData& PlanetData);
};
