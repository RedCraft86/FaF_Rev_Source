// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveLinearColor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InlineCurves.generated.h"

USTRUCT(BlueprintType)
struct GTCORE_API FInlineFloatCurve
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "InlineFloatCurve", meta = (ShowOnlyInnerProperties))
		FRuntimeFloatCurve Curve;

	FRichCurve* GetRichCurve();
	const FRichCurve* GetRichCurveConst() const;
	bool HasAnyData() const;
	UCurveFloat* GetCurveAsset() const;
	float GetValue(float InTime) const;
	void GetTimeRange(float& Min, float& Max) const;
	void GetTimeRange(double& Min, double& Max) const;
	void GetValueRange(float& Min, float& Max) const;
	void GetValueRange(double& Min, double& Max) const;
};

USTRUCT(BlueprintType)
struct GTCORE_API FInlineVectorCurve
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "InlineVectorCurve", meta = (ShowOnlyInnerProperties))
		FRuntimeVectorCurve Curve;

	FRichCurve* GetRichCurve(int32 Index);
	const FRichCurve* GetRichCurveConst(int32 Index) const;
	bool HasAnyData() const;
	UCurveVector* GetCurveAsset() const;
	FVector GetValue(float InTime) const;
	void GetTimeRange(float& Min, float& Max) const;
	void GetTimeRange(double& Min, double& Max) const;
	void GetValueRange(FVector& Min, FVector& Max) const;
};

USTRUCT(BlueprintType)
struct GTCORE_API FInlineColorCurve
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "InlineColorCurve", meta = (ShowOnlyInnerProperties))
		FRuntimeCurveLinearColor Curve;

	FRichCurve* GetRichCurve(int32 Index);
	const FRichCurve* GetRichCurveConst(int32 Index) const;
	bool HasAnyData() const;
	UCurveLinearColor* GetCurveAsset() const;
	FLinearColor GetValue(float InTime) const;
	void GetTimeRange(float& Min, float& Max) const;
	void GetTimeRange(double& Min, double& Max) const;
	void GetValueRange(FLinearColor& Min, FLinearColor& Max) const;
};

UCLASS(DisplayName = "Inline Curves Function Library")
class GTCORE_API UInlineCurveLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Has Any Data (Float Curve)")
		static bool HasAnyFloatCurveData(const FInlineFloatCurve& InCurve);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Curve Asset (Float Curve)")
		static UCurveFloat* GetFloatCurveAsset(const FInlineFloatCurve& InCurve);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Value (Float Curve)")
		static float GetFloatCurveValue(const FInlineFloatCurve& InCurve, float InTime);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Time Range (Float Curve)")
		static void GetFloatCurveTimeRange(const FInlineFloatCurve& InCurve, float& TimeMin, float& TimeMax);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Value Range (Float Curve)")
		static void GetFloatCurveValueRange(const FInlineFloatCurve& InCurve, float& ValueMin, float& ValueMax);
	
	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Has Any Data (Vector Curve)")
		static bool HasAnyVectorCurveData(const FInlineVectorCurve& InCurve);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Curve Asset (Vector Curve)")
		static UCurveVector* GetVectorCurveAsset(const FInlineVectorCurve& InCurve);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Value (Vector Curve)")
		static FVector GetVectorCurveValue(const FInlineVectorCurve& InCurve, float InTime);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Time Range (Vector Curve)")
		static void GetVectorCurveTimeRange(const FInlineVectorCurve& InCurve, float& TimeMin, float& TimeMax);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Value Range (Vector Curve)")
		static void GetVectorCurveValueRange(const FInlineVectorCurve& InCurve, FVector& ValueMin, FVector& ValueMax);
	
	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Has Any Data (Color Curve)")
		static bool HasAnyColorCurveData(const FInlineColorCurve& InCurve);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Curve Asset (Color Curve)")
		static UCurveLinearColor* GetColorCurveAsset(const FInlineColorCurve& InCurve);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Value (Color Curve)")
		static FLinearColor GetColorCurveValue(const FInlineColorCurve& InCurve, float InTime);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Time Range (Color Curve)")
		static void GetColorCurveTimeRange(const FInlineColorCurve& InCurve, float& TimeMin, float& TimeMax);

	UFUNCTION(BlueprintPure, Category = "Math|Curves|Runtime", DisplayName = "Get Value Range (Color Curve)")
		static void GetColorCurveValueRange(const FInlineColorCurve& InCurve, FLinearColor& ValueMin, FLinearColor& ValueMax);
};
