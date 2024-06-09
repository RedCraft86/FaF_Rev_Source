// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Components/BillboardComponent.h"
#include "ActorIconData.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct GTRUNTIME_API FGTActorIconData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "ActorIconData")
		TObjectPtr<UTexture2D> Sprite;

	UPROPERTY(EditAnywhere, Category = "ActorIconData", meta = (InlineEditConditionToggle))
		bool bScreenSizeScaled;

	UPROPERTY(EditAnywhere, Category = "ActorIconData", meta = (EditCondition = "bScreenSizeScaled", ClampMin = 0.00025f, UIMin = 0.00025f))
		float ScreenSizeScale;

	UPROPERTY(EditAnywhere, Category = "ActorIconData", meta = (MakeEditWidget = true))
		FTransform Transform;
	
	FGTActorIconData(const FString& BaseIconPath = TEXT(""));
	void ApplyData(UBillboardComponent* InBillboard) const;

#if WITH_EDITOR
private: TSoftObjectPtr<UTexture2D> BaseSprite;
#endif
};