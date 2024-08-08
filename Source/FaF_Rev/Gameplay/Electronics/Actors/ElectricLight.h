// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/InlineCurves.h"
#include "Optimization/SmartCulling.h"
#include "Electronics/ElectricActor.h"
#include "ElectricLight.generated.h"

USTRUCT(BlueprintType)
struct FAF_REV_API FLightMeshProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "LightMeshProperties")
		float Intensity;

	UPROPERTY(EditAnywhere, Category = "LightMeshProperties")
		float Fresnel;

	UPROPERTY(EditAnywhere, Category = "LightMeshProperties")
		float Flicker;

	FLightMeshProperties() : Intensity(1.0f), Fresnel(0.5f), Flicker(1.0f) {}
};

/*
* Mesh Primitive Idx 0 -> 3	: Color and Base Intensity
* Mesh Primitive Idx 4		: Fresnel
* Mesh Primitive Idx 5		: Intensity Multi
* Mesh Primitive Idx 6		: Flicker Amount
*/
UCLASS(Abstract)
class FAF_REV_API AElectricLightBase final : public AElectricActorBase
{
	GENERATED_BODY()

public:

	AElectricLightBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USmartCullingComponent> SmartCulling;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		bool bPreviewState = true;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		bool bFlicker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FLightMeshProperties MeshProperties;

	UPROPERTY(EditAnywhere, Category = "Settings|Flicker", meta = (ClampMin = 0.01f, UIMin = 0.01f))
		float FlickerRate;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Flicker")
		FVector2D LightFlickerRange;

	UPROPERTY(EditAnywhere, Category = "Settings|Flicker")
		FVector2D MeshFlickerRange;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Flicker")
		FInlineFloatCurve FlickerCurve;

	UFUNCTION(BlueprintCallable, Category = "ElectricActor|Light")
		void SetFlickerState(const bool bNewFlicker);
	
	UFUNCTION(BlueprintImplementableEvent)
		void GetLightInfo(TSet<ULightComponent*>& Lights, TMap<UStaticMeshComponent*, bool>& Meshes) const;

protected:

	UPROPERTY() float FlickerTime;
	UPROPERTY() FVector2D FlickerRange;
	UPROPERTY() FVector2D FlickerTimeRange;
	UPROPERTY() TSet<ULightComponent*> CachedLights;
	UPROPERTY() TMap<ULightComponent*, float> CachedIntensity;
	UPROPERTY() TMap<UStaticMeshComponent*, bool> CachedMeshes;
	
	void OnFlickerChanged(const bool bState);
	void OnStateChanged(const bool bState) override;
	void OnFlickerUpdate(const float DeltaTime);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif
public: // Statics

	UFUNCTION(BlueprintCallable, Category = "ElectronicActor|Light", meta = (DefaultToSelf = "Target"))
		static void SetLightMeshSettings(UStaticMeshComponent* Target, const ULightComponent* Source, const FLightMeshProperties& Properties);
};