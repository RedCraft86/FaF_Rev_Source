// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once 

#include "RCMathTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Vector Direction")
enum class ERCVectorDirection : uint8
{
	Forward,
	Right,
	Up
};

USTRUCT(BlueprintType, DisplayName = "Currency Data (Money)")
struct RCRUNTIME_API FRCCurrency
{
    GENERATED_BODY()

private:

    int64 CurrencyCents;

public:

    FRCCurrency() : CurrencyCents(0) {}
    FRCCurrency(const int64 InCents)
        : CurrencyCents(InCents)
    {}

    friend FArchive& operator<<(FArchive& Ar, FRCCurrency& Currency)
    {
        Ar << Currency.CurrencyCents;
        return Ar;
    }

    FORCEINLINE bool operator==(const FRCCurrency& Other) const
    {
        return CurrencyCents == Other.CurrencyCents;
    }

    FORCEINLINE bool operator!=(const FRCCurrency& Other) const
    {
        return CurrencyCents != Other.CurrencyCents;
    }

    FORCEINLINE bool operator<(const FRCCurrency& Other) const
    {
        return CurrencyCents < Other.CurrencyCents;
    }

    FORCEINLINE bool operator<=(const FRCCurrency& Other) const
    {
        return CurrencyCents <= Other.CurrencyCents;
    }

    FORCEINLINE bool operator>(const FRCCurrency& Other) const
    {
        return CurrencyCents > Other.CurrencyCents;
    }

    FORCEINLINE bool operator>=(const FRCCurrency& Other) const
    {
        return CurrencyCents >= Other.CurrencyCents;
    }

    int64 GetCents() const
    {
        return CurrencyCents;
    }

    void SetCents(const int64 InCents)
    {
        CurrencyCents += InCents;
    }

    void AddMoney(const int64 InCents)
    {
        CurrencyCents += InCents;
    }

    void RemoveMoney(const int64 InCents)
    {
        CurrencyCents -= InCents;
    }

    FString ToString() const
    {
        return CentsToString(CurrencyCents);
    }

    void InitFromString(const FString& InString, const bool bOverride = false)
    {
        if (bOverride)
            CurrencyCents = StringToCents(InString);
        else
            CurrencyCents += StringToCents(InString);
    }

    static FRCCurrency FromString(const FString& InString)
    {
        FRCCurrency Currency;
        Currency.InitFromString(InString);
        return Currency;
    }

    static FString CentsToString(const int64 InCents)
    {
        const int32 Whole = FMath::TruncToInt32(InCents / 100.0f);
        const int8 Cents = InCents - (Whole * 100);

        return FString::Printf(TEXT("%d.%d"), Whole, Cents);
    }

    static int64 StringToCents(const FString& InString)
    {
        FString _Whole, _Cent = TEXT("");
        InString.Split(TEXT("."), &_Whole, &_Cent);
        if (!_Whole.IsNumeric() || !_Cent.IsNumeric())
        {
            return 0;
        }

        return (FCString::Atoi(*_Whole) * 100) + FCString::Atoi(*_Cent);
    }
};

USTRUCT(BlueprintType, DisplayName = "Vector Range")
struct RCRUNTIME_API FRCVectorRange
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorRange")
        bool bClampX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorRange")
        bool bClampY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorRange")
        bool bClampZ;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorRange")
        FVector MinVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorRange")
        FVector MaxVector;

    friend FArchive& operator<<(FArchive& Ar, FRCVectorRange& Range)
    {
        Ar << Range.bClampX;
        Ar << Range.bClampY;
        Ar << Range.bClampZ;
        Ar << Range.MinVector;
        Ar << Range.MaxVector;
        return Ar;
    }

    FRCVectorRange()
        : bClampX(true)
        , bClampY(true)
        , bClampZ(true)
        , MinVector(FVector::ZeroVector)
        , MaxVector(FVector::ZeroVector)
    {}

    FRCVectorRange(const FVector& InMinVector, const FVector& InMaxVector, const bool bInClampX = true, const bool bInClampY = true, const bool bInClampZ = true)
        : bClampX(bInClampX)
        , bClampY(bInClampY)
        , bClampZ(bInClampZ)
        , MinVector(InMinVector)
        , MaxVector(InMaxVector)
    {}

    FVector ClampVector(const FVector& InVector) const
    {
        FVector Result = InVector;
        if (bClampX) {Result.X = FMath::Clamp(InVector.X, MinVector.X, MaxVector.X);}
        if (bClampY) {Result.Y = FMath::Clamp(InVector.Y, MinVector.Y, MaxVector.Y);}
        if (bClampZ) {Result.Z = FMath::Clamp(InVector.Z, MinVector.Z, MaxVector.Z);}
        return Result;
    }
};

USTRUCT(BlueprintType, DisplayName = "Rotator Range")
struct RCRUNTIME_API FRCRotatorRange
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorRange")
        bool bClampRoll;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorRange")
        bool bClampPitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorRange")
        bool bClampYaw;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorRange")
        FRotator MinRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorRange")
        FRotator MaxRotation;

    friend FArchive& operator<<(FArchive& Ar, FRCRotatorRange& Range)
    {
        Ar << Range.bClampRoll;
        Ar << Range.bClampPitch;
        Ar << Range.bClampYaw;
        Ar << Range.MinRotation;
        Ar << Range.MaxRotation;
        return Ar;
    }

    FRCRotatorRange()
        : bClampRoll(true)
        , bClampPitch(true)
        , bClampYaw(true)
        , MinRotation(FRotator::ZeroRotator)
        , MaxRotation(FRotator::ZeroRotator)
    {}

    FRCRotatorRange(const FRotator& InMinRotation, const FRotator& InMaxRotation, const bool bInClampRoll = true, const bool bInClampPitch = true, const bool bInClampYaw = true)
        : bClampRoll(bInClampPitch)
        , bClampPitch(bInClampRoll)
        , bClampYaw(bInClampYaw)
        , MinRotation(InMinRotation)
        , MaxRotation(InMaxRotation)
    {}

    FRotator ClampRotator(const FRotator& InRotator) const
    {
        FRotator Result = InRotator;
        if (bClampRoll) {Result.Roll = FMath::Clamp(InRotator.Roll, MinRotation.Roll, MaxRotation.Roll);}
        if (bClampPitch) {Result.Pitch = FMath::Clamp(InRotator.Pitch, MinRotation.Pitch, MaxRotation.Pitch);}
        if (bClampYaw) {Result.Yaw = FMath::Clamp(InRotator.Yaw, MinRotation.Yaw, MaxRotation.Yaw);}
        return Result;
    }
};