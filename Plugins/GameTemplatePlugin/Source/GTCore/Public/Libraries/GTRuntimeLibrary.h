// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/MiscTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTRuntimeLibrary.generated.h"

UCLASS()
class GTCORE_API UGTRuntimeLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Windows")
		static EWindowsDialogueResult OpenWindowsDialogue(const FString Title, const FString Message,
			const EWindowsDialogueType Type, const EWindowsDialogueIcon Icon);

	UFUNCTION(BlueprintCallable, Category = "Windows")
		static void TriggerVirtualKey(const FKey InKey, const EVirtualKeyTriggerType TriggerType, const bool bRepeat);

	/* Gets the vertices/corner locations of the actor's bounding box.
	* @param Target - Actor to use.
	* @param bOnlyCollidingComponents - If enabled, only components that can collide will be used in calculation.
	* @param bIncludeFromChildActors - Whether or not child actors should be included in the calculation.
	* @param Origin - Origin of the actor.
	* @param BoxExtent - Bounding box extent of the actor.
	* @return Locations of the 8 (or less) vertices of the bounding box.
	*/
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static TArray<FVector> GetBoundingBoxVertices(const AActor* Target, const bool bOnlyCollidingComponents,
			const bool bIncludeFromChildActors, FVector& Origin, FVector& BoxExtent);

	/* Checks if actor is on the player screen.
	* @param Target - Actor to check.
	* @param MaxDistance - The maximum distance before the actor is considered no longer in sight.
	* @param bOriginOnly - If enabled, the checks will only make use of the actor's origin, bounds will not be used.
	* @param bLineTraceCheck - Draw a line trace from the camera to the target when checking if it's in player view.
	* @param TraceCheckParams - Settings to use when testing the different vertices of the actor's bounding box.
	* @return Whether or not the actor is visible in screen with the given settings.
	*/
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target", AutoCreateRefTerm = "TraceCheckParams"))
	static bool IsActorInScreen(const AActor* Target, const float MaxDistance = 5000.0f, const bool bOriginOnly = false,
		const bool bLineTraceCheck = true, const FActorBoundsCheckParams& TraceCheckParams = FActorBoundsCheckParams());

	/* Checks if a Location is in front of the Target. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static bool IsLocationInFront(const AActor* Target, const FVector Location);

	/* Checks if an Actor is in front of the Target. */
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static bool IsActorInFront(const AActor* Target, const AActor* ActorToTest);

	/* Gets the active Game World. A way to get world context from anywhere but can be unreliable. */
	UFUNCTION(BlueprintPure, Category = "World")
		static UWorld* GetPlayWorld();
	
	/* Restarts the current Level. Returns the Level Name. */
	UFUNCTION(BlueprintCallable, Category = "World", meta = (WorldContext = "ContextObject", AdvancedDisplay = "Options"))
		static void RestartLevel(const UObject* ContextObject, const FString Options = TEXT(""));

	/* Calls an event by name in the Target object. */
	UFUNCTION(BlueprintCallable, Category = "Events", meta = (DefaultToSelf = "Target"))
		static void CallLocalEvent(UObject* Target, const FName EventName);
};
