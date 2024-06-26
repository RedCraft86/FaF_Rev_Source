// Copyright (C) RedCraft86. All Rights Reserved.

#include "Data/InlineCurves.h"

FRichCurve* FInlineFloatCurve::GetRichCurve()
{
	return Curve.GetRichCurve();
}

const FRichCurve* FInlineFloatCurve::GetRichCurveConst() const
{
	return Curve.GetRichCurveConst();
}

bool FInlineFloatCurve::HasAnyData() const
{
	return GetRichCurveConst()->HasAnyData();
}

UCurveFloat* FInlineFloatCurve::GetCurveAsset() const
{
	return Curve.ExternalCurve;
}

float FInlineFloatCurve::GetValue(float InTime) const
{
	return GetRichCurveConst()->Eval(InTime);
}

void FInlineFloatCurve::GetTimeRange(float& Min, float& Max) const
{
	GetRichCurveConst()->GetTimeRange(Min, Max);
}

void FInlineFloatCurve::GetTimeRange(double& Min, double& Max) const
{
	float X, Y = 0.0f;
	GetTimeRange(X, Y);
	Min = X; Max = Y;
}

void FInlineFloatCurve::GetValueRange(float& Min, float& Max) const
{
	GetRichCurveConst()->GetValueRange(Min, Max);
}

void FInlineFloatCurve::GetValueRange(double& Min, double& Max) const
{
	float X, Y = 0.0f;
	GetValueRange(X, Y);
	Min = X; Max = Y;
}

FRichCurve* FInlineVectorCurve::GetRichCurve(int32 Index)
{
	return Curve.GetRichCurve(Index);
}

const FRichCurve* FInlineVectorCurve::GetRichCurveConst(int32 Index) const
{
	return Curve.GetRichCurveConst(Index);
}

bool FInlineVectorCurve::HasAnyData() const
{
	return GetRichCurveConst(0)->HasAnyData()
		|| GetRichCurveConst(1)->HasAnyData()
		|| GetRichCurveConst(2)->HasAnyData();
}

UCurveVector* FInlineVectorCurve::GetCurveAsset() const
{
	return Curve.ExternalCurve;
}

FVector FInlineVectorCurve::GetValue(float InTime) const
{
	return {
		GetRichCurveConst(0)->Eval(InTime),
		GetRichCurveConst(1)->Eval(InTime),
		GetRichCurveConst(2)->Eval(InTime)
	};
}

void FInlineVectorCurve::GetTimeRange(float& Min, float& Max) const
{
	float Min1, Min2, Min3 = 0.0f;
	float Max1, Max2, Max3 = 0.0f;

	GetRichCurveConst(0)->GetTimeRange(Min1, Max1);
	GetRichCurveConst(1)->GetTimeRange(Min2, Max2);
	GetRichCurveConst(2)->GetTimeRange(Min3, Max3);

	Min = FMath::Min3(Min1, Min2, Min3);
	Max = FMath::Max3(Max1, Max2, Max3);
}

void FInlineVectorCurve::GetTimeRange(double& Min, double& Max) const
{
	float X, Y = 0.0f;
	GetTimeRange(X, Y);
	Min = X; Max = Y;
}

void FInlineVectorCurve::GetValueRange(FVector& Min, FVector& Max) const
{
	float MinX, MinY, MinZ = 0.0f;
	float MaxX, MaxY, MaxZ = 0.0f;

	GetRichCurveConst(0)->GetValueRange(MinX, MaxX);
	GetRichCurveConst(1)->GetValueRange(MinY, MaxY);
	GetRichCurveConst(2)->GetValueRange(MinZ, MaxZ);

	Min = { MinX, MinY, MinZ };
	Max = { MaxX, MaxY, MaxZ };
}

FRichCurve* FInlineColorCurve::GetRichCurve(int32 Index)
{
	if (Index < 0 || Index >= 4)
	{
		return nullptr;
	}

	if (Curve.ExternalCurve)
	{
		return &Curve.ExternalCurve->FloatCurves[Index];
	}
	
	return &Curve.ColorCurves[Index];
}

const FRichCurve* FInlineColorCurve::GetRichCurveConst(int32 Index) const
{
	if (Index < 0 || Index >= 4)
	{
		return nullptr;
	}

	if (Curve.ExternalCurve)
	{
		return &Curve.ExternalCurve->FloatCurves[Index];
	}
	
	return &Curve.ColorCurves[Index];
}

bool FInlineColorCurve::HasAnyData() const
{
	return GetRichCurveConst(0)->HasAnyData()
		|| GetRichCurveConst(1)->HasAnyData()
		|| GetRichCurveConst(2)->HasAnyData()
		|| GetRichCurveConst(3)->HasAnyData();
}

UCurveLinearColor* FInlineColorCurve::GetCurveAsset() const
{
	return Curve.ExternalCurve;
}

