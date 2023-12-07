// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCMathTypes.h"
#include "RCEngineTypes.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCMathLibrary.generated.h"

UCLASS(DisplayName = "RCUtils Math Library")
class RCRUNTIME_API URCMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/* Returns a small number. */
	UFUNCTION(BlueprintPure, Category = "Math|Float", meta = (CompactNodeTitle = "Small"))
		static float SmallNumber();

	/* Returns a kinda small number. */
	UFUNCTION(BlueprintPure, Category = "Math|Float", meta = (CompactNodeTitle = "Kinda Small"))
		static float KindaSmallNumber();

	/* Returns a big number. */
	UFUNCTION(BlueprintPure, Category = "Math|Float", meta = (CompactNodeTitle = "Big"))
		static float BigNumber();

	/* Gets the distance ignoring Z.
	* @param V1 - First position
	* @param V2 - Second position
	* @return Horizontal Distance between the two positions
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
		static float GetHorizontalDistance(const FVector& V1, const FVector& V2);

	/* Combines two or more Rotators together. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (CommutativeAssociativeBinaryOperator = "true", CompactNodeTitle = "+"))
		static FRotator AddRotators(const FRotator& A, const FRotator& B);

	/* Gets Linear Color from Temperature.
	* @param InTemperature - Temperature
	* @return Linear Color value of the Temperature
	*/
	UFUNCTION(BlueprintPure, Category = "Math|LinearColor")
		static FLinearColor LinearColorFromTemperature(float InTemperature);

	/* Get Date Time from Ticks
	* @param Ticks - Number of ticks since midnight, January 1, 0001
	*/
	UFUNCTION(BlueprintPure, Category = "Math|DateTime", meta = (DisplayName = "From Ticks (Date Time)"))
		static FDateTime GetDateTimeFromTicks(const int64 Ticks);
	
	/* Get Ticks from DateTime 
	* @param DateTime - DateTime to use
	* @return Number of ticks since midnight, January 1, 0001
	*/
	UFUNCTION(BlueprintPure, Category = "Math|DateTime", meta = (DisplayName = "Get Ticks (Date Time)"))
		static int64 GetDateTimeTicks(const FDateTime& DateTime);

	/* Get Timespan from Ticks
	* @param Ticks - Number of ticks since midnight, January 1, 0001
	*/
	UFUNCTION(BlueprintPure, Category = "Math|DateTime", meta = (DisplayName = "From Ticks (Timespan)"))
		static FTimespan GetTimespanFromTicks(const int64 Ticks);

	/* Gets Ticks from FTimespan 
	* @param Timespan - Timespan to use
	* @return Number of ticks represented by this time span
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Timespan", meta = (DisplayName = "Get Ticks (Timespan)"))
		static int64 GetTimespanTicks(const FTimespan& Timespan);

	/* Gets the Min and Max range from Float Range.
	* @param Range - Float range
	* @param Min - The minimum value
	* @param Max - The maximum value
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Float", meta = (DisplayName = "Get Range (Float)"))
		static void GetRangeFloat(const FFloatRange& Range, float& Min, float& Max);

	/* Clamps a Float within a Float Range.
	* @param Range - Float range
	* @param Value - Unclamped value
	* @return Clamped value
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Float", meta = (DisplayName = "Clamp Range (Float)"))
		static float ClampRangeFloat(const FFloatRange& Range, float Value);

	/* Clamps a Vector within a Vector Range.
	* @param Range - Vector range
	* @param Value - Unclamped value
	* @return Clamped value
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (DisplayName = "Clamp Range (Vector)"))
		static FVector ClampRangeVector(const FRCVectorRange& Range, const FVector& Value);

	/* Clamps a Rotator within a Rotator Range.
	* @param Range - Rotator range
	* @param Value - Unclamped value
	* @return Clamped value
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (DisplayName = "Clamp Range (Rotator)"))
		static FRotator ClampRangeRotator(const FRCRotatorRange& Range, const FRotator& Value);
	
	/* Calculates the line trace vectors from a base location.
	* @param Location - Base Location to start the trace from
	* @param Rotation - Rotation to identify the various vectors from.
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector")
		static void GetLineTraceVectors(const FVector& Location, const FRotator& Rotation, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End);

	/* Calculates the line trace vectors from an actor.
	* @param Actor - Actor to get base location from
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (DefaultToSelf = "Actor"))
		static void GetActorLineTraceVectors(const AActor* Actor, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End);

	/* Calculates the line trace vectors from a component.
	* @param Component - Component to get base location from
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (DefaultToSelf = "Component"))
		static void GetComponentLineTraceVectors(const USceneComponent* Component, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End);

	/* Calculates the line trace vectors from the player camera.
	* @param Direction - Direction of the Line Trace
	* @param Distance - Distance of the Line Trace
	* @param Start - Starting Line Trace Point
	* @param End - Ending Line Trace Point
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Vector", meta = (WorldContext = "WorldContextObject"))
		static void GetCameraLineTraceVectors(const UObject* WorldContextObject, const ERCVectorDirection Direction, float Distance, FVector& Start, FVector& End);

	/* Checks if there's any curve data.
	* @param Curve - Runtime Float Curve
	* @return Whether or not the curve contains any data
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Is Valid"))
		static bool IsRuntimeFloatCurveValid(const FRuntimeFloatCurve& Curve);

	/* Gets the External Curve from Runtime Float Curve. Can be null.
	* @param Curve - Runtime Float Curve
	* @return External Curve Object
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get External Curve"))
		static UCurveFloat* GetExternalRuntimeFloatCurve(const FRuntimeFloatCurve& Curve);

	/* Evaluates a Runtime Float Curve.
	* @param Curve - Runtime Float Curve
	* @param InTime - Time to evaluate
	* @return Value of the curve at the given time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Value"))
		static float GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& Curve, float InTime);

	/* Gets the Time Range of a Runtime Float Curve.
	* @param Curve - Runtime Float Curve
	* @param Min - Minimum Time
	* @param Max - Maximum Time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Time Range"))
		static void GetRuntimeFloatCurveTimeRange(const FRuntimeFloatCurve& Curve, float& Min, float& Max);

	/* Gets the Time Range of a Runtime Float Curve.
	* @param Curve - Runtime Float Curve
	* @param Min - Minimum Time
	* @param Max - Maximum Time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Value Range"))
		static void GetRuntimeFloatCurveValueRange(const FRuntimeFloatCurve& Curve, float& Min, float& Max);

	/* Checks if there's any curve data.
	* @param Curve - Runtime Vector Curve
	* @return Whether or not the curve contains any data in any axis
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Is Valid"))
		static bool IsRuntimeVectorCurveValid(const FRuntimeVectorCurve& Curve);

	/* Gets the External Curve from Runtime Vector Curve. Can be null.
	* @param Curve - Runtime Vector Curve
	* @return External Curve Object
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get External Curve"))
		static UCurveVector* GetExternalRuntimeVectorCurve(const FRuntimeVectorCurve& Curve);

	/* Evaluates a Runtime Vector Curve.
	* @param Curve - Runtime Vector Curve
	* @param InTime - Time to evaluate
	* @return Value of the curve at the given time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Value"))
		static FVector GetRuntimeVectorCurveValue(const FRuntimeVectorCurve& Curve, float InTime);

	/* Gets the Time Range of a Runtime Vector Curve.
	* @param Curve - Runtime Vector Curve
	* @param Min - Minimum Time
	* @param Max - Maximum Time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Time Range"))
		static void GetRuntimeVectorCurveTimeRange(const FRuntimeVectorCurve& Curve, float& Min, float& Max);

	/* Gets the Value Range of a Runtime Vector Curve.
	* @param Curve - Runtime Vector Curve
	* @param Min - Minimum Value
	* @param Max - Maximum Value
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Value Range"))
		static void GetRuntimeVectorCurveValueRange(const FRuntimeVectorCurve& Curve, FVector& Min, FVector& Max);

	/* Checks if there's any curve data.
	* @param Curve - Runtime Linear Color Curve
	* @return Whether or not the curve contains any data in any channel
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Is Valid"))
		static bool IsRuntimeLinearColorCurveValid(const FRuntimeLinearColorCurve& Curve);

	/* Gets the External Curve from Runtime Linear Color Curve. Can be null.
	* @param Curve - Runtime Linear Color Curve
	* @return External Curve Object
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get External Curve"))
		static UCurveLinearColor* GetExternalRuntimeLinearColorCurve(const FRuntimeLinearColorCurve& Curve);

	/* Evaluates a Runtime Linear Color Curve.
	* @param Curve - Runtime Linear Color Curve
	* @param InTime - Time to evaluate
	* @return Value of the curve at the given time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Value"))
		static FLinearColor GetRuntimeLinearColorCurveValue(const FRuntimeLinearColorCurve& Curve, float InTime);

	/* Gets the Time Range of a Runtime Linear Color Curve.
	* @param Curve - Runtime Linear Color Curve
	* @param Min - Minimum Time
	* @param Max - Maximum Time
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Time Range"))
		static void GetRuntimeLinearColorCurveTimeRange(const FRuntimeLinearColorCurve& Curve, float& Min, float& Max);

	/* Gets the Value Range of a Runtime Linear Color Curve.
	* @param Curve - Runtime Linear Color Curve
	* @param Min - Minimum Value
	* @param Max - Maximum Value
	*/
	UFUNCTION(BlueprintPure, Category = "Math|Curve|Runtime", meta = (DisplayName = "Get Value Range"))
		static void GetRuntimeLinearColorCurveValueRange(const FRuntimeLinearColorCurve& Curve, FLinearColor& Min, FLinearColor& Max);

	/* Adds all the floats in a float array. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Float", meta = (CompactNodeTitle = "+"))
		static float AddArray_Float(TArray<float> InFloatArray);

	/* Subtracts all the floats in a float array. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Float", meta = (CompactNodeTitle = "-"))
		static float SubtractArray_Float(TArray<float> InFloatArray);

	/* Multiply all the floats in a float array with a base value of 1. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Float", meta = (CompactNodeTitle = "*"))
		static float MultiplyArray_Float(TArray<float> InFloatArray);

	/* Divide all the floats in a float array with a base value of 1. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Float", meta = (CompactNodeTitle = "/"))
		static float DivideArray_Float(TArray<float> InFloatArray);

	/* Adds all the integer in an integer array. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Integer", meta = (CompactNodeTitle = "+"))
		static int AddArray_Int(TArray<int> InIntArray);

	/* Subtracts all the integer in an integer array. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Integer", meta = (CompactNodeTitle = "-"))
		static int SubtractArray_Int(TArray<int> InIntArray);

	/* Multiply all the integer in an integer array with a base value of 1. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Integer", meta = (CompactNodeTitle = "*"))
		static int MultiplyArray_Int(TArray<int> InIntArray);

	/* Divide all the integer in an integer array with a base value of 1. */
	UFUNCTION(BlueprintPure, Category = "Math|Arrays|Integer", meta = (CompactNodeTitle = "/"))
		static int DivideArray_Int(TArray<int> InIntArray);
};
