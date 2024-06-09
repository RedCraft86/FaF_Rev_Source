// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PrimitiveData.generated.h"

USTRUCT(BlueprintType)
struct GTCORE_API FStaticMeshProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshProperties")
        TSoftObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshProperties")
        TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshProperties")
        bool bCastShadows;

    FStaticMeshProperties() : Mesh(nullptr), Materials({}), bCastShadows(true) {}
    FStaticMeshProperties(const TSoftObjectPtr<UStaticMesh>& InMesh, const TArray<TSoftObjectPtr<UMaterialInterface>>& InMaterials = {}, const bool bInCastShadows = true)
        : Mesh(InMesh)
        , Materials(InMaterials)
        , bCastShadows(bInCastShadows)
    {}
    
    friend FArchive& operator<<(FArchive& Ar, FStaticMeshProperties& Properties)
    {
        Ar << Properties.Mesh;
        Ar << Properties.Materials;
        Ar << Properties.bCastShadows;
        return Ar;
    }
    
    void FillMaterials();
    FORCEINLINE bool IsValidData() const { return !Mesh.IsNull(); }
    void FromStaticMeshComponent(const UStaticMeshComponent* InComponent);
    void ToStaticMeshComponent(UStaticMeshComponent* InComponent) const;
};

USTRUCT(BlueprintType)
struct GTCORE_API FTransformMeshData : public FStaticMeshProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshProperties", meta = (DisplayPriority = 1))
        FTransform Transform;

    FTransformMeshData() : Transform(FTransform::Identity) {}
    FStaticMeshProperties operator*() const { return {Mesh, Materials, bCastShadows}; }
};

USTRUCT(BlueprintType, meta = (DisableSplitPin, HasNativeMake = "/Script/GTCore.PrimitiveDataLibrary.Make_PrimitiveCollision", HasNativeBreak = "/Script/GTCore.PrimitiveDataLibrary.Break_PrimitiveCollision"))
struct GTCORE_API FPrimitiveCollision
{
#if WITH_EDITOR
    friend class FPrimitiveCollisionDetails;
#endif
    
    GENERATED_BODY()

    FPrimitiveCollision()
        : CollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics)
        , ObjectType(ECC_WorldStatic)
        , Responses({})
    {
        SetProfileName(UCollisionProfile::BlockAll_ProfileName);
    }
    
    FPrimitiveCollision(const FName& ProfileName)
        : CollisionProfileName(NAME_None)
    {
        SetProfileName(ProfileName);
    }
    
    FPrimitiveCollision(const TEnumAsByte<ECollisionEnabled::Type> InCollisionEnabled, const TEnumAsByte<ECollisionChannel> InObjectType, const TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& InResponses = {})
        : CollisionEnabled(InCollisionEnabled)
        , ObjectType(InObjectType)
        , CollisionProfileName(UCollisionProfile::CustomCollisionProfileName)
        , Responses(InResponses)
    {
        FillResponsesMap();
    }

    friend FArchive& operator<<(FArchive& Ar, FPrimitiveCollision& Data)
    {
        Ar << Data.CollisionProfileName;
        Ar << Data.CollisionEnabled;
        Ar << Data.ObjectType;
        Ar << Data.Responses;
        return Ar;
    }

    void SetProfileName(const FName& InName);
    FName GetProfileName() const { return CollisionProfileName; }
    void SetCollisionEnabled(const ECollisionEnabled::Type InCollisionEnabled);
    ECollisionEnabled::Type GetCollisionEnabled() const { return CollisionEnabled; }
    void SetObjectType(const ECollisionChannel InObjectType);
    ECollisionChannel GetObjectType() const { return ObjectType; }
    void SetAllResponses(const ECollisionResponse InResponse);
    void SetResponse(const ECollisionChannel InChannel, const ECollisionResponse InResponse);
    ECollisionResponse GetResponse(const ECollisionChannel InChannel) const;

    void SetResponsesMap(const TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& InResponses) { Responses = InResponses; }
    TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> GetResponsesMap() const { return Responses; }

    void FillResponsesMap(const ECollisionResponse InResponses = ECR_Block);
    void FromPrimitiveComponent(const UPrimitiveComponent* InComponent);
    void ToPrimitiveComponent(UPrimitiveComponent* InComponent) const;
    
