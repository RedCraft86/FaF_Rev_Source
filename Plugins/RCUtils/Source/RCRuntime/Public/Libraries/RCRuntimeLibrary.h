// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

// ReSharper disable CppPassValueParameterByConstReference
#pragma once

#include "RCEngineTypes.h"
#include "RCLightingTypes.h"
#include "RCPrimitiveTypes.h"
#include "RCRuntimeTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCRuntimeLibrary.generated.h"

class URectLightComponent;
class USpotLightComponent;
class UPointLightComponent;

DECLARE_DYNAMIC_DELEGATE(FOnSequencerFinished);

UCLASS(DisplayName = "RCUtils Runtime Library")
class RCRUNTIME_API URCRuntimeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/* Sets the primitive component's collision data. 
	* @param Target - Target to set the data to
	* @param CollisionData - New Primitive Collision
	*/
	UFUNCTION(BlueprintCallable, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
		static void SetPrimitiveCollision(UPrimitiveComponent* Target, const FRCPrimitiveCollision& CollisionData);

	/* Gets the primitive component's collision data. 
	* @param Target - Target to get the data from
	* @return Primitive Collision
	*/
	UFUNCTION(BlueprintPure, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
		static FRCPrimitiveCollision GetPrimitiveCollisions(const UPrimitiveComponent* Target);

	/* Resets the static mesh component's materials to the mesh defaults. 
	* @param Target - Target to reset materials
	*/
	UFUNCTION(BlueprintCallable, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
		static void ResetStaticMeshMaterials(UStaticMeshComponent* Target);

	/* Sets the static mesh component's mesh, material and shadows. 
	* @param Target - Target to set the data to
	* @param Properties - New Static Mesh Properties
	*/
	UFUNCTION(BlueprintCallable, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
		static void SetStaticMeshProperties(UStaticMeshComponent* Target, const FRCStaticMeshProperties& Properties);

	/* Gets the static mesh component's mesh, material and shadows. 
	* @param Target - Target to get the data from
	* @return Static Mesh Properties
	*/
	UFUNCTION(BlueprintPure, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
		static FRCStaticMeshProperties GetStaticMeshProperties(const UStaticMeshComponent* Target);
	
	/* Sets the draw distance and distance fade range of a light component. 
	* @param Target - The target light to set the settings to
	* @param Settings - Draw Distance Settings
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetLightDrawDistanceSettings(ULightComponent* Target, const FRCLightDrawDistance& Settings);

	/* Gets the draw distance and distance fade range of a light component. 
	* @param Target - The target light to get the settings from
	* @return Draw Distance Settings
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FRCLightDrawDistance GetLightDrawDistanceSettings(const ULightComponent* Target);

	/* Sets the properties of a local light component. 
	* @param Target - The target light to set the properties to
	* @param Settings - Base Light Properties
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetBaseLightProperties(ULightComponent* Target, const FRCBaseLightProperties& Properties);

	/* Sets the properties of a local light component. 
	* @param Target - The target light to get the properties from
	* @return Base Light Properties
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FRCBaseLightProperties GetBaseLightProperties(const ULightComponent* Target);

	/* Sets the properties of a point light component. 
	* @param Target - The target light to set the properties to
	* @param Settings - Point Light Properties
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetPointLightProperties(UPointLightComponent* Target, const FRCPointLightProperties& Properties);

	/* Sets the properties of a point light component. 
	* @param Target - The target light to get the properties from
	* @return Point Light Properties
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FRCPointLightProperties GetPointLightProperties(const UPointLightComponent* Target);

	/* Sets the properties of a spot light component. 
	* @param Target - The target light to set the properties to
	* @param Settings - Spot Light Properties	
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetSpotLightProperties(USpotLightComponent* Target, const FRCSpotLightProperties& Properties);

	/* Sets the properties of a spot light component. 
	* @param Target - The target light to get the properties from
	* @return Spot Light Properties	
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FRCSpotLightProperties GetSpotLightProperties(const USpotLightComponent* Target);

	/* Sets the properties of a rect light component. 
	* @param Target - The target light to set the properties to
	* @param Settings - Rect Light Properties
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetRectLightProperties(URectLightComponent* Target, const FRCRectLightProperties& Properties);
	
	/* Sets the properties of a rect light component. 
	* @param Target - The target light to get the properties from
	* @return Rect Light Properties
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FRCRectLightProperties GetRectLightProperties(const URectLightComponent* Target);

	/* Does several line traces to check the number of blocking objects between Start and End
	* @param Start - Start of traces
	* @param End - End of traces
	* @param TraceChannel - Channel to trace to
	* @param MaxNumOfWalls - Maximum number of walls before finising task
	* @return Number of walls between Start and End
	*/
	UFUNCTION(BlueprintCallable, Category = "Collision", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore"))
		static int32 TraceNumberOfWalls(const UObject* WorldContextObject, const FVector Start, const FVector End, ECollisionChannel TraceChannel, const TArray<AActor*>& ActorsToIgnore, const int MaxNumOfWalls = 10);
	
	/* Simulates a keyboard press or a mouse click.
	* @param bRepeat - If this is a keyboard key pressing, if the key is already pressed down, should it repeat the press?
	* @param bDoubleClick - If this is a mouse button pressing, should it do a double click?
	*/
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (AdvancedDisplay = "bRepeat, bDoubleClick"))
		static void TriggerVirtualKey(const FKey& InKey, const ERCKeyTriggerType TriggerType, const bool bRepeat = false, const bool bDoubleClick = false);

	/* Re-open the current level (to restart) */
	UFUNCTION(BlueprintCallable, Category = "Game", meta = (WorldContext = "WorldContextObject"))
		static void RestartLevel(const UObject* WorldContextObject);

	/* Plays an Ambient Sound actor.
	* @param Target - Ambient Sound actor to play
	* @param StartTime - Time to start the sound from
	*/
	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (DefaultToSelf = "Target"))
		static void PlayAmbientAudio(class AAmbientSound* Target, const float StartTime);

	/* Plays a Level Sequence actor
	* @param Target - Level Sequence actor to play
	* @param StartTime - Time to start the sound from
	*/
	UFUNCTION(BlueprintCallable, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static void PlayLevelSequence(class ALevelSequenceActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static void OnLevelSequenceFinished(class ALevelSequenceActor* Target, const FOnSequencerFinished& Callback);

	UFUNCTION(BlueprintCallable, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static void ActorAddTag(AActor* Target, FName InTag);
	
	UFUNCTION(BlueprintCallable, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static void ActorDeleteTag(AActor* Target, FName InTag);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|MaterialSprite", meta = (DefaultToSelf = "Target"))
		static void SetBillboardElement(UMaterialBillboardComponent* Target, const int32 Index, UMaterialInterface* Material, const bool bSizeIsInScreenSpace, const float BaseSizeX, const float BaseSizeY);
	
	/* Gets the vertices/corner locations of the actor's bounding box. 
	 @param Target - Actor to use
	 @param bOnlyCollidingComponents - If enabled, only components that can collide will be used in calculation
	 @param bIncludeFromChildActors - Whether or not child actors should be included in the calculation
	 @param Origin - Origin of the actor
	 @param BoxExtent - Bounding box extent of the actor
	 @return Locations of the 8 vertices of the bounding box
	*/
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static TArray<FVector> GetBoundingBoxVertices(const AActor* Target, const bool bOnlyCollidingComponents, const bool bIncludeFromChildActors, FVector& Origin, FVector& BoxExtent);

	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static bool IsLocationInFront(const AActor* Target, const FVector Location);
	
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static bool IsActorInFront(const AActor* Target, const AActor* ActorToTest);

	/* Checks if actor is on the player screen
	 @param Target - Actor to check
	 @param MaxDistance - The maximum distance before the actor is considered no longer in sight
	 @param bOriginOnly - If enabled, the checks will only make use of the actor's origin, bounds will not be used
	 @param bLineTraceCheck - Draw a line trace from the camera to the target when checking if it's in player view
	 @param TraceCheckParams - Settings to use when testing the different vertices of the actor's bounding box
	 @return Whether or not the actor is visible in screen with the given settings
	*/
	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target", AutoCreateRefTerm = "TraceCheckParams"))
		static bool IsActorInScreen(const AActor* Target, const float MaxDistance = 5000.0f, const bool bOriginOnly = false, const bool bLineTraceCheck = true, const FActorBoundsCheckParams& TraceCheckParams = FActorBoundsCheckParams());

	UFUNCTION(BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Target"))
		static float GetDistanceFromCamera(const AActor* Target, const bool bIncludeZ = true);
	
	/* Get a String from FTimerHandle. */
	UFUNCTION(BlueprintPure, Category = "Utilities|Time", meta = (CompactNodeTitle = "ToString", BlueprintAutocast))
		static FString GetTimerHandleAsString(const FTimerHandle& Handle) { return Handle.IsValid() ? Handle.ToString() : TEXT(""); }

	/* Checks if a location overlaps a volume. */
	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (DefaultToSelf = "Target"))
		static bool LocationEncompassesPoint(const AVolume* Target, const FVector Location) { return Target ? Target->EncompassesPoint(Location) : false; }
	
	/* Gets the object's name. */
	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (DefaultToSelf = "Target"))
		static FName GetObjectFName(const UObject* Target) { return Target ? Target->GetFName() : NAME_None; }

	/* Gets the object's owning world. */
	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (DefaultToSelf = "Target"))
		static UWorld* GetObjectWorld(const UObject* Target) { return Target ? Target->GetWorld() : nullptr; }

	/* Gets the current play world from GEngine that can be used as world context. Can be null. */
	UFUNCTION(BlueprintPure, Category = "Utilities")
		static UWorld* GetCurrentPlayWorld() { return GEngine ? GEngine->GetCurrentPlayWorld() : nullptr; }

	UFUNCTION(BlueprintPure, Category = "Utilities")
		static FString GetLoginUserID() { return FPlatformMisc::GetLoginId(); }
	
	/* A node that returns a transient package. */
	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (CompactNodeTitle = "Transient"))
		static UObject* GetTransientObject() { return GetTransientPackage(); }

	/* A node that simply returns nullptr. */
	UFUNCTION(BlueprintPure, Category = "Utilities", meta = (CompactNodeTitle = "nullptr"))
		static UObject* GetNullptr() { return nullptr; }
	
	static EMouseButtons::Type KeyAsMouseButton(const FKey& InKey);
	static void GetKeyCode(const FKey& InKey, int32& KeyCode, int32& CharCode);
};
