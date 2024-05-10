// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "NarrativeComponent.h"
#include "NarrativeWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UNarrativeWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()
	
public:

	UNarrativeWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidgetAnim))
		UWidgetAnimation* HideFadeAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "NarrativeWidget")
		UNarrativeComponent* NarrativeComponent;

protected:

	float HideCheckTime;
	UPROPERTY(Transient) AWorldSettings* WorldSettings;
	UPROPERTY(Transient) class AFRPlayerBase* PlayerChar;

	void HideCheck();
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
