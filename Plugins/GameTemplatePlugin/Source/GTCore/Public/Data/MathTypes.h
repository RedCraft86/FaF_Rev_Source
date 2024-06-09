// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MathTypes.generated.h"

UENUM(BlueprintType)
enum class EVectorDirection : uint8
{
	Forward,
	Right,
	Up
};

USTRUCT(BlueprintType, DisplayName = "Interpolatable Scalar")
struct GTCORE_API FGTInterpScalar
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterpScalar", meta = (AllowPrivateAccess = true))
		float CurrentValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterpScalar", meta = (AllowPrivateAccess = true))
		float TargetValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterpScalar", meta = (AllowPrivateAccess = true))
		float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterpScalar", meta = (AllowPrivateAccess = true))
		bool bConstant;
	
	FGTInterpScalar() : CurrentValue(0.0f), TargetValue(0.0f), InterpSpeed(5.0f), bConstant(false) {}
	FGTInterpScalar(const float InValue, const float InSpeed = 5.0f, const bool bInConstant = false)
		: CurrentValue(InValue), TargetValue(InValue), InterpSpeed(InSpeed), bConstant(bInConstant) {}
	
	void SnapToTarget() { CurrentValue = TargetValue; }
	bool IsComplete() const { return FMath::IsNearlyEqual(CurrentValue, TargetValue); }
	void Tick(const float InDeltaTime)
	{
		if (IsComplete()) return;
		CurrentValue = bConstant ? FMath::FInterpConstantTo(CurrentValue, TargetValue, InDeltaTime, InterpSpeed)
			: FMath::FInterpTo(CurrentValue, TargetValue, InDeltaTime, InterpSpeed);
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct FGTModifiableScalarBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModifiableScalar")
		float BaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModifiableScalar")
		TMap<FString, float> Modifiers;

	FGTModifiableScalarBase() : BaseValue(0.0f), Modifiers({}) {}
	explicit FGTModifiableScalarBase(const float InBase) : BaseValue(InBase), Modifiers({}) {}
	virtual ~FGTModifiableScalarBase() = default;
	
	virtual float Evaluate() const { return BaseValue; }
};

USTRUCT(BlueprintType)
struct GTCORE_API FGTModifiableMultiplier : public FGTModifiableScalarBase
{
	GENERATED_BODY()
	
	FGTModifiableMultiplier() : FGTModifiableScalarBase(1.0f) {}
	FGTModifiableMultiplier(const float InBase) : FGTModifiableScalarBase(InBase) {}
	virtual float Evaluate() const
	{
		float Value = BaseValue;
		for (const TPair<FString, float>& Pair : Modifiers)
		{
			if (Pair.Key.IsEmpty()) continue;
			Value *= Pair.Value;
		}
		return Value;
	}
};

USTRUCT(BlueprintType)
struct GTCORE_API FGTModifiableSum : public FGTModifiableScalarBase
{
	GENERATED_BODY()
	
	FGTModifiableSum() {}
	FGTModifiableSum(const float InBase) : FGTModifiableScalarBase(InBase) {}
	virtual float Evaluate() const
	{
		float Value = BaseValue;
		for (const TPair<FString, float>& Pair : Modifiers)
		{
			if (Pair.Key.IsEmpty()) continue;
			Value += Pair.Value;
		}
		return Value;
	}
};
