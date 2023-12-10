// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "RCMathLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

float URCMathLibrary::SmallNumber()
{
	return UE_SMALL_NUMBER;
}

float URCMathLibrary::KindaSmallNumber()
{
	return UE_KINDA_SMALL_NUMBER;
}

float URCMathLibrary::BigNumber()
{
	return UE_BIG_NUMBER;
}

float URCMathLibrary::GetHorizontalDistance(const FVector& V1, const FVector& V2)
{
	return FVector::DistXY(V1, V2);
}

FRotator URCMathLibrary::AddRotators(const FRotator& A, const FRotator& B)
{
	const FQuat AQuat = FQuat(A);
	const FQuat BQuat = FQuat(B);

	return FRotator(BQuat * AQuat);
}

FLinearColor URCMathLibrary::LinearColorFromTemperature(float InTemperature)
{
	return FLinearColor::MakeFromColorTemperature(InTemperature);
}

FDateTime URCMathLibrary::GetDateTimeFromTicks(const int64 Ticks)
{
	return FDateTime(Ticks);
}

int64 URCMathLibrary::GetDateTimeTicks(const FDateTime& DateTime)
{
	return DateTime.GetTicks();
}

FTimespan URCMathLibrary::GetTimespanFromTicks(const int64 Ticks)
{
	return FTimespan(Ticks);
}

int64 URCMathLibrary::GetTimespanTicks(const FTimespan& Timespan)
{
	return Timespan.GetTicks();
}

void URCMathLibrary::GetRangeFloat(const FFloatRange& Range, float& Min, float& Max)
{
	Min = Range.GetLowerBoundValue();
	Max = Range.GetUpperBoundValue();
}

float URCMathLibrary::ClampRangeFloat(const FFloatRange& Range, float Value)
{
	return FMath::Clamp(Value, Range.GetLowerBoundValue(), Range.GetUpperBoundValue());
}

FVector URCMathLibrary::ClampRangeVector(const FRCVectorRange& Range, const FVector& Value)
{
	return Range.ClampVector(Value);
}

FRotator URCMathLibrary::ClampRangeRotator(const FRCRotatorRange& Range, const FRotator& Value)
{
	return Range.ClampRotator(Value);
}

void URCMathLibrary::GetLineTraceVectors(const FVector& Location, const FRotator& Rotation, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End)
{
	FVector DirectionVector;
	switch (Direction)
	{
	case ERCVectorDirection::Forward:
		DirectionVector = UKismetMathLibrary::GetForwardVector(Rotation);
		break;

	case ERCVectorDirection::Right:
		DirectionVector = UKismetMathLibrary::GetRightVector(Rotation);
		break;

	case ERCVectorDirection::Up:
		DirectionVector = UKismetMathLibrary::GetUpVector(Rotation);
		break;
	};

	Start = Location;
	End = Location + (DirectionVector * Distance);
}

void URCMathLibrary::GetActorLineTraceVectors(const AActor* Actor, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End)
{
	if (!Actor)
		return;

	FVector DirectionVector;
	switch (Direction)
	{
	case ERCVectorDirection::Forward:
		DirectionVector = Actor->GetActorForwardVector();
		break;

	case ERCVectorDirection::Right:
		DirectionVector = Actor->GetActorRightVector();
		break;

	case ERCVectorDirection::Up:
		DirectionVector = Actor->GetActorUpVector();
		break;
	};

	Start = Actor->GetActorLocation();
	End = Actor->GetActorLocation() + (DirectionVector * Distance);
}

void URCMathLibrary::GetComponentLineTraceVectors(const USceneComponent* Component, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End)
{
	if (!Component)
		return;

	FVector DirectionVector;
	switch (Direction)
	{
	case ERCVectorDirection::Forward:
		DirectionVector = Component->GetForwardVector();
		break;

	case ERCVectorDirection::Right:
		DirectionVector = Component->GetRightVector();
		break;

	case ERCVectorDirection::Up:
		DirectionVector = Component->GetUpVector();
		break;
	};

	Start = Component->GetComponentLocation();
	End = Component->GetComponentLocation() + (DirectionVector * Distance);
}

