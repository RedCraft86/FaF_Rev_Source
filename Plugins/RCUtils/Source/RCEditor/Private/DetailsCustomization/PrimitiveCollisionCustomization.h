// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCPrimitiveTypes.h"

#include "IDocumentation.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"

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

class FPrimitiveCollisionCustomization final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FPrimitiveCollisionCustomization());
	}

private:

	FRCPrimitiveCollision* StructPtr = nullptr;

	TArray<TSharedPtr<FName>> Presets = {};
	TArray<FCollisionChannelInfo> ValidTraceChannels = {};
	TArray<FCollisionChannelInfo> ValidObjectChannels = {};

	void ReloadPresets()
	{
		if (const UCollisionProfile* Profile = UCollisionProfile::Get())
		{
			Presets.Empty(Profile->GetNumOfProfiles() + 1);
			Presets.Add(MakeShareable(new FName("Custom...")));

			TArray<TSharedPtr<FName>> ValidPresets;
			Profile->GetProfileNames(ValidPresets);

			Presets.Append(ValidPresets);
		}
	}
	
	void ReloadValidChannels()
	{
		if (const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>())
		{
			ValidTraceChannels.Empty();
			ValidObjectChannels.Empty();

			for (int32 i = 0; i < ChannelEnum->NumEnums(); i++)
			{
				const ECollisionChannel Channel = (ECollisionChannel)i;
				if (Channel == ECC_OverlapAll_Deprecated || Channel == ECC_MAX)
					continue;

				const FString& RawName = ChannelEnum->GetNameStringByIndex(i);
				const FString& DisplayName = ChannelEnum->GetMetaData(TEXT("DisplayName"), i);
				if (DisplayName.Len() > 0)
				{
					FCollisionChannelInfo ChannelInfo(RawName, DisplayName, Channel);

					if (ChannelEnum->GetMetaData(TEXT("TraceQuery"), i) == TEXT("1"))
					{
						ValidTraceChannels.Add(ChannelInfo);
					}
					else
					{
						ValidObjectChannels.Add(ChannelInfo);
					}
				}
			}
		}
	}

	bool IsUsingCustomProfile() const
	{
		return StructPtr ? !FRCPrimitiveCollision::IsProfileValid(StructPtr->CollisionProfileName) : false;
	}
	
	FName GetProfileName() const
	{
		const UCollisionProfile* Profile = UCollisionProfile::Get();
		if (StructPtr && Profile)
		{
			FCollisionResponseTemplate Template;
			if (Profile->GetProfileTemplate(StructPtr->CollisionProfileName, Template))
			{
				return Template.Name;
			}
		}

		return TEXT("Custom...");
	}

	void SetResponseToAllChannels(ECheckBoxState InCheckState, const ECollisionResponse NewResponse, const TSharedRef<IPropertyHandle> StructPropertyHandle) const
	{
		if (StructPtr)
		{
			StructPropertyHandle->NotifyPreChange();

			StructPtr->SetAllResponses(NewResponse);

			StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			StructPropertyHandle->NotifyFinishedChangingProperties();
		}
	}
	
	ECheckBoxState DoesAllChannelsHaveResponse(const ECollisionResponse ResponseToCheck) const
	{
		if (StructPtr && !StructPtr->Responses.IsEmpty())
		{ 
			TArray<FCollisionChannelInfo> ValidChannels;
			ValidChannels.Append(ValidTraceChannels);
			ValidChannels.Append(ValidObjectChannels);

			int32 NumOfEqualValues = 0;
			for (const FCollisionChannelInfo& Info : ValidChannels)
			{
				const TEnumAsByte<ECollisionResponse>* Response = StructPtr->Responses.Find(Info.CollisionChannel);
				if (Response && *Response == ResponseToCheck)
				{
					NumOfEqualValues++;
				}
			}

			if (NumOfEqualValues == ValidChannels.Num())
			{
				return ECheckBoxState::Checked;
			}
			
			return NumOfEqualValues > 0 ? ECheckBoxState::Undetermined : ECheckBoxState::Unchecked;
		}

		return ECheckBoxState::Undetermined;
	}

	void SetResponseToChannel(ECheckBoxState InCheckState, const ECollisionChannel Channel, const ECollisionResponse NewResponse, const TSharedRef<IPropertyHandle> StructPropertyHandle) const
	{
		if (StructPtr)
		{
			StructPropertyHandle->NotifyPreChange();

			StructPtr->SetResponse(Channel, NewResponse);

			StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			StructPropertyHandle->NotifyFinishedChangingProperties();
		}
	}
	
	ECheckBoxState DoesChannelHaveResponse(const ECollisionChannel Channel, const ECollisionResponse ResponseToCheck) const
	{
		if (StructPtr && StructPtr->Responses.Contains(Channel))
		{
			return *StructPtr->Responses.Find(Channel) == ResponseToCheck ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}

		return ECheckBoxState::Undetermined;
	}
	
	TSharedRef<SWidget> OnPresetsGenerateWidget(TSharedPtr<FName> InOption)
	{
		return SNew(STextBlock).Font(IDetailLayoutBuilder::GetDetailFont()).Text(FText::FromName(*InOption));
	}
	
	void OnPresetsSelectionChanged(TSharedPtr<FName> NewSelection, ESelectInfo::Type SelectInfo, TSharedRef<IPropertyHandle> StructPropertyHandle)
	{
		if (StructPtr)
		{
			StructPropertyHandle->NotifyPreChange();

			bool bWasSuccess = false;
			FCollisionResponseTemplate Template;
			const UCollisionProfile* Profile = UCollisionProfile::Get();
			if (Profile && Profile->GetProfileTemplate(*NewSelection, Template))
			{
				StructPtr->SetCollisionProfile(Template.Name);
				bWasSuccess = true;
			}
			
			if (!bWasSuccess) 
			{
				StructPtr->SetCollisionProfile(UCollisionProfile::CustomCollisionProfileName);
			}

			StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			StructPropertyHandle->NotifyFinishedChangingProperties();
		}
	}
	
	FText GetProfileNameText(const TSharedRef<IPropertyHandle> StructPropertyHandle) const
	{
		const FProperty* PropertyValue;
		if (StructPropertyHandle->GetValue(PropertyValue) == FPropertyAccess::MultipleValues)
		{
			return FText::FromString(TEXT("Multiple Values"));
		}

		return FText::FromName(GetProfileName());
	}
	
	FText GetProfileTooltip(const TSharedRef<IPropertyHandle> StructPropertyHandle) const
	{
		const FProperty* PropertyValue;
		if (StructPropertyHandle->GetValue(PropertyValue) == FPropertyAccess::MultipleValues)
		{
			return FText::FromString(TEXT("Multiple Values"));
		}

		if (StructPtr)
		{
			FCollisionResponseTemplate Template;
			const UCollisionProfile* Profile = UCollisionProfile::Get();
			if (Profile && Profile->GetProfileTemplate(StructPtr->CollisionProfileName, Template))
			{
				return FText::FromString(Template.HelpMessage);
			}
		}

		return FText::FromString(TEXT("Custom Collision Profile"));
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		ReloadPresets();
	
		TArray<const void*> StructPointers;
		StructPropertyHandle->AccessRawData(StructPointers);
		check(StructPointers.Num() != 0);
		StructPtr = (FRCPrimitiveCollision*)StructPointers[0];

		if (StructPtr->Responses.IsEmpty()) StructPtr->FillDefaultResponses();

		const TSharedPtr<IPropertyHandle> ProfileNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCPrimitiveCollision, CollisionProfileName));
		ProfileNameHandle->MarkHiddenByCustomization();

		int32 PresetIdx = 0;
		if (StructPtr)
		{
			for (int32 i = 0; i < Presets.Num(); i++)
			{
				if (Presets[i].IsValid() && *Presets[i] == StructPtr->CollisionProfileName)
				{
					PresetIdx = i;
					break;
				}
			}
		}
		
		HeaderRow
		.OverrideResetToDefault(FResetToDefaultOverride::Create(true, 
			FSimpleDelegate::CreateLambda([this, StructPropertyHandle]() {
				OnPresetsSelectionChanged(MakeShareable(new FName("BlockAll")), ESelectInfo::Type::OnMouseClick, StructPropertyHandle);})
			))
		.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&Presets)
			.OnGenerateWidget(this, &FPrimitiveCollisionCustomization::OnPresetsGenerateWidget)
			.OnSelectionChanged(this, &FPrimitiveCollisionCustomization::OnPresetsSelectionChanged, StructPropertyHandle)
			.InitiallySelectedItem(Presets[PresetIdx])
			.Content()
			[
				SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(this, &FPrimitiveCollisionCustomization::GetProfileNameText, StructPropertyHandle)
					.ToolTipText(this, &FPrimitiveCollisionCustomization::GetProfileTooltip, StructPropertyHandle)
			]
		];
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		ReloadValidChannels();

		constexpr float RowWidth = 50.0f;
		const FSlateRenderTransform RowOffset = FSlateRenderTransform(FVector2D(-15.0f, 0.0f));

		const TAttribute<bool> bCanCustomize(this, &FPrimitiveCollisionCustomization::IsUsingCustomProfile);

		const TSharedPtr<IPropertyHandle> CollisionEnabledHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCPrimitiveCollision, CollisionEnabled));
		const TSharedPtr<IPropertyHandle> ObjectTypeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCPrimitiveCollision, ObjectType));
		const TSharedPtr<IPropertyHandle> ResponsesHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCPrimitiveCollision, Responses));
		
		CollisionEnabledHandle->MarkHiddenByCustomization();
		ObjectTypeHandle->MarkHiddenByCustomization();
		ResponsesHandle->MarkHiddenByCustomization();

		StructBuilder.AddProperty(CollisionEnabledHandle.ToSharedRef()).IsEnabled(bCanCustomize)
			.OverrideResetToDefault(FResetToDefaultOverride::Create(bCanCustomize,
				FSimpleDelegate::CreateLambda([this, CollisionEnabledHandle]()
					{ CollisionEnabledHandle->ResetToDefault(); })
			));

		StructBuilder.AddProperty(ObjectTypeHandle.ToSharedRef()).IsEnabled(bCanCustomize)
			.OverrideResetToDefault(FResetToDefaultOverride::Create(bCanCustomize,
				FSimpleDelegate::CreateLambda([this, ObjectTypeHandle]()
					{ ObjectTypeHandle->ResetToDefault(); })
			));

		StructBuilder.AddCustomRow(FText::GetEmpty()).IsEnabled(bCanCustomize)
		.ValueContent()
		[
			SNew(SHorizontalBox)
			.RenderTransform(RowOffset)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(RowWidth)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(STextBlock)
						.Font(IDetailLayoutBuilder::GetDetailFontBold())
						.Text(FText::FromString(TEXT("Ignore")))
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(RowWidth)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(STextBlock)
						.Font(IDetailLayoutBuilder::GetDetailFontBold())
						.Text(FText::FromString(TEXT("Overlap")))
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(RowWidth)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(STextBlock)
						.Font(IDetailLayoutBuilder::GetDetailFontBold())
						.Text(FText::FromString(TEXT("Block")))
				]
			]
		];
			
		StructBuilder.AddCustomRow(FText::FromString(TEXT("Collsion Responses"))).IsEnabled(bCanCustomize)
		.NameContent()
		[
			SNew(SHorizontalBox)
			.RenderTransform(RowOffset)
			+SHorizontalBox::Slot()
			.Padding(2.0f)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
					.Text(FText::FromString(TEXT("Collsion Responses")))
					.ToolTipText(FText::FromString(TEXT("Override Responses for All Channels")))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				IDocumentation::Get()->CreateAnchor(TEXT("Engine/Physics/Collision"))
			]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			.RenderTransform(RowOffset)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(RowWidth)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(SCheckBox)
						.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToAllChannels, ECR_Ignore, StructPropertyHandle)
						.IsChecked(this, &FPrimitiveCollisionCustomization::DoesAllChannelsHaveResponse, ECR_Ignore)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(RowWidth)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(SCheckBox)
						.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToAllChannels, ECR_Overlap, StructPropertyHandle)
						.IsChecked(this, &FPrimitiveCollisionCustomization::DoesAllChannelsHaveResponse, ECR_Overlap)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(RowWidth)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(SCheckBox)
						.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToAllChannels, ECR_Block, StructPropertyHandle)
						.IsChecked(this, &FPrimitiveCollisionCustomization::DoesAllChannelsHaveResponse, ECR_Block)
				]
			]
		];

		StructBuilder.AddCustomRow(FText::FromString(TEXT("Trace Responses"))).IsEnabled(bCanCustomize)
			.WholeRowContent().HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
					.Text(FText::FromString(TEXT("Trace Responses")))
					.ToolTipText(FText::FromString(TEXT("Collision Responses for Trace Channels")))
			];

		for (FCollisionChannelInfo TraceChannel : ValidTraceChannels)
		{
			StructBuilder.AddCustomRow(FText::FromString(TraceChannel.DisplayName)).IsEnabled(bCanCustomize)
			.NameContent()
			[
				SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(FText::FromString(TraceChannel.DisplayName))
					//.ToolTipText(FText::FromString(FString::Printf(TEXT("Configure Collision Response for the %s Trace Channel."), *TraceChannel.DisplayName)))
					.ToolTipText(FText::FromString(TraceChannel.RawName))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				.RenderTransform(RowOffset)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RowWidth)
					.HAlign(HAlign_Center)
					.Content()
					[
						SNew(SCheckBox)
							.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToChannel, TraceChannel.CollisionChannel, ECR_Ignore, StructPropertyHandle)
							.IsChecked(this, &FPrimitiveCollisionCustomization::DoesChannelHaveResponse, TraceChannel.CollisionChannel, ECR_Ignore)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RowWidth)
					.HAlign(HAlign_Center)
					.Content()
					[
						SNew(SCheckBox)
							.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToChannel, TraceChannel.CollisionChannel, ECR_Overlap, StructPropertyHandle)
							.IsChecked(this, &FPrimitiveCollisionCustomization::DoesChannelHaveResponse, TraceChannel.CollisionChannel, ECR_Overlap)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RowWidth)
					.HAlign(HAlign_Center)
					.Content()
					[
						SNew(SCheckBox)
							.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToChannel, TraceChannel.CollisionChannel, ECR_Block, StructPropertyHandle)
							.IsChecked(this, &FPrimitiveCollisionCustomization::DoesChannelHaveResponse, TraceChannel.CollisionChannel, ECR_Block)
					]
				]
			];
		}

		StructBuilder.AddCustomRow(FText::FromString(TEXT("Object Responses"))).IsEnabled(bCanCustomize)
			.WholeRowContent().HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
					.Text(FText::FromString(TEXT("Object Responses")))
					.ToolTipText(FText::FromString(TEXT("Collision Responses for Object Channels")))
			];

		for (FCollisionChannelInfo ObjectChannel : ValidObjectChannels)
		{
					StructBuilder.AddCustomRow(FText::FromString(ObjectChannel.DisplayName)).IsEnabled(bCanCustomize)
			.NameContent()
			[
				SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(FText::FromString(ObjectChannel.DisplayName))
					.ToolTipText(FText::FromString(TEXT("Configure Collision Response for the ") + ObjectChannel.DisplayName + TEXT(" Object Channel")))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				.RenderTransform(RowOffset)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RowWidth)
					.HAlign(HAlign_Center)
					.Content()
					[
						SNew(SCheckBox)
							.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToChannel, ObjectChannel.CollisionChannel, ECR_Ignore, StructPropertyHandle)
							.IsChecked(this, &FPrimitiveCollisionCustomization::DoesChannelHaveResponse, ObjectChannel.CollisionChannel, ECR_Ignore)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RowWidth)
					.HAlign(HAlign_Center)
					.Content()
					[
						SNew(SCheckBox)
							.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToChannel, ObjectChannel.CollisionChannel, ECR_Overlap, StructPropertyHandle)
							.IsChecked(this, &FPrimitiveCollisionCustomization::DoesChannelHaveResponse, ObjectChannel.CollisionChannel, ECR_Overlap)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RowWidth)
					.HAlign(HAlign_Center)
					.Content()
					[
						SNew(SCheckBox)
							.OnCheckStateChanged(this, &FPrimitiveCollisionCustomization::SetResponseToChannel, ObjectChannel.CollisionChannel, ECR_Block, StructPropertyHandle)
							.IsChecked(this, &FPrimitiveCollisionCustomization::DoesChannelHaveResponse, ObjectChannel.CollisionChannel, ECR_Block)
					]
				]
			];
		}
	}
};