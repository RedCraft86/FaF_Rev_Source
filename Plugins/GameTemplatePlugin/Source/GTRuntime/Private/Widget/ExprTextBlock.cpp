// Copyright (C) RedCraft86. All Rights Reserved.

#include "ExprTextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/ExpressiveTextRendererWidget.h"

UExprTextBlock::UExprTextBlock(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	ExpressiveText = {};
	RootBorder = nullptr;
	Renderer = nullptr;
	CachedChecksum = 0;
#if WITH_EDITORONLY_DATA
	PaletteCategory = NSLOCTEXT("UMG", "Common", "Common");
#endif
}

void UExprTextBlock::SetText(const FText InText, const bool bUseFieldsFromAsset)
{
	ExpressiveText.SetText(InText, bUseFieldsFromAsset);
}

void UExprTextBlock::SetTextFields(const FExpressiveTextFields& InFields)
{
	ExpressiveText.SetTextFields(InFields);
}

void UExprTextBlock::SetTextAsset(UExpressiveTextAsset* InAsset)
{
	ExpressiveText.SetTextAsset(InAsset);
}

void UExprTextBlock::UpdateText()
{
	CachedSize = CurrentSize;
	CachedChecksum = ExpressiveText.CalcChecksum();

	FExpressiveText GenExprText = ExpressiveText.GetExpressiveText();
	GenExprText.DefineWorldContext(this);
	if (Renderer) Renderer->SetExpressiveText(GenExprText);
}

bool UExprTextBlock::IsCacheInvalid() const
{
	return CachedChecksum != ExpressiveText.CalcChecksum() || !CachedSize.Equals(CurrentSize, 0.001f);
}

void UExprTextBlock::NativePreConstruct()
{
	Super::NativePreConstruct();
	CurrentSize = GetCachedGeometry().GetLocalSize();
	UpdateText();
}

void UExprTextBlock::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	CurrentSize = MyGeometry.GetLocalSize();
	if (!IsCacheInvalid()) return;
	UpdateText();
}

bool UExprTextBlock::Initialize()
{
	const bool bResult = Super::Initialize();
	
	RootBorder = Cast<UBorder>(GetRootWidget());
	if (!RootBorder && WidgetTree)
	{
		RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RootBorder"));
		RootBorder->Background.DrawAs = ESlateBrushDrawType::NoDrawType;
		RootBorder->SetVisibility(ESlateVisibility::Visible);
		WidgetTree->RootWidget = RootBorder;
	}
	
	if (!Renderer && RootBorder && WidgetTree)
	{
		Renderer = WidgetTree->ConstructWidget<UExpressiveTextRendererWidget>(
			UExpressiveTextRendererWidget::StaticClass(), TEXT("Renderer"));
		Renderer->SetVisibility(ESlateVisibility::HitTestInvisible);
		RootBorder->AddChild(Renderer);
	}
	
	UpdateText();
	return bResult;
}
