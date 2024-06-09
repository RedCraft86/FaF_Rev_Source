// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MiscTypes.generated.h"

UENUM(BlueprintType)
enum class EWindowsDialogueType : uint8
{
	Ok						= 0	UMETA(DisplayName = "OK"),
	OkCancel				= 1	UMETA(DisplayName = "OK, Cancel"),
	AbortRetryIgnore		= 2	UMETA(DisplayName = "Abort, Retry, and Ignore"),
	YesNoCancel				= 3	UMETA(DisplayName = "Yes, No, Cancel"),
	YesNo					= 4	UMETA(DisplayName = "Yes, No"),
	RetryCancel				= 5	UMETA(DisplayName = "Retry, Cancel"),
	CancelTryAgainContinue	= 6	UMETA(DisplayName = "Cancel, Try Again, Continue")
};

UENUM(BlueprintType)
enum class EWindowsDialogueIcon : uint8
{
	None		= 0,
	Error		= 16,
	Warning		= 48,
	Information	= 64
};

UENUM(BlueprintType)
enum class EWindowsDialogueResult : uint8
{
	None		= 0,
	OK			= 1,
	Cancel		= 2,
	Abort		= 3,
	Retry		= 4,
	Ignore		= 5,
	Yes			= 6,
	No			= 7,
	TryAgain	= 10,
	Continue	= 11
};

UENUM(BlueprintType)
enum class EVirtualKeyTriggerType : uint8
{
	Click,
	DoubleClick,
	Press,
	Release
};

USTRUCT(BlueprintType)
struct GTCORE_API FActorBoundsCheckParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
		TEnumAsByte<ECollisionChannel> LineTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
		FVector BoundingBoxLerp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
		bool bOnlyCollidingComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
		bool bIncludeFromChildActors;

	FActorBoundsCheckParams()
		: LineTraceChannel(ECC_Visibility)
		, BoundingBoxLerp(FVector(1.0f))
		, bOnlyCollidingComponents(true)
		, bIncludeFromChildActors(false)
	{}
	
	FActorBoundsCheckParams(const ECollisionChannel InLineTraceChannel, const FVector& InBoundingBoxLerp,
		const bool bInOnlyCollidingComponents, const bool bInIncludeFromChildActors)
		: LineTraceChannel(InLineTraceChannel)
		, BoundingBoxLerp(InBoundingBoxLerp)
		, bOnlyCollidingComponents(bInOnlyCollidingComponents)
		, bIncludeFromChildActors(bInIncludeFromChildActors)
	{}
	
	FActorBoundsCheckParams(const float InBoundingBoxLerp)
		: LineTraceChannel(ECC_Visibility)
		, BoundingBoxLerp(FVector(InBoundingBoxLerp))
		, bOnlyCollidingComponents(true)
		, bIncludeFromChildActors(false)
	{}
	
	friend FArchive& operator<<(FArchive& Ar, FActorBoundsCheckParams& Params)
	{
		Ar << Params.LineTraceChannel;
		Ar << Params.BoundingBoxLerp;
		Ar << Params.bOnlyCollidingComponents;
		Ar << Params.bIncludeFromChildActors;
		return Ar;
	}
};

USTRUCT(BlueprintType)
struct GTCORE_API FGameCurrency
{
	GENERATED_BODY()

	FGameCurrency() : CurrencyUnits(0) {}
	FGameCurrency(const int64 InCents) : CurrencyUnits(InCents) {}
	friend FArchive& operator<<(FArchive& Ar, FGameCurrency& Data)
	{
		Ar << Data.CurrencyUnits;
		return Ar;
	}

	int32 GetAmount() const { return CurrencyUnits; }
	void SetAmount(const int32 InCents) { CurrencyUnits = FMath::Max(0, InCents); }
	bool HasEnoughMoney(const int32 InRequirement) const { return CurrencyUnits > FMath::Max(0, InRequirement); }
	void RemoveMoney(const int32 InRemoval) { CurrencyUnits = FMath::Max(0, CurrencyUnits - FMath::Max(0, InRemoval)); }
	void AddMoney(const int32 InAddition) { CurrencyUnits += FMath::Max(0, InAddition); }

private:
	
	UPROPERTY(EditAnywhere, Category = "GameCurrency", meta = (ClampMin = 0, UIMin = 0))
		int32 CurrencyUnits;
};

UCLASS(DisplayName = "Currency Function Library")
class GTCORE_API UGameCurrencyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "GameCurrency")
		static int32 GetMoney(const FGameCurrency& Target) { return Target.GetAmount(); }

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
		static void SetMoney(UPARAM(ref) FGameCurrency& Target, const int32 InMoney) { return Target.SetAmount(InMoney); }

	UFUNCTION(BlueprintPure, Category = "GameCurrency")
		static bool HasEnoughMoney(const FGameCurrency& Target, const int32 InRequirement) { return Target.HasEnoughMoney(InRequirement); }

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
		static void RemoveMoney(UPARAM(ref) FGameCurrency& Target, const int32 InRemoval) { return Target.RemoveMoney(InRemoval); }

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
		static void AddMoney(UPARAM(ref) FGameCurrency& Target, const int32 InAddition) { return Target.AddMoney(InAddition); }
};