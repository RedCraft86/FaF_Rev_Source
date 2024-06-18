// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PulldownTypes.h"

#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"

class GTEDITOR_API FStringPulldownDetails final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStringPulldownDetails>();
	}
	
private:

	int32 SelectedIdx = INDEX_NONE;
	int32 CustomIndex = INDEX_NONE;
	TArray<TSharedPtr<FString>> Options;
	TMap<TSharedPtr<FString>, FString> Tooltips;
	TSharedPtr<IPropertyHandle> StructHandle;
	FStringPulldown* StructPtr = nullptr;
	FSimpleDelegate OnValueChanged;
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		TArray<const void*> StructPtrs;
		InStructHandle->AccessRawData(StructPtrs);
		if (StructPtrs.IsEmpty()) return;

		StructPtr = StructPtrs.Num() == 1 ? (FStringPulldown*)StructPtrs[0] : nullptr;
		Tooltips.Empty();
		Options.Empty();
		if (StructPtr)
		{
			StructHandle = InStructHandle.ToSharedPtr();

			OnValueChanged.Unbind();
			OnValueChanged.BindRaw(this, &FStringPulldownDetails::OnPropertyValueChanged);
			InStructHandle->SetOnPropertyValueChanged(OnValueChanged);
			
			StructPtr->EdData.OnListChanged.BindRaw(this, &FStringPulldownDetails::OnPulldownListChanged);
			const TSortedMap<FString, FString> Pairs = StructPtr->GetPulldownOptions();
			
			TArray<FString> Keys;
			Pairs.GenerateKeyArray(Keys);
			Tooltips.Reserve(Keys.Num());
			Options.Reserve(Keys.Num() + 1);
			
			for (int i = 0; i < Keys.Num(); i++)
			{
				if (Keys[i] == StructPtr->Get()) SelectedIdx = i;
				TSharedPtr<FString> Option(MakeShared<FString>(Keys[i]));
				Tooltips.Add(Option, Pairs.FindRef(Keys[i]));
				Options.Add(Option);
			}

			Options.Add(MakeShared<FString>(TEXT("Custom...")));
			CustomIndex = Options.Num() - 1;
			if (SelectedIdx == INDEX_NONE)
			{
				SelectedIdx = CustomIndex;
			}
		}
		else
		{
			SelectedIdx = 0;
			Options.Add(MakeShared<FString>(TEXT("Unknown")));
			StructHandle.Reset();
		}

		HeaderRow.NameContent()[InStructHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&Options)
			.InitiallySelectedItem(Options[SelectedIdx])
			.OnGenerateWidget(this, &FStringPulldownDetails::OnGenerateWidget)
			.OnSelectionChanged(this, &FStringPulldownDetails::OnSelectionChanged)
			.IsEnabled(StructPtr != nullptr)
			.Content()
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(this, &FStringPulldownDetails::GetOptionLabel)
				.ToolTipText(this, &FStringPulldownDetails::GetOptionTooltip)
			]
		];
	}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> ValueHandle = InStructHandle->GetChildHandle("Value");
		ValueHandle->MarkHiddenByCustomization();
		if (StructPtr && SelectedIdx == CustomIndex)
		{
			StructBuilder.AddProperty(ValueHandle.ToSharedRef());
		}
	}

	void OnPropertyValueChanged()
	{
		if (!StructPtr || !StructHandle.IsValid()) OnValueChanged.Unbind();
		else StructHandle->RequestRebuildChildren();
	}

	void OnPulldownListChanged(FPulldownEdData* EdData) const
	{
		if (!StructPtr || &StructPtr->EdData != EdData) EdData->OnListChanged.Unbind();
		else if (StructHandle.IsValid()) StructHandle->RequestRebuildChildren();
	}

	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> InOption) const
	{
		return SNew(STextBlock)
			.Text(FText::FromString(*InOption))
			.ToolTipText(FText::FromString(Tooltips.FindRef(InOption)))
			.Font(IDetailLayoutBuilder::GetDetailFont());
	}

	void OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
	{
		if (!StructPtr || !StructHandle.IsValid()) return;
		SelectedIdx = Options.Find(NewSelection);
		
		StructHandle->NotifyPreChange();
		StructPtr->Set(SelectedIdx == CustomIndex ? TEXT("None") : *NewSelection);
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		StructHandle->NotifyFinishedChangingProperties();
		StructHandle->RequestRebuildChildren();
	}

	FText GetOptionLabel() const
	{
		if (!StructPtr) return INVTEXT("Unknown");
		if (SelectedIdx == CustomIndex) return INVTEXT("Custom...");
		return FText::FromString(*Options[SelectedIdx]);
	}
	
	FText GetOptionTooltip() const
	{
		if (!StructPtr) return INVTEXT("");
		return FText::FromString(Tooltips.FindRef(Options[SelectedIdx]));
	}
};