// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ElectricActor.h"
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

	AElectricLightBase() : bPreviewState(false), bBroken(false), MinEnergy(0), bCachedState(false) {}

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bPreviewState;
#endif

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bBroken;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		uint8 MinEnergy;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
		TObjectPtr<UMaterialInterface> FlickerFunction;

	UFUNCTION(BlueprintPure, Category = "ElectricActor|Light")
		bool GetState();

	UFUNCTION(BlueprintCallable, Category = "ElectricActor|Light")
		void SetBrokenState(const bool bNewBroken);
	
	UFUNCTION(BlueprintImplementableEvent)
		void GetLightInfo(TSet<ULightComponent*>& Lights, TArray<FLightMeshData>& Meshes) const;
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "State Changed")
		void StateChangedEvent(const bool bState);

protected:

	bool bCachedState;
	
	void OnLightUpdate(const bool bState);
	void OnEnergyChanged(const uint8 Total) override;
	
	virtual void BeginPlay() override;
#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
};