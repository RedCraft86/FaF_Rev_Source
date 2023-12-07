// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once 

#include "RCPrimitiveTypes.generated.h"

USTRUCT(BlueprintType, DisplayName = "Static Mesh Properties")
struct RCRUNTIME_API FRCStaticMeshProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMeshProperties")
        UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMeshProperties")
        TArray<TObjectPtr<UMaterialInterface>> Materials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMeshProperties")
        bool bCastShadows;

    FRCStaticMeshProperties() : Mesh(nullptr), Materials({}), bCastShadows(true) {}
    FRCStaticMeshProperties(UStaticMesh* InMesh, const TArray<UMaterialInterface*>& InMaterials = {}, const bool bInCastShadows = true)
        : Mesh(InMesh)
        , Materials(InMaterials)
        , bCastShadows(bInCastShadows)
    {}

    friend FArchive& operator<<(FArchive& Ar, FRCStaticMeshProperties& Properties)
    {
        Ar << Properties.Mesh;
        Ar << Properties.Materials;
        Ar << Properties.bCastShadows;
        return Ar;
    }

    bool IsValidMesh() const
    {
        return IsValid(Mesh);
    }

    void FillMaterials()
    {
        if (!IsValidMesh())
        {
            Materials.Empty();
            return;
        }
        
        const TArray<FStaticMaterial>& Mats = Mesh->GetStaticMaterials();
        if (Materials.IsEmpty() || Materials.Num() < Mats.Num())
        {
            for (int32 i = 0; i < Mats.Num(); i++)
            {
                if (!Materials.IsValidIndex(i))
                {
                    Materials.Add(Mats[i].MaterialInterface);
                }
                else if (!Materials[i])
                {
                    Materials[i] = Mats[i].MaterialInterface;
                }
            }
        }
    }
};

USTRUCT(BlueprintType, DisplayName = "Primitive Collision", meta = (HasNativeMake = "/Script/RCRuntime.RCStructLibrary:MakePrimitiveCollision", HasNativeBreak = "/Script/RCRuntime.RCStructLibrary:BreakPrimitiveCollision"))
struct RCRUNTIME_API FRCPrimitiveCollision
{
    GENERATED_BODY()

    /* Collision Profile Name */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PrimitiveCollision")
        FName CollisionProfileName;

    /* Type of collision enabled.
    *
    *	No Collision      : Will not create any representation in the physics engine. Cannot be used for spatial queries (raycasts, sweeps, overlaps) or simulation (rigid body, constraints). Best performance possible (especially for moving objects)
    *	Query Only        : Only used for spatial queries (raycasts, sweeps, and overlaps). Cannot be used for simulation (rigid body, constraints). Useful for character movement and things that do not need physical simulation. Performance gains by keeping data out of simulation tree.
    *	Physics Only      : Only used only for physics simulation (rigid body, constraints). Cannot be used for spatial queries (raycasts, sweeps, overlaps). Useful for jiggly bits on characters that do not need per bone detection. Performance gains by keeping data out of query tree
    *	Collision Enabled : Can be used for both spatial queries (raycasts, sweeps, overlaps) and simulation (rigid body, constraints).
    */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PrimitiveCollision")
        TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;

    /* Enum indicating what type of object this should be considered as when it moves */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PrimitiveCollision")
        TEnumAsByte<ECollisionChannel> ObjectType;

    /* Channels and Responses */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PrimitiveCollision")
        TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> Responses;

