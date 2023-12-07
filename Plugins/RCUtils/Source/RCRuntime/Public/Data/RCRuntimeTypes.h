// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCRuntimeTypes.generated.h"

USTRUCT(BlueprintType, DisplayName = "Game Version")
struct RCRUNTIME_API FRCGameVersion
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameVersion")
        uint8 Major;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameVersion")
        uint8 Minor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameVersion")
        uint8 Patch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameVersion", meta = (Multiline = true))
        FString AdditionalInfo;

    FRCGameVersion() : Major(0), Minor(0), Patch(0), AdditionalInfo("") {}
    FRCGameVersion(const FString& VersionString) { InitFromString(VersionString); }
    FRCGameVersion(const uint8 InMajor, const uint8 InMinor, const uint8 InPatch, const FString& InAdditional)
        : Major(InMajor)
        , Minor(InMinor)
        , Patch(InPatch)
        , AdditionalInfo(InAdditional)
    {}

    friend FArchive& operator<<(FArchive& Ar, FRCGameVersion& Version)
    {
        Ar << Version.Major;
        Ar << Version.Minor;
        Ar << Version.Patch;
        Ar << Version.AdditionalInfo;
        return Ar;
    }

    FORCEINLINE bool operator==(const FRCGameVersion& Other) const
    {
        return ToComparableInt() == Other.ToComparableInt();
    }

    FORCEINLINE bool operator!=(const FRCGameVersion& Other) const
    {
        return ToComparableInt() != Other.ToComparableInt();
    }

    FORCEINLINE bool operator<(const FRCGameVersion& Other) const
    {
        return ToComparableInt() < Other.ToComparableInt();
    }

    FORCEINLINE bool operator<=(const FRCGameVersion& Other) const
    {
        return ToComparableInt() <= Other.ToComparableInt();
    }

    FORCEINLINE bool operator>(const FRCGameVersion& Other) const
    {
        return ToComparableInt() > Other.ToComparableInt();
    }

    FORCEINLINE bool operator>=(const FRCGameVersion& Other) const
    {
        return ToComparableInt() >= Other.ToComparableInt();
    }

    int32 ToComparableInt() const
    {
        return FCString::Atoi(*FString::Printf(TEXT("%d%d%d"), Major, Minor, Patch));
    }

    FString ToString(const bool AddAdditionalInfo = false) const
    {
        return FString::Printf(TEXT("%d.%d.%d%s"), Major, Minor, Patch,
            AddAdditionalInfo && !AdditionalInfo.IsEmpty() ? *(TEXT(" ") + AdditionalInfo) : TEXT(""));
    }

    void InitFromString(const FString& InString)
    {
        FString Str = InString;
        FString Additional = TEXT("");
        Str.ReplaceInline(TEXT("v"), TEXT(""));
        if (Str.Contains(TEXT(" ")))
            Str.Split(TEXT(" "), &Str, &Additional);
            
        TArray<FString> Components;
        Str.ParseIntoArray(Components, TEXT("."), true);

        Major = Components.IsValidIndex(0) ? FCString::Atoi(*Components[0]) : 0;
        Minor = Components.IsValidIndex(1) ? FCString::Atoi(*Components[1]) : 0;
        Patch = Components.IsValidIndex(2) ? FCString::Atoi(*Components[2]) : 0;
        AdditionalInfo = Additional;
    }
};

USTRUCT(BlueprintType)
struct FActorBoundsCheckParams
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
        TEnumAsByte<ECollisionChannel> LineTraceChannel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
        FVector BoundingBoxLerp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
        bool bOnlyCollidingComponents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBoundsCheckParams")
        bool bIncludeFromChildActors;

    FActorBoundsCheckParams()
        : LineTraceChannel(ECollisionChannel::ECC_Visibility)
        , BoundingBoxLerp(FVector(1.0f))
        , bOnlyCollidingComponents(true)
        , bIncludeFromChildActors(false)
    {}

    FActorBoundsCheckParams(ECollisionChannel InLineTraceChannel, float InBoundingBoxLerp, bool bInOnlyCollidingComponents, bool bInIncludeFromChildActors)
        : LineTraceChannel(InLineTraceChannel)
        , BoundingBoxLerp(FVector(InBoundingBoxLerp))
        , bOnlyCollidingComponents(bInOnlyCollidingComponents)
        , bIncludeFromChildActors(bInIncludeFromChildActors)
    {}

    FActorBoundsCheckParams(float InBoundingBoxLerp)
        : LineTraceChannel(ECollisionChannel::ECC_Visibility)
        , BoundingBoxLerp(FVector(InBoundingBoxLerp))
        , bOnlyCollidingComponents(true)
        , bIncludeFromChildActors(false)
    {}
};
