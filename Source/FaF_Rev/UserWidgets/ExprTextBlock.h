// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Handles/ExpressiveTextSelector.h"
#include "Widgets/ExpressiveTextRendererWidget.h"
#include "ExprTextBlock.generated.h"

UCLASS(DisplayName = "Expressive Text")
class FAF_REV_API UExprTextBlock final : public UUserWidget
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text", meta = (AllowPrivateAccess = true))
		FExpressiveTextSelector ExprText;

	UPROPERTY(BlueprintReadOnly, Category = "Elements", meta = (AllowPrivateAccess = true))
		class UBorder* Border;
	
	UPROPERTY(BlueprintReadOnly, Category = "Elements", meta = (AllowPrivateAccess = true))
		UExpressiveTextRendererWidget* Renderer;

	FVector2D CurrentSize, CachedSize;
	int64 CachedChecksum;
	
	void UpdateText();
	bool IsCacheInvalid() const;
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
