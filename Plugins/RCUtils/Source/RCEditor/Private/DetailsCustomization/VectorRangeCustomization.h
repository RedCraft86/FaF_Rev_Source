// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCMathTypes.h"

#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"
#include "RCEdHelpers.h"

class FVectorRangeCustomization final : public IPropertyTypeCustomization
{
public:

    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
    	return MakeShareable(new FVectorRangeCustomization());
    }

private:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const FMargin PropertySpacing(15.0f, 0.0f, 0.0f, 0.0f);
		const FMargin NameValueSpacing(4.0f, 0.0f, 0.0f, 0.0f);

		const TSharedPtr<IPropertyHandle> ClampXHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, bClampX));
		const TSharedPtr<IPropertyHandle> ClampYHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, bClampY));
		const TSharedPtr<IPropertyHandle> ClampZHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, bClampZ));

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
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("X")))
					.ToolTipText(FText::FromString(TEXT("Clamp the X axis")))
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(NameValueSpacing)
				[
					ClampXHandle->CreatePropertyValueWidget()
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(PropertySpacing)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Y")))
					.ToolTipText(FText::FromString(TEXT("Clamp the Y axis")))
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(NameValueSpacing)
				[
					ClampYHandle->CreatePropertyValueWidget()
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(PropertySpacing)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Z")))
					.ToolTipText(FText::FromString(TEXT("Clamp the Z axis")))
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(NameValueSpacing)
				[
					ClampZHandle->CreatePropertyValueWidget()
				]
			]
		];
	}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		constexpr float PropertyWidth(40.0f);
		const FMargin PropertySpacing(5.0f, 0.0f, 0.0f, 0.0f);

		const TSharedPtr<IPropertyHandle> MinHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, MinVector));
		const TSharedPtr<IPropertyHandle> MaxHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, MaxVector));

		auto IsClampingAxis = [StructPropertyHandle](const EAxis::Type Axis)->bool{
			bool bResult = false;
			switch (Axis)
			{
			case EAxis::X:
				StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, bClampX))->GetValue(bResult);
				break;

			case EAxis::Y:
				StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, bClampY))->GetValue(bResult);
				break;

			case EAxis::Z:
				StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCVectorRange, bClampZ))->GetValue(bResult);
				break;

			default: break;
			}
			return bResult;
		};

		StructBuilder.AddCustomRow(FText::FromString(TEXT("ClampMinValues")))
		.NameContent()[MinHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			[
				RCEdHelpers::CreatePropertyValueWidget(MinHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector, X)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::X), FText::FromString(TEXT("Min X")))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MinHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector, Y)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Y), FText::FromString(TEXT("Min Y")))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MinHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector, Z)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Z), FText::FromString(TEXT("Min Z")))
			]
		];

		StructBuilder.AddCustomRow(FText::FromString(TEXT("ClampMaxValues")))
		.NameContent()[MaxHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			[
				RCEdHelpers::CreatePropertyValueWidget(MaxHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector, X)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::X), FText::FromString(TEXT("Max X")))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MaxHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector, Y)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Y), FText::FromString(TEXT("Max Y")))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MaxHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector, Z)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Z), FText::FromString(TEXT("Max Z")))
			]
		];
	}
};