FLinearColor FInlineColorCurve::GetValue(float InTime) const
{
	FLinearColor Result = {
		GetRichCurveConst(0)->Eval(InTime),
		GetRichCurveConst(1)->Eval(InTime),
		GetRichCurveConst(2)->Eval(InTime)
	};
	
	const FRichCurve* Alpha = GetRichCurveConst(3);
	Result.A = Alpha->GetNumKeys() == 0 ? 1.0f : Alpha->Eval(InTime);

	return Result;
}

void FInlineColorCurve::GetTimeRange(float& Min, float& Max) const
{
	float Min1, Min2, Min3, Min4 = 0.0f;
	float Max1, Max2, Max3, Max4 = 0.0f;

	GetRichCurveConst(0)->GetTimeRange(Min1, Max1);
	GetRichCurveConst(1)->GetTimeRange(Min2, Max2);
	GetRichCurveConst(2)->GetTimeRange(Min3, Max3);
	GetRichCurveConst(3)->GetTimeRange(Min4, Max4);

	Min = FMath::Min3(Min1, Min2, FMath::Min(Min3, Min4));
	Max = FMath::Max3(Max1, Max2, FMath::Min(Max3, Max4));
}

void FInlineColorCurve::GetTimeRange(double& Min, double& Max) const
{
	float X, Y = 0.0f;
	GetTimeRange(X, Y);
	Min = X; Max = Y;
}

void FInlineColorCurve::GetValueRange(FLinearColor& Min, FLinearColor& Max) const
{
	float MinR, MinG, MinB, MinA = 0.0f;
	float MaxR, MaxG, MaxB, MaxA = 0.0f;

	GetRichCurveConst(0)->GetValueRange(MinR, MaxR);
	GetRichCurveConst(1)->GetValueRange(MinG, MaxG);
	GetRichCurveConst(2)->GetValueRange(MinB, MaxB);

	const FRichCurve* Alpha = GetRichCurveConst(3);
	const bool HasAlpha = Alpha->GetNumKeys() == 0;
	Alpha->GetTimeRange(MinA, MaxA);

	Min = { MinR, MinG, MinB, HasAlpha ? MinA : 1.0f };
	Max = { MaxR, MaxG, MaxB, HasAlpha ? MaxA : 1.0f };
}

bool UInlineCurveLibrary::HasAnyFloatCurveData(const FInlineFloatCurve& InCurve)
{
	return InCurve.HasAnyData();
}

UCurveFloat* UInlineCurveLibrary::GetFloatCurveAsset(const FInlineFloatCurve& InCurve)
{
	return InCurve.GetCurveAsset();
}

float UInlineCurveLibrary::GetFloatCurveValue(const FInlineFloatCurve& InCurve, float InTime)
{
	return InCurve.GetValue(InTime);
}

void UInlineCurveLibrary::GetFloatCurveTimeRange(const FInlineFloatCurve& InCurve, float& TimeMin, float& TimeMax)
{
	InCurve.GetTimeRange(TimeMin, TimeMax);
}

void UInlineCurveLibrary::GetFloatCurveValueRange(const FInlineFloatCurve& InCurve, float& ValueMin, float& ValueMax)
{
	InCurve.GetValueRange(ValueMin, ValueMax);
}

bool UInlineCurveLibrary::HasAnyVectorCurveData(const FInlineVectorCurve& InCurve)
{
	return InCurve.HasAnyData();
}

UCurveVector* UInlineCurveLibrary::GetVectorCurveAsset(const FInlineVectorCurve& InCurve)
{
	return InCurve.GetCurveAsset();
}

FVector UInlineCurveLibrary::GetVectorCurveValue(const FInlineVectorCurve& InCurve, float InTime)
{
	return InCurve.GetValue(InTime);
}

void UInlineCurveLibrary::GetVectorCurveTimeRange(const FInlineVectorCurve& InCurve, float& TimeMin, float& TimeMax)
{
	InCurve.GetTimeRange(TimeMin, TimeMax);
}

void UInlineCurveLibrary::GetVectorCurveValueRange(const FInlineVectorCurve& InCurve, FVector& ValueMin, FVector& ValueMax)
{
	InCurve.GetValueRange(ValueMin, ValueMax);
}

bool UInlineCurveLibrary::HasAnyColorCurveData(const FInlineColorCurve& InCurve)
{
	return InCurve.HasAnyData();
}

UCurveLinearColor* UInlineCurveLibrary::GetColorCurveAsset(const FInlineColorCurve& InCurve)
{
	return InCurve.GetCurveAsset();
}

FLinearColor UInlineCurveLibrary::GetColorCurveValue(const FInlineColorCurve& InCurve, float InTime)
{
	return InCurve.GetValue(InTime);
}

void UInlineCurveLibrary::GetColorCurveTimeRange(const FInlineColorCurve& InCurve, float& TimeMin, float& TimeMax)
{
	InCurve.GetTimeRange(TimeMin, TimeMax);
}

void UInlineCurveLibrary::GetColorCurveValueRange(const FInlineColorCurve& InCurve, FLinearColor& ValueMin, FLinearColor& ValueMax)
{
	InCurve.GetValueRange(ValueMin, ValueMax);
}
