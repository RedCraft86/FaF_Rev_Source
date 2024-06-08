// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

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

	AElectricLightBase() : bPreviewState(true), bFlicker(false) { MinEnergy = 0; }

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		bool bPreviewState;
#endif

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		bool bFlicker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FLightMeshProperties MeshProperties;

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UMaterialInterface> FlickerFunction;

	UFUNCTION(BlueprintCallable, Category = "ElectricActor|Light")
		void SetFlickerState(const bool bNewFlicker);
	
	UFUNCTION(BlueprintImplementableEvent)
		void GetLightInfo(TSet<ULightComponent*>& Lights, TMap<UStaticMeshComponent*, bool>& Meshes) const;

protected:
	
	void OnFlickerUpdate() const;
	void OnStateChanged(const bool bState) override;

#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

public: // Statics

	UFUNCTION(BlueprintCallable, Category = "ElectronicActor|Light", meta = (DefaultToSelf = "Target"))
		static void SetLightMeshSettings(UStaticMeshComponent* Target, const ULightComponent* Source, const FLightMeshProperties& Properties);
};