// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExpressiveTextData.h"
#include "Blueprint/UserWidget.h"
#include "ExprTextBlock.generated.h"

class UBorder;
class UExpressiveTextRendererWidget;

UCLASS(NotBlueprintable, DisplayName = "Expressive Text Block")
class GTRUNTIME_API UExprTextBlock final : public UUserWidget
{
	GENERATED_BODY()

public:

	UExprTextBlock(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (ShowOnlyInnerProperties))
		FExpressiveTextData ExpressiveText;

	UFUNCTION(BlueprintCallable, Category = "Expressive Text Block")
		void SetText(const FText InText, const bool bUseFieldsFromAsset = true);

	UFUNCTION(BlueprintCallable, Category = "Expressive Text Block")
		void SetTextFields(const FExpressiveTextFields& InFields);

	UFUNCTION(BlueprintCallable, Category = "Expressive Text Block")
		void SetTextAsset(UExpressiveTextAsset* InAsset);

	UBorder* GetRootBorder() const { return RootBorder; } 
	UExpressiveTextRendererWidget* GetRenderer() const { return Renderer; }

private:

	UPROPERTY(BlueprintReadOnly, Category = "Elements", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBorder> RootBorder;
	
	UPROPERTY(BlueprintReadOnly, Category = "Elements", meta = (AllowPrivateAccess = true))
		TObjectPtr<UExpressiveTextRendererWidget> Renderer;
	
	int64 CachedChecksum;
	FVector2D CurrentSize, CachedSize;
	
	void UpdateText();
	bool IsCacheInvalid() const;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual bool Initialize() override;
};
