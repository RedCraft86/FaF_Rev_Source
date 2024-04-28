// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PlayerData.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerFlags : uint8
{
	Locked,
};
ENUM_CLASS_FLAGS(EPlayerFlags);

USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerFlags"))
		uint8 PlayerFlags;

	FPlayerSettings() : PlayerFlags(0) {}
};

USTRUCT(BlueprintInternalUseOnly)
struct FPlayerInteraction
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
		AActor* Target;

	UPROPERTY(Transient)
		FHitResult Hit;

	FPlayerInteraction() : Target(nullptr) {}
	FPlayerInteraction(AActor* InTarget, const FHitResult& InHit) : Target(InTarget), Hit(InHit) {}
	FORCEINLINE bool operator==(const FPlayerInteraction& Other) const { return Target == Other.Target; }
	FORCEINLINE bool operator!=(const FPlayerInteraction& Other) const { return Target != Other.Target; }
	FORCEINLINE bool IsValidData() const { return IsValid(Target) && IsValid(Hit.GetActor()); }
	void Reset()
	{
		Target = nullptr;
		Hit.Reset(1.0f, false);
	}
};
