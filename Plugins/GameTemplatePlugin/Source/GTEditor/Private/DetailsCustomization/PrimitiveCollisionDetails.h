// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PrimitiveData.h"

struct FCollisionChannelInfo
{
	FString RawName;
	FString DisplayName;
	ECollisionChannel CollisionChannel;

	FCollisionChannelInfo(const FString& InRawName, const FString& InDisplayName, const ECollisionChannel InChannel)
		: RawName (InRawName)
		, DisplayName(InDisplayName)
		, CollisionChannel(InChannel)
	{}
};

class FPrimitiveCollisionDetails final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FPrimitiveCollisionDetails>();
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

	bool bEnabled = false;
	FPrimitiveCollision* StructPtr = nullptr;
	
	TArray<TSharedPtr<FName>> Presets;
	TArray<FCollisionChannelInfo> ValidTraceChannels;
	TArray<FCollisionChannelInfo> ValidObjectChannels;

	void ReloadPresets();
	TSharedRef<SWidget> OnGeneratePresetWidget(TSharedPtr<FName> InOption) const;
	void OnPresetSelectionChanged(TSharedPtr<FName> NewSelection, ESelectInfo::Type SelectInfo, const TSharedRef<IPropertyHandle> StructHandle) const;
	FText GetProfileLabel(const TSharedRef<IPropertyHandle> StructPropertyHandle) const;
	FText GetProfileTooltip(const TSharedRef<IPropertyHandle> StructPropertyHandle) const;
	
	void ReloadChannels();
	bool CanCustomize() const;
	void SetResponseToAllChannels(ECheckBoxState InCheckState, const ECollisionResponse NewResponse, const TSharedRef<IPropertyHandle> StructHandle) const;
	ECheckBoxState DoesAllChannelsHaveResponse(const ECollisionResponse ResponseToCheck) const;
	void SetResponseToChannel(ECheckBoxState InCheckState, const ECollisionChannel Channel, const ECollisionResponse NewResponse, const TSharedRef<IPropertyHandle> StructHandle) const;
	ECheckBoxState DoesChannelHaveResponse(const ECollisionChannel Channel, const ECollisionResponse ResponseToCheck) const;
};
