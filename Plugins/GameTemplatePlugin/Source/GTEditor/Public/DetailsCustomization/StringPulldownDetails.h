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
	FStringPulldown* StructPtr = nullptr;
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		TArray<const void*> StructPtrs;
		StructHandle->AccessRawData(StructPtrs);
		if (StructPtrs.IsEmpty()) return;

		StructPtr = StructPtrs.Num() == 1 ? (FStringPulldown*)StructPtrs[0] : nullptr;
		Tooltips.Empty();
		Options.Empty();
		if (StructPtr)
		{
			const TArray<FPulldownOption> Pairs = StructPtr->GetPulldownOptions();
			Tooltips.Reserve(Pairs.Num());
			Options.Reserve(Pairs.Num() + 1);
			for (int i = 0; i < Pairs.Num(); i++)
			{
				if (Pairs[i].Option == StructPtr->Get()) SelectedIdx = i;
				TSharedPtr<FString> Option(MakeShared<FString>(Pairs[i].Option));
				Tooltips.Add(Option, Pairs[i].Tooltip);
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
		}

		HeaderRow.NameContent()[StructHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&Options)
			.InitiallySelectedItem(Options[SelectedIdx])
			.OnGenerateWidget(this, &FStringPulldownDetails::OnGenerateWidget)
			.OnSelectionChanged(this, &FStringPulldownDetails::OnSelectionChanged, StructHandle)
			.IsEnabled(TAttribute<bool>::CreateLambda([this]() -> bool {return StructPtr != nullptr;}))
			.Content()
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(this, &FStringPulldownDetails::GetOptionLabel)
				.ToolTipText(this, &FStringPulldownDetails::GetOptionTooltip)
			]
		];
	}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> ValueHandle = StructHandle->GetChildHandle("Value");
		ValueHandle->MarkHiddenByCustomization();
		if (StructPtr && SelectedIdx == CustomIndex)
		{
			StructBuilder.AddProperty(ValueHandle.ToSharedRef());
		}
	}

	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> InOption) const
	{
		return SNew(STextBlock)
			.Text(FText::FromString(*InOption))
			.ToolTipText(FText::FromString(Tooltips.FindRef(InOption)))
			.Font(IDetailLayoutBuilder::GetDetailFont());
	}

	void OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo, const TSharedRef<IPropertyHandle> StructHandle)
	{
		if (!StructPtr) return;
		SelectedIdx = Options.Find(NewSelection);
		
		StructHandle->NotifyPreChange();
		StructPtr->Set(SelectedIdx == CustomIndex ? TEXT("") : *NewSelection);
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