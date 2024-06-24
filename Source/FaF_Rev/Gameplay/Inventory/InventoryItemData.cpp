// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryItemData.h"

FText UInventoryItemData::GetDisplayName(const TMap<FName, FString>& InMetadata) const
{
	FText NameFmt = DisplayName;
	if (const FString NameStr = InMetadata.FindRef(NativeItemKeys::DisplayName); !NameStr.IsEmpty())
	{
		NameFmt = FText::FromString(NameStr);
	}

	FFormatNamedArguments MetaArgs;
	for (const TPair<FName, FString>& Meta : InMetadata)
	{
		if (!Meta.Key.IsNone() && !Meta.Value.IsEmpty())
		{
			MetaArgs.Add(TEXT("m") + Meta.Key.ToString(), FText::FromString(Meta.Value));
		}
	}

	return FText::Format(NameFmt, MetaArgs);
}

FText UInventoryItemData::GetDescription(const TMap<FName, FString>& InMetadata) const
{
	FText DescFmt = Description;
	if (const FString DescStr = InMetadata.FindRef(NativeItemKeys::Description); !DescStr.IsEmpty())
	{
		DescFmt = FText::FromString(DescStr);
	}

	FFormatNamedArguments MetaArgs;
	for (const TPair<FName, FString>& Meta : InMetadata)
	{
		if (!Meta.Key.IsNone() && !Meta.Value.IsEmpty())
		{
			MetaArgs.Add(TEXT("m") + Meta.Key.ToString(), FText::FromString(Meta.Value));
		}
	}
	
	return FText::Format(DescFmt, MetaArgs);
}

FTransformMeshData UInventoryItemData::GetMeshData(const TMap<FName, FString>& InMetadata) const
{
	const FTransformMeshData AltData = AltMeshes.FindRef(InMetadata.FindRef(NativeItemKeys::AltMeshID));
	if (AltData.IsValidData()) return AltData;
	return BaseMesh;
}

#if WITH_EDITOR
void UInventoryItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
	if (Priority == 255 && (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, ItemType)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, Priority)))
	{
		switch (ItemType)
		{
		case EInventoryItemType::Basic: Priority = 5; break;
		case EInventoryItemType::Objective: Priority = 2; break;
		case EInventoryItemType::Consumable: Priority = 3; break;
		case EInventoryItemType::Equipment: Priority = 1; break;
		case EInventoryItemType::Viewable: Priority = 4; break;
		default: Priority = 6; break;
		}
	}
	if (bUpdate || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, BaseMesh))
	{
		bUpdate = false;
		BaseMesh.FillMaterials();
	}
	if (bUpdate || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, AltMeshes))
	{
		bUpdate = false;
		for (TPair<FString, FTransformMeshData>& Data : AltMeshes)
		{
			Data.Value.FillMaterials();
		}

		return;
	}
#endif

	if (ItemType == EInventoryItemType::Any)
	{
		ItemType = EInventoryItemType::Basic;
	}

	for (const FName Key : NativeItemKeys::All)
	{
		if (!DefaultMetadata.Contains(Key)) DefaultMetadata.Add(Key);
	}

	if (PreviewZoomRange.X > PreviewZoomRange.Y)
	{
		PreviewZoomRange.Y = PreviewZoomRange.X;
	}
}
#endif