    FRCPrimitiveCollision()
        : CollisionProfileName(UCollisionProfile::CustomCollisionProfileName)
        , CollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics)
        , ObjectType(ECC_WorldStatic)
        , Responses({})
    {
        FillDefaultResponses();
    }
    
    FRCPrimitiveCollision(const TEnumAsByte<ECollisionEnabled::Type> InCollisionEnabled, const ECollisionChannel InObjectType, const TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& Responses = {})
        : CollisionProfileName(UCollisionProfile::CustomCollisionProfileName)
        , CollisionEnabled(InCollisionEnabled)
        , ObjectType(InObjectType)
        , Responses(Responses)
    {
        FillDefaultResponses(ECR_Ignore);
    }

    FRCPrimitiveCollision(const FName& InProfileName)
    {
        SetCollisionProfile(InProfileName);
    }

    friend FArchive& operator<<(FArchive& Ar, FRCPrimitiveCollision& Data)
    {
        Ar << Data.CollisionProfileName;
        Ar << Data.CollisionEnabled;
        Ar << Data.ObjectType;
        Ar << Data.Responses;
        return Ar;
    }
    
    void FillDefaultResponses(const ECollisionResponse DefaultResponse = ECR_Block)
    {
        if (const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>())
        {
            for (int32 i = 0; i < ChannelEnum->NumEnums(); i++)
            {
                const TEnumAsByte<ECollisionChannel> EnumVal(i);
#if WITH_EDITORONLY_DATA
                if (!ChannelEnum->HasMetaData(TEXT("Hidden"), i) && !Responses.Contains(EnumVal) && EnumVal != ECC_MAX)
#else
                if (!Responses.Contains(EnumVal) && EnumVal != ECC_MAX)
#endif
                {
                    Responses.Add(EnumVal, DefaultResponse);
                }
            }
        }
    }

    void SetResponse(const ECollisionChannel Channel, const ECollisionResponse Response)
    {
        FillDefaultResponses();
        Responses.Add(Channel, Response);
    }

    void SetAllResponses(const ECollisionResponse Response)
    {
        if (const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>())
        {
            for (int32 i = 0; i < ChannelEnum->NumEnums(); i++)
            {
                const TEnumAsByte<ECollisionChannel> EnumVal(i);
                
#if WITH_EDITORONLY_DATA
                if (!ChannelEnum->HasMetaData(TEXT("Hidden"), i) && EnumVal != ECC_MAX)
#else
                if (EnumVal != ECC_MAX)
#endif
                {
                    Responses.Add(EnumVal, Response);
                }
            }
        }
    }

    void SetCollisionProfile(const FName& ProfileName)
    {
        CollisionProfileName = UCollisionProfile::CustomCollisionProfileName;
        if (const UCollisionProfile* Profile = UCollisionProfile::Get())
        {
            FCollisionResponseTemplate Template;
            if (Profile->GetProfileTemplate(ProfileName, Template))
            {
                CollisionProfileName = ProfileName;
                CollisionEnabled = Template.CollisionEnabled;
                ObjectType = Template.ObjectType;

                if (const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>())
                {
                    for (int32 i = 0; i < ChannelEnum->NumEnums(); i++)
                    {
                        const TEnumAsByte<ECollisionChannel> EnumVal(i);
                        
#if WITH_EDITORONLY_DATA
                        if (!ChannelEnum->HasMetaData(TEXT("Hidden"), i) && EnumVal != ECC_MAX)
#else
                        if (EnumVal != ECC_MAX)
#endif
                        {
                            Responses.Add(EnumVal, Template.ResponseToChannels.GetResponse(EnumVal));
                        }
                    }
                }
            }
        }
    }
    
    FString GetDebugString() const
    {
        FString Result = FString::Printf(TEXT("Profile Name: %s"), *CollisionProfileName.ToString());
#if WITH_EDITORONLY_DATA
        if (const UEnum* EnabledEnum = StaticEnum<ECollisionEnabled::Type>())
        {
            Result += FString::Printf(TEXT("\nEnabled: %s"),
                *EnabledEnum->GetDisplayNameTextByValue(CollisionEnabled).ToString());
        }
        
        if (const UEnum* ObjectTypeEnum = StaticEnum<ECollisionChannel>())
        {
            Result += FString::Printf(TEXT("\nObject Type: %s"),
                *ObjectTypeEnum->GetDisplayNameTextByValue(ObjectType).ToString());
        }

        const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>();
        const UEnum* ResponseEnum = StaticEnum<ECollisionResponse>();
        if (ChannelEnum && ResponseEnum)
        {
            FString TraceResult = TEXT("~ Trace Channels ~");
            FString ObjectResult = TEXT("~ Object Channels ~");

            for (int32 i = 0; i < ChannelEnum->NumEnums(); i++)
            {
                const TEnumAsByte<ECollisionChannel> ChannelVal(i);
                const FString ChannelName = ChannelEnum->GetMetaData(TEXT("DisplayName"), i);
                if (!ChannelName.IsEmpty() && ChannelVal != ECC_MAX)
                {
                    if (ChannelEnum->GetMetaData(TEXT("TraceQuery"), i) == TEXT("1"))
                    {
                        TraceResult += FString::Printf(TEXT("\n%s: %s"), *ChannelName,
                            *ResponseEnum->GetDisplayNameTextByValue(Responses.FindRef(ChannelVal)).ToString());
                    }
                    else
                    {
                        ObjectResult += FString::Printf(TEXT("\n%s: %s"), *ChannelName,
                            *ResponseEnum->GetDisplayNameTextByValue(Responses.FindRef(ChannelVal)).ToString());
                    }
                }
            }

            Result += FString::Printf(TEXT("\n%s\n%s"), *TraceResult, *ObjectResult);
        }
#endif
        return Result;
    }

    bool HasValidProfile() const { return IsProfileValid(CollisionProfileName); }

    static bool IsProfileValid(const FName& ProfileName)
    {
        if (ProfileName == UCollisionProfile::CustomCollisionProfileName)
            return false;
        
        if (const UCollisionProfile* Profile = UCollisionProfile::Get())
        {
            FCollisionResponseTemplate Template;
            return Profile->GetProfileTemplate(ProfileName, Template);
        }

        return false;
    }
};