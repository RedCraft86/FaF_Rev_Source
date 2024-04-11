// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExpressiveTextData.h"
#include "Blueprint/UserWidget.h"
#include "ExprTextBlock.generated.h"

class UBorder;
class UExpressiveTextRendererWidget;

UCLASS(NotBlueprintable, DisplayName = "Expressive Text Block")
class FAF_REV_API UExprTextBlock final : public UUserWidget
{
	GENERATED_BODY()

public:

	UExprTextBlock(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (ShowOnlyInnerProperties))
		FExpressiveTextData ExpressiveText;

	UBorder* GetRootBorder() const { return RootBorder; } 
	UExpressiveTextRendererWidget* GetRenderer() const { return Renderer; }

private:

	UPROPERTY(BlueprintReadOnly, Category = "Elements", meta = (AllowPrivateAccess = true))
		UBorder* RootBorder;
	
	UPROPERTY(BlueprintReadOnly, Category = "Elements", meta = (AllowPrivateAccess = true))
		UExpressiveTextRendererWidget* Renderer;

	FVector2D CurrentSize, CachedSize;
	int64 CachedChecksum;
	
	void UpdateText();
	bool IsCacheInvalid() const;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual bool Initialize() override;
};
