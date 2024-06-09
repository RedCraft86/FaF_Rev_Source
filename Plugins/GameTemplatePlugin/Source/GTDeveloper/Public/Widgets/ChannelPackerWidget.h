// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EditorUtilityWidget.h"
#include "ChannelPackerWidget.generated.h"

class UImage;
class UButton;

UCLASS(Abstract)
class GTDEVELOPER_API UGTChannelPackerWidget final : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UGTChannelPackerWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "ChannelPacker")
		void Update() const { UpdateMaterials(); }
	
	UFUNCTION(BlueprintCallable, Category = "ChannelPacker")
		void Reset();

	inline static TObjectPtr<UTexture2D> DefaultTexture = nullptr;
	inline static TObjectPtr<UMaterialInterface> PackerMaterial = nullptr;
	inline static TObjectPtr<UMaterialInterface> UnpackerMaterial = nullptr;

private:
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> PackingButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> UnpackingButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ProcessButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UTextBlock> ProcessLabel;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UWidgetSwitcher> PackerWidgets;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ImageRGBA;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ImageR;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ImageG;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ImageB;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ImageA;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TObjectPtr<UTexture2D> InputR;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TObjectPtr<UTexture2D> InputG;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TObjectPtr<UTexture2D> InputB;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TObjectPtr<UTexture2D> InputA;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TObjectPtr<UTexture2D> InputRGBA;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TEnumAsByte<TextureCompressionSettings> CompressionSettings;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		TEnumAsByte<TextureMipGenSettings> MipGenSettings;

	UPROPERTY(EditAnywhere, Transient, Category = "Data", meta = (EditCondition = "CompressionSettings != TextureCompressionSettings::TC_Masks && CompressionSettings != TextureCompressionSettings::TC_HDR"))
		bool bSRGB;
	
	UPROPERTY(EditAnywhere, Transient, Category = "Data", meta = (ClampMin = 8, UIMin = 8))
		int32 MaxSize;
	
	UPROPERTY(EditAnywhere, Transient, Category = "Data", meta = (ContentDir, RelativeToGameContentDir))
		FDirectoryPath ExportPath;

	UPROPERTY(EditAnywhere, Transient, Category = "Data")
		FString AssetName;

	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> MIDOutputRGBA;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> MIDOutputR;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> MIDOutputG;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> MIDOutputB;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> MIDOutputA;

	UFUNCTION() void PackingButtonEvent();
	UFUNCTION() void UnpackingButtonEvent();
	UFUNCTION() void ProcessButtonEvent();

	void ExportPacked();
	void ExportUnpacked();
	void UpdateMaterials() const;
	FVector2D GetFinalRGBASize() const;
	void SaveTextureAsset(UTextureRenderTarget2D* RenderTarget, const FString& Suffix, const bool bHasAlpha);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
