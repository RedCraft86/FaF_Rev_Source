// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTTextureUtilsLibrary.generated.h"

USTRUCT(BlueprintType, DisplayName = "Texture Data")
struct FGTTextureData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureData", SaveGame)
		TArray<FColor> Pixels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureData", SaveGame)
		FIntPoint Size;
	
	FGTTextureData() : Pixels({}), Size({0}) {}
	FGTTextureData(const TArray<FColor>& InPixels, const int32 SizeX, const int32 SizeY)
		: Pixels(InPixels), Size({SizeX, SizeY}) {}
	
	friend FArchive& operator<<(FArchive& Ar, FGTTextureData& TextureData)
	{
		Ar << TextureData.Pixels;
		Ar << TextureData.Size;
		return Ar;
	}
	
	bool HasValidData() const { return Pixels.Num() == Size.X * Size.Y; }
};

UCLASS(DisplayName = "Texture Utils Library")
class GTCORE_API UGTTextureUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static void DrawWidgetToRenderTarget(UTextureRenderTarget2D* InRenderTarget, UUserWidget* InUserWidget);
	
	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static FGTTextureData GetDataFromRenderTarget(UTextureRenderTarget2D* InRenderTarget, const bool bInvertAlpha);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static FGTTextureData GetDataFromTexture(const UTexture2D* InTexture);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static UTexture2D* CreateTextureFromData(const FGTTextureData& InData);
	
	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static UTexture2D* ConvertRenderTargetToTexture(UTextureRenderTarget2D* InRenderTarget, const bool bHasAlpha);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static void SaveTextureDataToFile(const FGTTextureData& InData, const FString InPath, const bool bAsync = true);
	
	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static void SaveTextureToFile(const UTexture2D* InTexture, const FString InPath, const bool bAsync = true);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
		static void SaveRenderTargetToFile(UTextureRenderTarget2D* InRenderTarget, const FString InPath, const bool bHasAlpha, const bool bAsync = true);
};
