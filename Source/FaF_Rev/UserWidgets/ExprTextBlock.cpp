// Copyright (C) RedCraft86. All Rights Reserved.

#include "ExprTextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/ExpressiveTextRendererWidget.h"

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
	Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	RootWidget->AddChild(Border);
	
	Renderer = WidgetTree->ConstructWidget<UExprTextRenderer>(UExprTextRenderer::StaticClass(), TEXT("Renderer"));
	Border->AddChild(Renderer);
}

void UExprTextBlock::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	CurrentSize = MyGeometry.GetLocalSize();
	if (!IsCacheInvalid()) return;
	UpdateText();
}
