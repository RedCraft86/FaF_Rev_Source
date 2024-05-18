// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "SmartBlendable.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class FAF_REV_API USmartBlendable final : public UObject
{
	GENERATED_BODY()
	
	friend class ASmartPostProcess;

public:

	USmartBlendable()
		: bEnabled(true), Intensity(1.0f), bUseStencil(false), StencilValue(0), InterpTarget(1.0f), InterpSpeed(5.0f)
	{}

	/* Whether this blendable is enabled or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (DisplayPriority = -1))
		bool bEnabled;

	/* Intensity of this blendable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayPriority = -1, ClampMin = 0.0f, UIMin = 0.0f))
		float Intensity;

	/* Whether to mask with stencil */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (InlineEditConditionToggle))
		bool bUseStencil;
	
	/* Stencil index to mask */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (DisplayPriority = -1, EditCondition = "bUseStencil"))
		uint8 StencilValue;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Init")
		void EventInit();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Tick")
		void EventTick(const float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "SmartBlendable")
		void SetIntensity(const float NewValue);
	
	UFUNCTION(BlueprintCallable, Category = "SmartBlendable")
		void AdjustIntensity(const float NewValue, const float Speed);
	
	UMaterialInstanceDynamic* GetDynamicMaterialInstance() const { return DynamicInstance; }
	virtual UWorld* GetWorld() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (DisplayPriority = -1))
		TObjectPtr<UMaterialInterface> ParentMaterial;

	UPROPERTY(Transient, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (DisplayPriority = -1))
		TObjectPtr<UMaterialInstanceDynamic> DynamicInstance;

	UPROPERTY(Transient, BlueprintReadOnly)
		TObjectPtr<const ASmartPostProcess> PostProcessActor;

	float InterpTarget, InterpSpeed;

	void OnInitialize();
	void AssignBlendable();
	void OnTick(const float DeltaTime);
};