private:

    /* Type of collision enabled.
    * No Collision      : Will not create any representation in the physics engine. Cannot be used for spatial queries (raycasts, sweeps, overlaps) or simulation (rigid body, constraints). Best performance possible (especially for moving objects)
    * Query Only        : Only used for spatial queries (raycasts, sweeps, and overlaps). Cannot be used for simulation (rigid body, constraints). Useful for character movement and things that do not need physical simulation. Performance gains by keeping data out of simulation tree.
    * Physics Only      : Only used only for physics simulation (rigid body, constraints). Cannot be used for spatial queries (raycasts, sweeps, overlaps). Useful for jiggly bits on characters that do not need per bone detection. Performance gains by keeping data out of query tree
    * Collision Enabled : Can be used for both spatial queries (raycasts, sweeps, overlaps) and simulation (rigid body, constraints).
    */
    UPROPERTY(EditAnywhere, Category = "PrimitiveCollision") TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;

    /* Enum indicating what type of object this should be considered as when it moves */
    UPROPERTY(EditAnywhere, Category = "PrimitiveCollision") TEnumAsByte<ECollisionChannel> ObjectType;
    
    /* Collision Profile Name */
    UPROPERTY() FName CollisionProfileName;
    
    /* Channels and Responses */
    UPROPERTY() TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> Responses;
};

UCLASS(DisplayName = "Primitive Function Library")
class GTCORE_API UPrimitiveDataLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "StaticMesh|MeshProperties")
        static void FillMaterials(UPARAM(ref) FStaticMeshProperties& Target);

    UFUNCTION(BlueprintPure, Category = "StaticMesh|MeshProperties", DisplayName = "Is Valid")
        static bool IsValid_MeshProperties(const FStaticMeshProperties& Target);

    UFUNCTION(BlueprintPure, Category = "Collision|PrimitiveCollision", meta = (BlueprintThreadSafe, AutoCreateRefTerm = "InResponses", AdvancedDisplay = "InCollisionEnabled, InObjectType, InResponses"))
        static FPrimitiveCollision Make_PrimitiveCollision(const FName InProfileName, const TEnumAsByte<ECollisionEnabled::Type> InCollisionEnabled,
            const TEnumAsByte<ECollisionChannel> InObjectType, const TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& InResponses);

    UFUNCTION(BlueprintPure, Category = "Collision|PrimitiveCollision", meta = (BlueprintThreadSafe, AdvancedDisplay = "ProfileName, CollisionEnabled, ObjectType, Responses"))
        static void Break_PrimitiveCollision(const FPrimitiveCollision& Target, FName& ProfileName, TEnumAsByte<ECollisionEnabled::Type>& CollisionEnabled,
            TEnumAsByte<ECollisionChannel>& ObjectType, TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>>& Responses);

    UFUNCTION(BlueprintCallable, Category = "Collision|PrimitiveCollision")
        static void SetCollisionProfileName(UPARAM(ref) FPrimitiveCollision& Target, const FName InProfileName);

    UFUNCTION(BlueprintPure, Category = "Collision|PrimitiveCollision", meta = (BlueprintAutocast = true))
        static FName GetCollisionProfileName(const FPrimitiveCollision& Target);

    UFUNCTION(BlueprintCallable, Category = "Collision|PrimitiveCollision")
        static void SetAllCollisionResponses(UPARAM(ref) FPrimitiveCollision& Target, const ECollisionResponse InResponse);
    
    UFUNCTION(BlueprintCallable, Category = "Collision|PrimitiveCollision")
        static void SetCollisionResponse(UPARAM(ref) FPrimitiveCollision& Target, const ECollisionChannel InChannel, const ECollisionResponse InResponse);

    UFUNCTION(BlueprintPure, Category = "Collision|PrimitiveCollision")
        static ECollisionResponse GetCollisionResponse(const FPrimitiveCollision& Target, const ECollisionChannel InChannel);

    UFUNCTION(BlueprintCallable, Category = "Collision|PrimitiveCollision")
        static void SetCollisionEnabled(UPARAM(ref) FPrimitiveCollision& Target, const ECollisionEnabled::Type InEnabled);

    UFUNCTION(BlueprintPure, Category = "Collision|PrimitiveCollision")
        static ECollisionEnabled::Type GetCollisionEnabled(const FPrimitiveCollision& Target);

    UFUNCTION(BlueprintCallable, Category = "Collision|PrimitiveCollision")
        static void SetCollisionObjectType(UPARAM(ref) FPrimitiveCollision& Target, const ECollisionChannel InObjectType);

    UFUNCTION(BlueprintPure, Category = "Collision|PrimitiveCollision")
        static ECollisionChannel GetCollisionObjectType(const FPrimitiveCollision& Target);

    UFUNCTION(BlueprintPure, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
        static FStaticMeshProperties GetStaticMeshProperties(const UStaticMeshComponent* Target);

    UFUNCTION(BlueprintCallable, Category = "Components|StaticMesh", meta = (DefaultToSelf = "Target"))
        static void SetStaticMeshProperties(UStaticMeshComponent* Target, const FStaticMeshProperties& Properties);

    UFUNCTION(BlueprintPure, Category = "Collision", meta = (DefaultToSelf = "Target"))
        static FPrimitiveCollision GetPrimitiveCollision(const UPrimitiveComponent* Target);

    UFUNCTION(BlueprintCallable, Category = "Collision", meta = (DefaultToSelf = "Target"))
        static void SetPrimitiveCollision(UPrimitiveComponent* Target, const FPrimitiveCollision& Properties);
};
