// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCWatermarkWidget.h"
#include "Components/Image.h"
#include "Slate/WidgetRenderer.h"
#include "Engine/CanvasRenderTarget2D.h"

UGCWatermarkWidget::UGCWatermarkWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	BackgroundImage = nullptr;
}

UCanvasRenderTarget2D* UGCWatermarkWidget::ProcessImage(UTexture2D* InTexture)
{
	if (InTexture)
	{
		BackgroundImage->SetBrushFromTexture(InTexture);
		const FVector2D Size(InTexture->GetSizeX(), InTexture->GetSizeY());
		if (UCanvasRenderTarget2D* RenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this,
			UCanvasRenderTarget2D::StaticClass(), Size.X, Size.Y))
		{
			RenderTarget->InitCustomFormat(Size.X, Size.Y, PF_B8G8R8A8, true);
			AdjustWatermark(Size);

			const TSharedRef<SWidget> Widget = TakeWidget();
			FWidgetRenderer* Renderer = new FWidgetRenderer(true, false);
			Renderer->DrawWidget(RenderTarget, Widget, Widget->GetCachedGeometry().Scale, Size, 0.0f);

			// Cleanup
			FlushRenderingCommands();
			BeginCleanup(Renderer);
			BackgroundImage->SetBrushFromTexture(nullptr);

			return RenderTarget;
		}
	}

	return nullptr;
}