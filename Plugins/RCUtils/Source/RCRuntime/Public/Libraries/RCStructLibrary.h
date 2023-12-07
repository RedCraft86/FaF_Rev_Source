// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCMathTypes.h"
#include "RCRuntimeTypes.h"
#include "RCLightingTypes.h"
#include "RCPrimitiveTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCStructLibrary.generated.h"

UCLASS(DisplayName = "RCUtils Struct Library")
class RCRUNTIME_API URCStructLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/* Returns true if A is Less than B (A < B) */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "Game Version < Game Version", CompactNodeTitle = "<", Keywords = "< less"))
	static bool Less_GameVersionGameVersion(const FRCGameVersion& A, const FRCGameVersion& B) { return A < B; }
	
	/* Returns true if A is greater than B (A > B) */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "Game Version > Game Version", CompactNodeTitle = ">", Keywords = "> greater"))
	static bool Greater_GameVersionGameVersion(const FRCGameVersion& A, const FRCGameVersion& B) { return A > B; }

	/* Returns true if A is Less than or equal to B (A <= B) */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "Game Version <= Game Version", CompactNodeTitle = "<=", Keywords = "<= less"))
	static bool LessEqual_GameVersionGameVersion(const FRCGameVersion& A, const FRCGameVersion& B) { return A <= B; }

	/* Returns true if A is greater than or equal to B (A >= B) */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "Game Version >= Game Version", CompactNodeTitle = ">=", Keywords = ">= greater"))
	static bool GreaterEqual_GameVersionGameVersion(const FRCGameVersion& A, const FRCGameVersion& B) { return A >= B; }

	/* Returns true if A is exactly equal to B (A == B) */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "Equal (Game Version)", CompactNodeTitle = "==", Keywords = "== equal"))
	static bool EqualEqual_GameVersionGameVersion(const FRCGameVersion& A, const FRCGameVersion& B) { return A == B; }

	/* Returns true if A does not equal B (A != B) */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "Not Equal (Game Version)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
	static bool NotEqual_GameVersionGameVersion(const FRCGameVersion& A, const FRCGameVersion& B) { return A != B; }

	/* Convert a Game Version to string in X.Y.Z INFO format. */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "To String (Game Version)", CompactNodeTitle = "->", BlueprintAutocast))
	static FString GameVersionToString(const FRCGameVersion& InGameVersion) { return InGameVersion.ToString(); }

	/* Convert a string in X.Y.Z INFO format to Game Version. */
	UFUNCTION(BlueprintPure, Category = "GameVersion", meta = (DisplayName = "From String (Game Version)", CompactNodeTitle = "->", BlueprintAutocast))
	static FRCGameVersion GameVersionFromString(const FString InString) { return FRCGameVersion(InString); }

	/* Returns true if A is Less than B (A < B) */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "Currency < Currency", CompactNodeTitle = "<", Keywords = "< less"))
	static bool Less_CurrencyCurrency(const FRCCurrency& A, const FRCCurrency& B) { return A < B; }

	/* Returns true if A is greater than B (A > B) */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "Currency > Currency", CompactNodeTitle = ">", Keywords = "> greater"))
	static bool Greater_CurrencyCurrency(const FRCCurrency& A, const FRCCurrency& B) { return A > B; }

	/* Returns true if A is Less than or equal to B (A <= B) */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "Currency <= Currency", CompactNodeTitle = "<=", Keywords = "<= less"))
	static bool LessEqual_CurrencyCurrency(const FRCCurrency& A, const FRCCurrency& B) { return A <= B; }

	/* Returns true if A is greater than or equal to B (A >= B) */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "Currency >= Currency", CompactNodeTitle = ">=", Keywords = ">= greater"))
	static bool GreaterEqual_CurrencyCurrency(const FRCCurrency& A, const FRCCurrency& B) { return A >= B; }

	/* Returns true if A is exactly equal to B (A == B) */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "Equal (Currency)", CompactNodeTitle = "==", Keywords = "== equal"))
	static bool EqualEqual_CurrencyCurrency(const FRCCurrency& A, const FRCCurrency& B) { return A == B; }

	/* Returns true if A does not equal B (A != B) */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "Not Equal (Currency)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
	static bool NotEqual_CurrencyCurrency(const FRCCurrency& A, const FRCCurrency& B) { return A != B; }

	/* Convert an int64 to currency. Value must be in cents. */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "From int64 (Currency)", CompactNodeTitle = "->", BlueprintAutocast))
	static FRCCurrency CurrencyFromInt64(const int64 InValue) { return FRCCurrency(InValue); }

	/* Convert a Currency to int64 in cents. */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "To int64 (Currency)", CompactNodeTitle = "->", BlueprintAutocast))
	static int64 CurrencyToInt64(const FRCCurrency& InCurrency) { return InCurrency.GetCents(); }

	/* Convert a string to currency. String must be formatted as such: "X.Y" where X and Y are numeric with no additional characters. */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "From String (Currency)", CompactNodeTitle = "->", BlueprintAutocast))
	static FRCCurrency CurrencyFromString(const FString& InString) { return FRCCurrency::FromString(InString); }

	/* Convert a Currency to string in X.Y format. */
	UFUNCTION(BlueprintPure, Category = "Currency", meta = (DisplayName = "To String (Currency)", CompactNodeTitle = "->", BlueprintAutocast))
	static FString CurrencyToString(const FRCCurrency& InCurrency) { return InCurrency.ToString(); }

	/* Adds an amount in cents to a currency. */
	UFUNCTION(BlueprintCallable , Category = "Currency")
	static void AddCurrency(UPARAM(ref) FRCCurrency InCurrency, const int64 InCents) { InCurrency.AddMoney(InCents); }

	/* Removes an amount in cents from a currency. */
	UFUNCTION(BlueprintCallable, Category = "Currency")
	static void RemoveCurrency(UPARAM(ref) FRCCurrency InCurrency, const int64 InCents) { InCurrency.RemoveMoney(InCents); }

	/* Make a FPrimitiveCollision. */
	UFUNCTION(BlueprintPure, Category = "PrimitiveCollision", meta = (DisplayName = "Make Primitive Collision"))
	static FRCPrimitiveCollision MakePrimitiveCollision(const FName& InProfileName, const TEnumAsByte<ECollisionEnabled::Type>& InCollisionEnabled, const TEnumAsByte<ECollisionChannel>& InObjectType, const TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& InResponses)
	{
		if (FRCPrimitiveCollision::IsProfileValid(InProfileName))
			return FRCPrimitiveCollision(InProfileName);

		return FRCPrimitiveCollision(InCollisionEnabled, InObjectType, InResponses);
	}

	/* Break a FPrimitiveCollision. */
	UFUNCTION(BlueprintPure, Category = "PrimitiveCollision", meta = (DisplayName = "Break Primitive Collision"))
	static void BreakPrimitiveCollision(const FRCPrimitiveCollision& InData, FName& ProfileName, TEnumAsByte<ECollisionEnabled::Type>& CollisionEnabled, TEnumAsByte<ECollisionChannel>& ObjectType, TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& Responses)
	{
		ProfileName = InData.CollisionProfileName;
		CollisionEnabled = InData.CollisionEnabled;
		ObjectType = InData.ObjectType;
		Responses = InData.Responses;
	}

	/* Set a FPrimitiveCollision's Profile Name. Only if it is Valid. */
	UFUNCTION(BlueprintCallable, Category = "PrimitiveCollision", meta = (DisplayName = "Set Profile Name"))
	static void SetPrimitiveCollisionProfileName(UPARAM(ref)FRCPrimitiveCollision InData, const FName& InNewProfileName) { InData.SetCollisionProfile(InNewProfileName); }

	/* Get a Debug String from FPrimitiveCollision. */
	UFUNCTION(BlueprintPure, Category = "PrimitiveCollision", meta = (DisplayName = "Get Debug String (Primitive Collision)", CompactNodeTitle = "->", BlueprintAutocast))
	static FString GetPrimitiveCollisionDebugString(const FRCPrimitiveCollision& InData) { return InData.GetDebugString(); }

	/* Fill the materials in static mesh properties. */
	UFUNCTION(BlueprintCallable, Category = "StaticMeshProperties", meta = (DisplayName = "Is Valid"))
	static void UpdateMaterials_StaticMeshProperties(UPARAM(ref)FRCStaticMeshProperties InProperties) { InProperties.FillMaterials(); }
	
	/* Check if static mesh properties is valid. */
	UFUNCTION(BlueprintPure, Category = "StaticMeshProperties", meta = (DisplayName = "Is Valid", CompactNodeTitle = "->", BlueprintAutocast))
	static bool IsValidMesh_StaticMeshProperties(const FRCStaticMeshProperties& InProperties) { return InProperties.IsValidMesh(); }

	/* Multiplies intensity with multiplier. 
	* @param Target - The target to get the intensity from.
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DisplayName = "Get Base Light Properties", CompactNodeTitle = "*", BlueprintAutocast))
	static FRCBaseLightProperties SetIntensityMultiplier(const FRCBaseLightProperties& Target, const float Multiplier)
	{
		FRCBaseLightProperties RetVal = Target;
		RetVal.Intensity *= Multiplier;
		return RetVal;
	}
	
	/* Gets the base light property. 
	* @param Target - The target to get from.
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DisplayName = "Get Base Light Properties", CompactNodeTitle = "->", BlueprintAutocast))
	static FRCBaseLightProperties GetBaseLightPropertiesPoint(const FRCPointLightProperties& Target) { return Target.GetBaseLightProperties(); }

	/* Gets the base light property. 
	* @param Target - The target to get from.
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DisplayName = "Get Base Light Properties", CompactNodeTitle = "->", BlueprintAutocast))
	static FRCBaseLightProperties GetBaseLightPropertiesSpot(const FRCSpotLightProperties& Target) { return Target.GetBaseLightProperties(); }

	/* Gets the base light property. 
	* @param Target - The target to get from.
	*/
	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DisplayName = "Get Base Light Properties", CompactNodeTitle = "->", BlueprintAutocast))
	static FRCBaseLightProperties GetBaseLightPropertiesRect(const FRCRectLightProperties& Target) { return Target.GetBaseLightProperties(); }
	
	/* Copies basic light data such as Intensity, Color and Temperature. 
	* @param Target - The target to set to.
	* @param Source - Source to get the data from.
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DisplayName = "Copy Basic Light Data"))
	static void CopyBasicLightDataPoint(UPARAM(ref)FRCPointLightProperties& Target, const FRCBaseLightProperties& Source) { Target.CopyBasicLightData(Source); } 

	/* Copies basic light data such as Intensity, Color and Temperature. 
	* @param Target - The target to set to.
	* @param Source - Source to get the data from.
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DisplayName = "Copy Basic Light Data"))
	static void CopyBasicLightDataSpot(UPARAM(ref)FRCSpotLightProperties& Target, const FRCBaseLightProperties& Source) { Target.CopyBasicLightData(Source); } 

	/* Copies basic light data such as Intensity, Color and Temperature. 
	* @param Target - The target to set to.
	* @param Source - Source to get the data from.
	*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DisplayName = "Copy Basic Light Data"))
	static void CopyBasicLightDataRect(UPARAM(ref)FRCRectLightProperties& Target, const FRCBaseLightProperties& Source) { Target.CopyBasicLightData(Source); } 
};
