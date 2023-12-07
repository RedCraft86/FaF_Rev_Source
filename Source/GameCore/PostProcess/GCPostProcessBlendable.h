// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "GCPostProcessBlendable.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, DisplayName = "Smart Blendable")
class GAMECORE_API UGCPostProcessBlendable : public UObject
{
	GENERATED_BODY()

	friend class AGCPostProcessActor;

public:

	UGCPostProcessBlendable();

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
	
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() const { return DynamicInstance; }
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (DisplayPriority = -1))
		UMaterialInterface* ParentMaterial;

	UPROPERTY(Transient, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (DisplayPriority = -1))
		UMaterialInstanceDynamic* DynamicInstance;

	UPROPERTY(Transient, BlueprintReadOnly)
		const AGCPostProcessActor* PostProcessActor;
	
	float InterpTarget;
	float InterpSpeed;
	
	virtual void OnInit();
	virtual void OnTick(const float DeltaTime);

	void AssignBlendable();
};
