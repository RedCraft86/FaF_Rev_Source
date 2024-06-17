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
				if (Pairs[i].Option == StructPtr->Value) SelectedIdx = i;
				TSharedPtr<FString> Option(MakeShared<FString>(Pairs[i].Option));
				Tooltips.Add(Option, Pairs[i].Tooltip);
				Options.Add(Option);
			}
			
			Options.Add(MakeShared<FString>(TEXT("Custom...")));
			if (SelectedIdx == INDEX_NONE)
			{
				SelectedIdx = Options.Num() - 1;
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
		const TSharedPtr<IPropertyHandle> ValueHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FStringPulldown, Value));
		ValueHandle->MarkHiddenByCustomization();
		if (StructPtr && SelectedIdx == Options.Num() - 1)
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
		StructPtr->Value = SelectedIdx > 0 ? *NewSelection : TEXT("");
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		StructHandle->NotifyFinishedChangingProperties();
		StructHandle->RequestRebuildChildren();
	}

	FText GetOptionLabel() const
	{
		if (!StructPtr) return INVTEXT("Unknown");
		if (SelectedIdx == Options.Num() - 1) return INVTEXT("Custom...");
		return FText::FromString(StructPtr->Value);
	}
	
	FText GetOptionTooltip() const
	{
		if (!StructPtr) return INVTEXT("");
		return FText::FromString(Tooltips.FindRef(Options[SelectedIdx]));
	}
};

class GTEDITOR_API FNamePulldownDetails final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FNamePulldownDetails>();
	}
	
private:

	int32 SelectedIdx = INDEX_NONE;
	TArray<TSharedPtr<FName>> Options;
	TMap<TSharedPtr<FName>, FString> Tooltips;
	FNamePulldown* StructPtr = nullptr;
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		TArray<const void*> StructPtrs;
		StructHandle->AccessRawData(StructPtrs);
		if (StructPtrs.IsEmpty()) return;

		StructPtr = StructPtrs.Num() == 1 ? (FNamePulldown*)StructPtrs[0] : nullptr;
		Tooltips.Empty();
		Options.Empty();
		if (StructPtr)
		{
			const TArray<FPulldownOption> Pairs = StructPtr->GetPulldownOptions();
			Tooltips.Reserve(Pairs.Num());
			Options.Reserve(Pairs.Num() + 1);
			for (int i = 0; i < Pairs.Num(); i++)
			{
				if (Pairs[i].Option == StructPtr->Value) SelectedIdx = i;
				TSharedPtr<FName> Option(MakeShared<FName>(*Pairs[i].Option));
				Tooltips.Add(Option, Pairs[i].Tooltip);
				Options.Add(Option);
			}
			
			Options.Add(MakeShared<FName>(TEXT("Custom...")));
			if (SelectedIdx == INDEX_NONE)
			{
				SelectedIdx = Options.Num() - 1;
			}
		}
		else
		{
			SelectedIdx = 0;
			Options.Add(MakeShared<FName>(TEXT("Unknown")));
		}

		HeaderRow.NameContent()[StructHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SNew(SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&Options)
			.InitiallySelectedItem(Options[SelectedIdx])
			.OnGenerateWidget(this, &FNamePulldownDetails::OnGenerateWidget)
			.OnSelectionChanged(this, &FNamePulldownDetails::OnSelectionChanged, StructHandle)
			.IsEnabled(TAttribute<bool>::CreateLambda([this]() -> bool {return StructPtr != nullptr;}))
			.Content()
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(this, &FNamePulldownDetails::GetOptionLabel)
				.ToolTipText(this, &FNamePulldownDetails::GetOptionTooltip)
			]
		];
	}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> ValueHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNamePulldown, Value));
		ValueHandle->MarkHiddenByCustomization();
		if (StructPtr && SelectedIdx == Options.Num() - 1)
		{
			StructBuilder.AddProperty(ValueHandle.ToSharedRef());
		}
	}

	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FName> InOption) const
	{
		return SNew(STextBlock)
			.Text(FText::FromName(*InOption))
			.ToolTipText(FText::FromString(Tooltips.FindRef(InOption)))
			.Font(IDetailLayoutBuilder::GetDetailFont());
	}

	void OnSelectionChanged(TSharedPtr<FName> NewSelection, ESelectInfo::Type SelectInfo, const TSharedRef<IPropertyHandle> StructHandle)
	{
		if (!StructPtr) return;
		SelectedIdx = Options.Find(NewSelection);
		
		StructHandle->NotifyPreChange();
		StructPtr->Value = SelectedIdx > 0 ? *NewSelection : NAME_None;
		StructHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		StructHandle->NotifyFinishedChangingProperties();
		StructHandle->RequestRebuildChildren();
	}

	FText GetOptionLabel() const
	{
		if (!StructPtr) return INVTEXT("Unknown");
		if (SelectedIdx == Options.Num() - 1) return INVTEXT("Custom...");
		return FText::FromName(StructPtr->Value);
	}
	
	FText GetOptionTooltip() const
	{
		if (!StructPtr) return INVTEXT("");
		return FText::FromString(Tooltips.FindRef(Options[SelectedIdx]));
	}
};