// Copyright (C) RedCraft86. All Rights Reserved.

#include "WatermarkWidget.h"
#include "Components/Image.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Libraries/GTTextureUtilsLibrary.h"

void UWatermarkWidgetBase::ClearImage()
{
	FSlateBrush Brush = BaseImage->GetBrush();
	Brush.SetResourceObject(nullptr);
	BaseImage->SetBrush(Brush);
	ImageTex = nullptr;
}

UTexture2D* UWatermarkWidgetBase::AddWatermark(UTexture2D* InTexture)
{
	if (InTexture)
	{
		ImageTex = InTexture;
		BaseImage->SetBrushFromTexture(ImageTex);
		if (UTextureRenderTarget2D* RT = UKismetRenderingLibrary::CreateRenderTarget2D(this,
			ImageTex->GetSizeX(), ImageTex->GetSizeY()))
		{
			UGTTextureUtilsLibrary::DrawWidgetToRenderTarget(RT, this);
			ImageTex = UGTTextureUtilsLibrary::ConvertRenderTargetToTexture(RT);
			RT = nullptr;
		}
	}
	
	return ImageTex;
}
