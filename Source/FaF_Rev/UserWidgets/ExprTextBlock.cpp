// Copyright (C) RedCraft86. All Rights Reserved.

#include "ExprTextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

// REQUIRES: EXPRESSIVETEXT_API being added to FExpressiveTextSelector

bool UExprTextBlock::IsCacheInvalid() const
{
	return CachedChecksum != ExprText.CalcChecksum() || CachedSize.Equals(CurrentSize, 0.001f);
}

void UExprTextBlock::NativePreConstruct()
{
	Super::NativePreConstruct();
	CurrentSize = GetCachedGeometry().GetLocalSize();
	UpdateText();
}

void UExprTextBlock::UpdateText()
{
	CachedSize = CurrentSize;
	CachedChecksum = ExprText.CalcChecksum();

	FExpressiveText GenExprText = ExprText.GenerateExpressiveText();
	GenExprText.DefineWorldContext(this);
	Renderer->SetExpressiveText(GenExprText);
}

void UExprTextBlock::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());
	Border = WidgetTree->ConstructWidget<UBorder>();
	RootWidget->AddChild(Border);
	
	Renderer = WidgetTree->ConstructWidget<UExpressiveTextRendererWidget>();
	Border->AddChild(Renderer);
}

void UExprTextBlock::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	CurrentSize = MyGeometry.GetLocalSize();
	if (!IsCacheInvalid()) return;
	UpdateText();
}