void URCMathLibrary::GetCameraLineTraceVectors(const UObject* WorldContextObject, const ERCVectorDirection Direction, const float Distance, FVector& Start, FVector& End)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	const APlayerCameraManager* PCM = PC ? PC->PlayerCameraManager : nullptr;
	if (!PCM)
		return;

	FVector DirectionVector;
	switch (Direction)
	{
	case ERCVectorDirection::Forward:
		DirectionVector = PCM->GetActorForwardVector();
		break;

	case ERCVectorDirection::Right:
		DirectionVector = PCM->GetActorRightVector();
		break;

	case ERCVectorDirection::Up:
		DirectionVector = PCM->GetActorUpVector();
		break;
	};

	Start = PCM->GetCameraLocation();
	End = PCM->GetCameraLocation() + (DirectionVector * Distance);
}

bool URCMathLibrary::IsRuntimeFloatCurveValid(const FRuntimeFloatCurve& Curve)
{
	return Curve.GetRichCurveConst()->HasAnyData();
}

UCurveFloat* URCMathLibrary::GetExternalRuntimeFloatCurve(const FRuntimeFloatCurve& Curve)
{
	return Curve.ExternalCurve;
}

float URCMathLibrary::GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& Curve, float InTime)
{
	return Curve.GetRichCurveConst()->Eval(InTime);
}

void URCMathLibrary::GetRuntimeFloatCurveTimeRange(const FRuntimeFloatCurve& Curve, float& Min, float& Max)
{
	Curve.GetRichCurveConst()->GetTimeRange(Min, Max);
}

void URCMathLibrary::GetRuntimeFloatCurveValueRange(const FRuntimeFloatCurve& Curve, float& Min, float& Max)
{
	Curve.GetRichCurveConst()->GetValueRange(Min, Max);
}

bool URCMathLibrary::IsRuntimeVectorCurveValid(const FRuntimeVectorCurve& Curve)
{
	return Curve.GetRichCurveConst(0)->HasAnyData()
		|| Curve.GetRichCurveConst(1)->HasAnyData()
		|| Curve.GetRichCurveConst(2)->HasAnyData();
}

UCurveVector* URCMathLibrary::GetExternalRuntimeVectorCurve(const FRuntimeVectorCurve& Curve)
{
	return Curve.ExternalCurve;
}

FVector URCMathLibrary::GetRuntimeVectorCurveValue(const FRuntimeVectorCurve& Curve, float InTime)
{
	return Curve.GetValue(InTime);
}

void URCMathLibrary::GetRuntimeVectorCurveTimeRange(const FRuntimeVectorCurve& Curve, float& Min, float& Max)
{
	float Min1, Max1 = 0.0f;
	Curve.GetRichCurveConst(0)->GetTimeRange(Min1, Max1);

	float Min2, Max2 = 0.0f;
	Curve.GetRichCurveConst(1)->GetTimeRange(Min2, Max2);

	float Min3, Max3 = 0.0f;
	Curve.GetRichCurveConst(2)->GetTimeRange(Min3, Max3);

	Min = FMath::Min3(Min1, Min2, Min3);
	Max = FMath::Max3(Max1, Max2, Max3);
}

void URCMathLibrary::GetRuntimeVectorCurveValueRange(const FRuntimeVectorCurve& Curve, FVector& Min, FVector& Max)
{
	float MinX, MaxX = 0.0f;
	Curve.GetRichCurveConst(0)->GetValueRange(MinX, MaxX);

	float MinY, MaxY = 0.0f;
	Curve.GetRichCurveConst(1)->GetValueRange(MinY, MaxY);

	float MinZ, MaxZ = 0.0f;
	Curve.GetRichCurveConst(2)->GetValueRange(MinZ, MaxZ);

	Min = FVector(MinX, MinY, MinZ);
	Max = FVector(MaxX, MaxY, MaxZ);
}

