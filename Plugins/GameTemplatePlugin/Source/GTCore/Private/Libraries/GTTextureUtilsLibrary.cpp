// Copyright (C) RedCraft86. All Rights Reserved.

#include "Libraries/GTTextureUtilsLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"
#include "Blueprint/UserWidget.h"
#include "ImageUtils.h"

void UGTTextureUtilsLibrary::DrawWidgetToRenderTarget(UTextureRenderTarget2D* InRenderTarget, UUserWidget* InUserWidget)
{
	if (!IsValid(InRenderTarget) || !IsValid(InUserWidget)) return;
	
	const TSharedRef<SWidget> Widget = InUserWidget->TakeWidget();
	FWidgetRenderer* Renderer = new FWidgetRenderer(true, false);
	Renderer->DrawWidget(InRenderTarget, Widget, Widget->GetCachedGeometry().Scale,
		FVector2D(InRenderTarget->SizeX, InRenderTarget->SizeY), 0.0f);
}

FGTTextureData UGTTextureUtilsLibrary::GetDataFromRenderTarget(UTextureRenderTarget2D* InRenderTarget, const bool bInvertAlpha)
{
	if (!InRenderTarget) return {};
	
	FGTTextureData Result;
	InRenderTarget->GameThread_GetRenderTargetResource()->ReadPixels(Result.Pixels);
	Result.Size = {InRenderTarget->SizeX, InRenderTarget->SizeY};
	if (bInvertAlpha)
	{
		for (FColor& Pixel : Result.Pixels)
		{
			Pixel.A = (uint8)((int32)Pixel.A * -1 + 255);
		}
	}
	return Result;
}

FGTTextureData UGTTextureUtilsLibrary::GetDataFromTexture(const UTexture2D* InTexture)
{
	if (!InTexture) return {};

	FGTTextureData Result;
	const FTexturePlatformData* PlatformData = InTexture->GetPlatformData();
	Result.Size = {PlatformData->SizeX, PlatformData->SizeY};
	Result.Pixels.AddDefaulted(Result.Size.X * Result.Size.Y);
	const void* TexData = PlatformData->Mips[0].BulkData.LockReadOnly();
	FMemory::Memcpy(Result.Pixels.GetData(), TexData, sizeof(FColor) * Result.Pixels.Num());
	PlatformData->Mips[0].BulkData.Unlock();
	return Result;
}

UTexture2D* UGTTextureUtilsLibrary::CreateTextureFromData(const FGTTextureData& InData)
{
	if (!InData.HasValidData()) return nullptr;
	
	UTexture2D* Image = UTexture2D::CreateTransient(InData.Size.X, InData.Size.Y, PF_B8G8R8A8);
	FTexturePlatformData* PlatformData = Image->GetPlatformData();
	void* TextureData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, InData.Pixels.GetData(), sizeof(FColor) * InData.Size.X * InData.Size.Y);
	PlatformData->Mips[0].BulkData.Unlock();
	Image->SetPlatformData(PlatformData);
	Image->UpdateResource();
	
	return Image;
}

UTexture2D* UGTTextureUtilsLibrary::ConvertRenderTargetToTexture(UTextureRenderTarget2D* InRenderTarget, const bool bHasAlpha)
{
	return CreateTextureFromData(GetDataFromRenderTarget(InRenderTarget, bHasAlpha));
}

void UGTTextureUtilsLibrary::SaveTextureDataToFile(const FGTTextureData& InData, const FString InPath, const bool bAsync)
{
	AsyncTask(bAsync ? ENamedThreads::AnyBackgroundHiPriTask : ENamedThreads::GameThread, [InData, InPath]() {
		TArray64<uint8> ImageBinary;
		FImageUtils::PNGCompressImageArray(InData.Size.X, InData.Size.Y, InData.Pixels, ImageBinary);
		FFileHelper::SaveArrayToFile(ImageBinary, *(InPath + TEXT(".png")));
	});
}

void UGTTextureUtilsLibrary::SaveTextureToFile(const UTexture2D* InTexture, const FString InPath, const bool bAsync)
{
	const FGTTextureData Data = GetDataFromTexture(InTexture);
	SaveTextureDataToFile(Data, InPath, bAsync);
}

void UGTTextureUtilsLibrary::SaveRenderTargetToFile(UTextureRenderTarget2D* InRenderTarget, const FString InPath, const bool bHasAlpha, const bool bAsync)
{
	const FGTTextureData Data = GetDataFromRenderTarget(InRenderTarget, bHasAlpha);
	SaveTextureDataToFile(Data, InPath, bAsync);
}
