// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCRuntimeTypes.h"

#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"

class FGameVersionCustomization final : public IPropertyTypeCustomization
{
public:

    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
    	return MakeShareable(new FGameVersionCustomization());
    }

private:
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> MajorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCGameVersion, Major));
		MajorHandle->SetToolTipText(FText::FromString(TEXT("Major")));

		const TSharedPtr<IPropertyHandle> MinorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCGameVersion, Minor));
		MinorHandle->SetToolTipText(FText::FromString(TEXT("Minor")));

		const TSharedPtr<IPropertyHandle> PatchHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCGameVersion, Patch));
		PatchHandle->SetToolTipText(FText::FromString(TEXT("Patch")));

		HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					MajorHandle->CreatePropertyValueWidget()
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(2.0f, 0.0f))
				[
					SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
					.Text(FText::FromString(TEXT(".")))
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					MinorHandle->CreatePropertyValueWidget()
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(2.0f, 0.0f))
				[
					SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
					.Text(FText::FromString(TEXT(".")))
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					PatchHandle->CreatePropertyValueWidget()
				]
			]
		];
	}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> AdditionalHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCGameVersion, AdditionalInfo));
		AdditionalHandle->SetToolTipText(FText::FromString(TEXT("Additional Information (eg. \"Debug Build\")")));

		StructBuilder.AddProperty(AdditionalHandle.ToSharedRef());
	}
};