bool URCMathLibrary::IsRuntimeLinearColorCurveValid(const FRuntimeLinearColorCurve& Curve)
{
	return Curve.GetRichCurveConst(0)->HasAnyData()
		|| Curve.GetRichCurveConst(1)->HasAnyData()
		|| Curve.GetRichCurveConst(2)->HasAnyData()
		|| Curve.GetRichCurveConst(3)->HasAnyData();
}

UCurveLinearColor* URCMathLibrary::GetExternalRuntimeLinearColorCurve(const FRuntimeLinearColorCurve& Curve)
{
	return Curve.Curve.ExternalCurve;
}

FLinearColor URCMathLibrary::GetRuntimeLinearColorCurveValue(const FRuntimeLinearColorCurve& Curve, float InTime)
{
	return Curve.Curve.GetLinearColorValue(InTime);
}

void URCMathLibrary::GetRuntimeLinearColorCurveTimeRange(const FRuntimeLinearColorCurve& Curve, float& Min, float& Max)
{
	float MinR, MaxR = 0.0f;
	Curve.GetRichCurveConst(0)->GetTimeRange(MinR, MaxR);

	float MinG, MaxG = 0.0f;
	Curve.GetRichCurveConst(1)->GetTimeRange(MinG, MaxG);

	float MinB, MaxB = 0.0f;
	Curve.GetRichCurveConst(2)->GetTimeRange(MinB, MaxB);

	float MinA, MaxA = 0.0f;
	Curve.GetRichCurveConst(3)->GetTimeRange(MinA, MaxA);

	Min = FMath::Min(FMath::Min3(MinR, MinG, MinB), MinA);
	Max = FMath::Max(FMath::Max3(MaxR, MaxG, MaxB), MaxA);
}

void URCMathLibrary::GetRuntimeLinearColorCurveValueRange(const FRuntimeLinearColorCurve& Curve, FLinearColor& Min, FLinearColor& Max)
{
	float MinR, MaxR = 0.0f;
	Curve.GetRichCurveConst(0)->GetValueRange(MinR, MaxR);

	float MinG, MaxG = 0.0f;
	Curve.GetRichCurveConst(1)->GetValueRange(MinG, MaxG);

	float MinB, MaxB = 0.0f;
	Curve.GetRichCurveConst(2)->GetValueRange(MinB, MaxB);

	float MinA, MaxA = 0.0f;
	Curve.GetRichCurveConst(3)->GetValueRange(MinA, MaxA);

	Min = FLinearColor(MinR, MinG, MinB, MinA);
	Max = FLinearColor(MaxR, MaxG, MaxB, MaxA);
}

float URCMathLibrary::AddArray_Float(TArray<float> InFloatArray)
{
	float Result = 0.0f;
	for (const float Element : InFloatArray)
	{
		Result += Element;
	}

	return Result;
}

float URCMathLibrary::SubtractArray_Float(TArray<float> InFloatArray)
{
	float Result = 0.0f;
	for (const float Element : InFloatArray)
	{
		Result -= Element;
	}

	return Result;
}

float URCMathLibrary::MultiplyArray_Float(TArray<float> InFloatArray)
{
	float Result = 1.0f;
	for (const float Element : InFloatArray)
	{
		Result *= Element;
	}

	return Result;
}

float URCMathLibrary::DivideArray_Float(TArray<float> InFloatArray)
{
	float Result = 1.0f;
	for (const float Element : InFloatArray)
	{
		Result /= Element;
	}

	return Result;
}

int URCMathLibrary::AddArray_Int(TArray<int> InIntArray)
{
	int Result = 0;
	for (const int Element : InIntArray)
	{
		Result += Element;
	}

	return Result;
}

int URCMathLibrary::SubtractArray_Int(TArray<int> InIntArray)
{
	int Result = 0;
	for (const int Element : InIntArray)
	{
		Result -= Element;
	}

	return Result;
}

int URCMathLibrary::MultiplyArray_Int(TArray<int> InIntArray)
{
	int Result = 1;
	for (const int Element : InIntArray)
	{
		Result *= Element;
	}

	return Result;
}

int URCMathLibrary::DivideArray_Int(TArray<int> InIntArray)
{
	int Result = 1;
	for (const int Element : InIntArray)
	{
		Result /= Element;
	}

	return Result;
}
