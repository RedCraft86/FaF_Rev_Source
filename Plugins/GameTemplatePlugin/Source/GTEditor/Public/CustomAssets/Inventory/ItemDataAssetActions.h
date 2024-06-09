// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTEditorStyle.h"
#include "CustomAssets/GTAssetTypeActions.h"
#include "Inventory/InventoryItemDataBase.h"
#include "Widgets/Layout/SScaleBox.h"

class FItemDataAssetTypeActions final : public FGTAssetTypeActions
{
public:
	
	explicit FItemDataAssetTypeActions() {}
	virtual FColor GetTypeColor() const override { return FColor(100, 25, 160); }
	virtual UClass* GetSupportedClass() const override { return UInventoryItemDataBase::StaticClass(); }
	virtual FText GetName() const override { return NSLOCTEXT("GTEditor", "ItemDataName", "Inventory Item Data"); }
	virtual TSharedPtr<SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override
	{
		const UInventoryItemDataBase* Asset = Cast<UInventoryItemDataBase>(AssetData.GetAsset());
		if (UTexture2D* Icon = IsValid(Asset) ? Asset->DisplayIcon.LoadSynchronous() : nullptr)
		{
			FSlateBrush* Brush = new FSlateBrush();
			Brush->SetResourceObject(Icon);
			Brush->DrawAs = ESlateBrushDrawType::Image;
			Brush->ImageSize = FIntPoint(Icon->GetSizeX(), Icon->GetSizeY());

			return SNew(SOverlay)
				.Visibility(EVisibility::HitTestInvisible)
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(0.0f, 0.0f, 0.0f, 2.0f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush(TEXT("ContentBrowser.AssetTileItem.ThumbnailAreaBackground")))
					.Padding(FMargin(10.0f))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SScaleBox)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Stretch(EStretch::ScaleToFit)
						[
							SNew(SImage).Image(Brush)
						]
					]
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(2.0f, 4.0f)
				[
					SNew(SImage).Image(FGTEditorStyle::Get().GetBrush(TEXT("SmallIcon.InventoryGrid")))
				];
		}

		return FAssetTypeActions_Base::GetThumbnailOverlay(AssetData);
	}
};
