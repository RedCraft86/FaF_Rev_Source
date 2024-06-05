// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Electronics/ElectricActor.h"
#include "ElectricLight.generated.h"

USTRUCT(BlueprintType)
struct FAF_REV_API FLightMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightMeshData")
		TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightMeshData")
		bool bHideCompletely;

	// Index -> Off, On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightMeshData")
		TMap<uint8, FVector2D> PrimitiveValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightMeshData")
		uint8 FlickerIndex;
};

UCLASS(Abstract, meta = (AllowedCategories = "Rendering"))
class FAF_REV_API AElectricLightBase final : public AElectricActorBase
{
	GENERATED_BODY()

public:

	AElectricLightBase() : bPreviewState(false), bFlicker(false) { MinEnergy = 0; }

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bPreviewState;
#endif

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bFlicker;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
		TObjectPtr<UMaterialInterface> FlickerFunction;

	UFUNCTION(BlueprintCallable, Category = "ElectricActor|Light")
		void SetFlickerState(const bool bNewFlicker);
	
	UFUNCTION(BlueprintImplementableEvent)
		void GetLightInfo(TSet<ULightComponent*>& Lights, TArray<FLightMeshData>& Meshes) const;

protected:
	
	void OnFlickerUpdate() const;
	void OnStateChanged(const bool bState) override;

#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
};