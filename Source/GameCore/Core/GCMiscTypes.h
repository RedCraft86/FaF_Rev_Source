// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Camera/CameraShakeBase.h"
#include "GCMiscTypes.generated.h"

USTRUCT(BlueprintType, DisplayName = "Modifiable Scalar")
struct GAMECORE_API FGCModifiableScalar
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModifiableScalar", meta = (AllowPrivateAccess = true))
		float DefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModifiableScalar", meta = (AllowPrivateAccess = true))
		TMap<FName, float> Modifiers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ModifiableScalar", meta = (AllowPrivateAccess = true))
		float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ModifiableScalar", meta = (AllowPrivateAccess = true))
		bool bIsMultiplier;
	
public:

	FGCModifiableScalar() : DefaultValue(0.0f), Modifiers({}), InterpSpeed(10.0f), bIsMultiplier(true) {}
	FGCModifiableScalar(const float InDefaultValue, const bool bInIsMultiplier)
		: DefaultValue(InDefaultValue)
		, Modifiers({})
		, InterpSpeed(10.0f)
		, bIsMultiplier(bInIsMultiplier)
	{}
	FGCModifiableScalar(const float InDefaultValue, const bool bInIsMultiplier, const float InInterpSpeed)
		: DefaultValue(InDefaultValue)
		, Modifiers({})
		, InterpSpeed(InInterpSpeed)
		, bIsMultiplier(bInIsMultiplier)
	{}

	FORCEINLINE void SetDefaultValue(const float InValue) { DefaultValue = InValue; }
	FORCEINLINE void AddModifier(const FName& InKey, const float InValue) { Modifiers.Add(InKey, InValue); }
	FORCEINLINE void RemoveModifier(const FName& InKey) { Modifiers.Remove(InKey); }
	FORCEINLINE void ClearModifiers() { Modifiers.Empty(); }

	FORCEINLINE float GetDefaultValue() const { return DefaultValue; }
	FORCEINLINE float GetInterpSpeed() const { return InterpSpeed; }
	float GetValue() const
	{
		float Result = DefaultValue;
		for (const TPair<FName, float>& Modifier : Modifiers)
		{
			Result = bIsMultiplier ? Result * Modifier.Value : Result + Modifier.Value;
		}

		return Result;
	}
};

USTRUCT(BlueprintType, DisplayName = "Interpolatable Scalar")
struct GAMECORE_API FGCInterpolatableScalar
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InterpolatableScalar", meta = (AllowPrivateAccess = true))
		float Current;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InterpolatableScalar", meta = (AllowPrivateAccess = true))
		float Target;

public:
	
	FGCInterpolatableScalar() : Current(0.0f), Target(0.0f) {}
	FGCInterpolatableScalar(const float InValue) : Current(InValue), Target(InValue) {}
	
	FORCEINLINE float GetValue() const { return Current; }
	FORCEINLINE float GetValueClamped(const float Min = 0.0f, const float Max = 1.0f) const { return FMath::Clamp(Current, Min, Max); }
	FORCEINLINE bool IsDifferent() const { return Current != Target; }
	FORCEINLINE void SetTarget(const float InValue) { Target = InValue; }
	FORCEINLINE void SnapToTarget() { Current = Target; }
	void Interp(const float InDeltaTime, const float InInterpSpeed)
	{
		Current = FMath::FInterpTo(Current, Target, InDeltaTime, InInterpSpeed);
	}
};