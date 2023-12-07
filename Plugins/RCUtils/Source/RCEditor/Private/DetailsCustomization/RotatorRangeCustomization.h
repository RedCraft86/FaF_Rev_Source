// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCMathTypes.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"
#include "RCEdHelpers.h"

class FRotatorRangeCustomization final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FRotatorRangeCustomization());
	}

private:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const FMargin PropertySpacing(15.0f, 0.0f, 0.0f, 0.0f);
		const FMargin NameValueSpacing(4.0f, 0.0f, 0.0f, 0.0f);

		const TSharedPtr<IPropertyHandle> ClampRollHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, bClampRoll));
		const TSharedPtr<IPropertyHandle> ClampPitchHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, bClampPitch));
		const TSharedPtr<IPropertyHandle> ClampYawHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, bClampYaw));

		HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("X")))
					.ToolTipText(FText::FromString(TEXT("Clamp the X (Roll) axis")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(NameValueSpacing)
				[
					ClampRollHandle->CreatePropertyValueWidget()
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(PropertySpacing)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Y")))
					.ToolTipText(FText::FromString(TEXT("Clamp the Y (Pitch) axis")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(NameValueSpacing)
				[
					ClampPitchHandle->CreatePropertyValueWidget()
				]
			]
			+ SHorizontalBox::Slot()
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
					.ToolTipText(FText::FromString(TEXT("Clamp the Z (Yaw) axis")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(NameValueSpacing)
				[
					ClampYawHandle->CreatePropertyValueWidget()
				]
			]
		];
	}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		constexpr float PropertyWidth(40.0f);
		const FMargin PropertySpacing(5.0f, 0.0f, 0.0f, 0.0f);

		const TSharedPtr<IPropertyHandle> MinHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, MinRotation));
		const TSharedPtr<IPropertyHandle> MaxHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, MaxRotation));

		auto IsClampingAxis = [StructPropertyHandle](const EAxis::Type Axis)->bool{
			bool bResult = false;
			switch (Axis)
			{
			case EAxis::X:
				StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, bClampRoll))->GetValue(bResult);
				break;

			case EAxis::Y:
				StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, bClampPitch))->GetValue(bResult);
				break;

			case EAxis::Z:
				StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCRotatorRange, bClampYaw))->GetValue(bResult);
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
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			[
				RCEdHelpers::CreatePropertyValueWidget(MinHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRotator, Roll)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::X), FText::FromString(TEXT("Min Roll")))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MinHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRotator, Pitch)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Y), FText::FromString(TEXT("Min Pitch")))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MinHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRotator, Yaw)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Z), FText::FromString(TEXT("Min Yaw")))
			]
		];

		StructBuilder.AddCustomRow(FText::FromString(TEXT("ClampMaxValues")))
		.NameContent()[MaxHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			[
				RCEdHelpers::CreatePropertyValueWidget(MaxHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRotator, Roll)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::X), FText::FromString(TEXT("Max Roll")))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MaxHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRotator, Pitch)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Y), FText::FromString(TEXT("Max Pitch")))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(PropertyWidth)
			.MaxWidth(PropertyWidth)
			.Padding(PropertySpacing)
			[
				RCEdHelpers::CreatePropertyValueWidget(MaxHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRotator, Yaw)),
					TAttribute<bool>::CreateLambda(IsClampingAxis, EAxis::Z), FText::FromString(TEXT("Max Yaw")))
			]
		];
	}
};
