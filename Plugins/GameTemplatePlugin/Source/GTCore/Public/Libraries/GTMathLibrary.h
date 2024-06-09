// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/MathTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTMathLibrary.generated.h"

UCLASS()
class GTCORE_API UGTMathLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/* Returns UE_SMALL_NUMBER */
	UFUNCTION(BlueprintPure, Category = "Math|Constant", meta = (CompactNodeTitle = "Small"))
		static float SmallNumber() { return UE_SMALL_NUMBER; }

	/* Returns UE_KINDA_SMALL_NUMBER */
	UFUNCTION(BlueprintPure, Category = "Math|Constant", meta = (CompactNodeTitle = "Kinda Small"))
		static float KindaSmallNumber() { return UE_KINDA_SMALL_NUMBER; }

	/* Returns UE_BIG_NUMBER */
	UFUNCTION(BlueprintPure, Category = "Math|Constant", meta = (CompactNodeTitle = "Big"))
		static float BigNumber() { return UE_BIG_NUMBER; }

	UFUNCTION(BlueprintPure, Category = "Math|Vector")
		static float GetHorizontalDistance(const FVector A, const FVector B);

	/* Calculates the line trace vectors from a base location.
	* @param Location - Base Location to start the trace from
	* @param Rotation - Rotation to identify the various vectors from.
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
		static void GetLineTraceVectors(const FVector& Location, const FRotator& Rotation,
			const EVectorDirection Direction, const float Distance, FVector& Start, FVector& End);

	/* Calculates the line trace vectors from an actor.
	* @param Actor - Actor to get base location from
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (DefaultToSelf = "Actor"))
		static void GetActorLineTraceVectors(const AActor* Actor, const EVectorDirection Direction,
			const float Distance, FVector& Start, FVector& End);

	/* Calculates the line trace vectors from a component.
	* @param Component - Component to get base location from
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (DefaultToSelf = "Component"))
		static void GetComponentLineTraceVectors(const USceneComponent* Component, const EVectorDirection Direction,
			const float Distance, FVector& Start, FVector& End);

	/* Calculates the line trace vectors from the player camera.
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (WorldContext = "ContextObject"))
		static void GetCameraLineTraceVectors(const UObject* ContextObject, const EVectorDirection Direction,
			const float Distance, FVector& Start, FVector& End);
	
	/* Rotator Addition */
	UFUNCTION(BlueprintPure, Category="Math|Rotator", meta = (DisplayName = "rotator + rotator", CompactNodeTitle = "+", ScriptMethod = "Add", ScriptOperator = "+;+=", Keywords = "+ add plus", CommutativeAssociativeBinaryOperator = "true"))
		static FRotator Add_RotatorRotator(const FRotator A, const FRotator B);
	
	/* Creates a Linear Color from the given Hex code */
	UFUNCTION(BlueprintPure, Category = "Math|LinearColor")
		static FLinearColor LinearColorFromHex(const FString InHex);
	
	/* Creates a Hex code from the given Linear Color */
	UFUNCTION(BlueprintPure, Category = "Math|LinearColor")
		static FString LinearColorToHex(const FLinearColor InColor, const bool IsSRGB);

	/* Creates a Linear Color from Temperature */
	UFUNCTION(BlueprintPure, Category = "Math|LinearColor")
		static FLinearColor LinearColorFromTemperature(const float InTemperature);
	
	/* Creates a random Linear Color */
	UFUNCTION(BlueprintPure, Category = "Math|LinearColor")
		static FLinearColor RandomLinearColor(const bool bTrueRandom, const bool bRandomAlpha);

	/* Creates a Color from the given Hex code */
	UFUNCTION(BlueprintPure, Category = "Math|Color")
		static FColor ColorFromHex(const FString InHex);
	
	/* Creates a Hex code from the given Color */
	UFUNCTION(BlueprintPure, Category = "Math|Color")
		static FString ColorToHex(const FColor InColor);
	
	/* Creates a Color from Temperature */
	UFUNCTION(BlueprintPure, Category = "Math|Color")
		static FColor ColorFromTemperature(const float InTemperature);
	
	/* Creates a random Color */
	UFUNCTION(BlueprintPure, Category = "Math|Color")
		static FColor RandomColor(const bool bTrueRandom, const bool bRandomAlpha);

	/* Get Date Time from Ticks
	* @param Ticks - Number of ticks since midnight, January 1, 0001
	*/
	UFUNCTION(BlueprintPure, Category = "Math|DateTime", meta = (DisplayName = "From Ticks (Date Time)"))
		static FDateTime GetDateTimeFromTicks(const int64 InTicks);
	
	/* Get Ticks from DateTime 
	* @param DateTime - DateTime to use
	* @return Number of ticks since midnight, January 1, 0001
	*/
	UFUNCTION(BlueprintPure, Category = "Math|DateTime", meta = (DisplayName = "Get Ticks (Date Time)"))
		static int64 GetDateTimeTicks(const FDateTime& InDateTime);

	/* Get Timespan from Ticks
	* @param Ticks - Number of ticks since midnight, January 1, 0001
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Timespan", meta = (DisplayName = "From Ticks (Timespan)"))
		static FTimespan GetTimespanFromTicks(const int64 InTicks);

	/* Gets Ticks from FTimespan 
	* @param Timespan - Timespan to use
	* @return Number of ticks represented by this time span
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Timespan", meta = (DisplayName = "Get Ticks (Timespan)"))
		static int64 GetTimespanTicks(const FTimespan& InTimespan);

	// Expose math types

	UFUNCTION(BlueprintPure, Category = "Math|InterpolatableScalar", DisplayName = "Is Complete")
	static bool IntScalar_IsComplete(const FGTInterpScalar& Target)
	{
		return Target.IsComplete();
	}

	UFUNCTION(BlueprintPure, Category = "Math|InterpolatableScalar", DisplayName = "Tick (Interp)")
	static void IntScalar_Tick(UPARAM(ref) FGTInterpScalar& Target, const float DeltaTime)
	{
		return Target.Tick(DeltaTime);
	}

	UFUNCTION(BlueprintPure, Category = "Math|InterpolatableScalar", DisplayName = "Snap To Target")
	static void IntScalar_SnapToTarget(UPARAM(ref) FGTInterpScalar& Target)
	{
		return Target.SnapToTarget();
	}

	UFUNCTION(BlueprintPure, Category = "Math|ModifiableScalar", DisplayName = "Evaluate")
	static float ModScalarMulti_Evaluate(const FGTModifiableMultiplier& Target)
	{
		return Target.Evaluate();
	}

	UFUNCTION(BlueprintPure, Category = "Math|ModifiableScalar", DisplayName = "Evaluate")
	static float ModScalarSum_Evaluate(const FGTModifiableSum& Target)
	{
		return Target.Evaluate();
	}
};
