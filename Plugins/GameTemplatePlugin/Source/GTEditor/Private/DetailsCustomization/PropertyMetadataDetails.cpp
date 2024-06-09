// Copyright (C) RedCraft86. All Rights Reserved.

#include "PropertyMetadataDetails.h"
#include "PropertyCustomizationHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintEditorModule.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"

#define LOCTEXT_NAMESPACE "GTEditor"
#define MAKE_TOOLTIP(inKey, inLiteral) FSlateApplicationBase::Get().MakeToolTip(LOCTEXT(inKey, inLiteral))

TSharedPtr<IDetailCustomization> FPropertyMetadataDetails::MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor)
{
	const TArray<UObject*>* Objects = (InBlueprintEditor.IsValid() ? InBlueprintEditor->GetObjectsCurrentlyBeingEdited() : nullptr);
	if (Objects && Objects->Num() == 1)
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>((*Objects)[0]))
		{
			return MakeShared<FPropertyMetadataDetails>(InBlueprintEditor, Blueprint);
		}
	}

	return nullptr;
}

void FPropertyMetadataDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);
	if (Objects.IsEmpty()) return;
	
	UPropertyWrapper* PropWrapper = Cast<UPropertyWrapper>(Objects[0].Get());
	const TWeakFieldPtr<FProperty> Property = PropWrapper ? PropWrapper->GetProperty() : nullptr;
	if (!Property.IsValid()) return;

	FName PropertyName = Property->GetFName();
	const int32 PropertyIndex = FBlueprintEditorUtils::FindNewVariableIndex(BlueprintPtr.Get(), PropertyName);
	if (PropertyIndex == INDEX_NONE) return;

	auto ToggleMetadata = [this, PropertyName](const ECheckBoxState State, FName MetadataKey)
	{
		if (State == ECheckBoxState::Checked)
		{
			SetMetadataValue(PropertyName, MetadataKey, "");
		}
		else
		{
			RemoveMetadata(PropertyName, MetadataKey);
		}
	};

	auto OnEditConditionChanged = [this, PropertyName](const FText& NewValue, ETextCommit::Type ChangeType)
	{
		ChangeMetadataValue(PropertyName, "EditCondition", NewValue.ToString(), true);
	};

	auto OnAddMetadata = [this, PropertyName]()
	{
		SetMetadataValue(PropertyName, *FString::Printf(TEXT("NewMeta_%d"), GetMetadataCount(PropertyName)), "");
	};

	auto OnRemoveMetadata = [this, PropertyName](FName MetadataKey)
	{
		RemoveMetadata(PropertyName, MetadataKey);
	};

	auto OnMetaKeyChanged = [this, PropertyName](const FText& NewKey, ETextCommit::Type ChangeType, FName OldKey)
	{
		ChangeMetadataKey(PropertyName, OldKey, FName(*NewKey.ToString()));
	};

	auto OnMetaValueChanged = [this, PropertyName](const FText& NewValue, ETextCommit::Type ChangeType, FName MetadataKey)
	{
		ChangeMetadataValue(PropertyName, MetadataKey, NewValue.ToString(), true);
	};

	VerifyMetadata(PropertyName, "tooltip");
	VerifyMetadata(PropertyName, "UIMin");
	VerifyMetadata(PropertyName, "UIMax");
	VerifyMetadata(PropertyName, "ClampMin");
	VerifyMetadata(PropertyName, "ClampMax");

	IDetailGroup& MetadataGroup = DetailLayout.EditCategory("Variable").AddGroup("MetadataGroup", LOCTEXT("MetadataGroupName", "Metadata"));
	MetadataGroup.HeaderRow()
	.WholeRowContent()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.ToolTip(MAKE_TOOLTIP("VariableMetadataTooltip", "Modify the metadata entries of this variable."))
		.Text(LOCTEXT("VariableMetadataTitle", "Property Metadata"))
		.Justification(ETextJustify::Center)
		.Font(GetFont(10, true))
	];

	if (Property->IsA(FStructProperty::StaticClass()))
	{
		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("StructInnerTooltip", "Inner properties of structs will not be placed in a seperate group."))
			.Text(LOCTEXT("StructInnerName", "Show Only Inner Properties"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(DoesMetadataExist(PropertyName, "ShowOnlyInnerProperties"))
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda(ToggleMetadata, "ShowOnlyInnerProperties"))
		];
	}
	else
	{
		RemoveMetadata(PropertyName, "ShowOnlyInnerProperties");
	}

	if (IsPropertyOfStruct(Property, TBaseStructure<FLinearColor>::Get()))
	{
		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("HideLinearColorAlphaTooltip", "Hides the Alpha Channel from showing up in the color picker and the details panel."))
			.Text(LOCTEXT("HideLinearColorAlphaName", "Hide Alpha Channel"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(DoesMetadataExist(PropertyName, "HideAlphaChannel"))
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda(ToggleMetadata, "HideAlphaChannel"))
		];
	}
	else
	{
		RemoveMetadata(PropertyName, "HideAlphaChannel");
	}

	if (IsMapWithStructKey(Property))
	{
		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("StructMapInlineTooltip", "If a map property with a key type of a struct displays vertically you can use this to force it back to a row.\nHowever, it may cause some unexpected behaviour."))
			.Text(LOCTEXT("StructMapInlineName", "Force Inline Row"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(DoesMetadataExist(PropertyName, "ForceInlineRow"))
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda(ToggleMetadata, "ForceInlineRow"))
		];
	}
	else
	{
		RemoveMetadata(PropertyName, "inlineRow");
	}

	if (IsPropertyOfType(Property, FStrProperty::StaticClass()) || IsPropertyOfType(Property, FNameProperty::StaticClass()))
	{
		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("GetOptionsTooltip", "Get the available options of this variable from an String/Name array property or function."))
			.Text(LOCTEXT("GetOptionsName", "Get Options"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SEditableTextBox)
			.Font(GetFont())
			.SelectAllTextOnCommit(false)
			.Text(FText::FromString(GetMetadataValue(PropertyName, "GetOptions")))
			.OnTextCommitted(FOnTextCommitted::CreateLambda(OnMetaValueChanged, "GetOptions"))
		];
	}
	else
	{
		RemoveMetadata(PropertyName, "GetOptions");
	}

	if ((Property->IsA(FBoolProperty::StaticClass()) || Property->IsA(FEnumProperty::StaticClass()) || Property->IsA(FByteProperty::StaticClass())))
	{
		if (DoesMetadataExist(PropertyName, "InlineCategoryProperty"))
			RemoveMetadata(PropertyName, "InlineEditConditionToggle");

		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("InlineCategoryTooltip", "Property will be embedded into the category header."))
			.Text(LOCTEXT("InlineCategoryName", "Inline Category Property"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(DoesMetadataExist(PropertyName, "InlineCategoryProperty"))
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda(ToggleMetadata, "InlineCategoryProperty"))
		]
		.IsEnabled(!DoesMetadataExist(PropertyName, "InlineEditConditionToggle"));
	}
	else
	{
		RemoveMetadata(PropertyName, "InlineCategoryProperty");
	}

	if (Property->IsA(FBoolProperty::StaticClass()))
	{
		if (DoesMetadataExist(PropertyName, "InlineEditConditionToggle"))
			RemoveMetadata(PropertyName, "InlineCategoryProperty");

		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("InlineEditCondToggleTooltip", "Boolean property will show inline with the edit condition property it's controlling."))
			.Text(LOCTEXT("InlineEditCondToggleName", "Inline Edit Condition Toggle"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(DoesMetadataExist(PropertyName, "InlineEditConditionToggle"))
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda(ToggleMetadata, "InlineEditConditionToggle"))
		]
		.IsEnabled(!DoesMetadataExist(PropertyName, "InlineCategoryProperty"));;
	}
	else
	{
		RemoveMetadata(PropertyName, "InlineEditConditionToggle");
	}

	MetadataGroup.AddWidgetRow()
	.NameContent()
	[
		SNew(STextBlock)
		.ToolTip(MAKE_TOOLTIP("EditConditionTooltip", "Used to change a property's editor editability depending on another property.\nCan accept simple operators such as A > B or A == B."))
		.Text(LOCTEXT("EditConditionName", "Edit Condition"))
		.Font(GetFont())
	]
	.ValueContent()
	[
		SNew(SEditableTextBox)
		.Font(GetFont())
		.SelectAllTextOnCommit(false)
		.Text(FText::FromString(GetMetadataValue(PropertyName, "EditCondition")))
		.OnTextCommitted(FOnTextCommitted::CreateLambda(OnEditConditionChanged))
	];

	if (!GetMetadataValue(PropertyName, "EditCondition").IsEmpty())
	{
		MetadataGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTip(MAKE_TOOLTIP("EditConditionHidesTooltip", "Used with Edit Condition, property becomes fully hidden instead of being editor read-only."))
			.Text(LOCTEXT("EditConditionHidesName", "Edit Condition Hides"))
			.Font(GetFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(DoesMetadataExist(PropertyName, "EditConditionHides"))
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda(ToggleMetadata, "EditConditionHides"))
		];
	}
	else
	{
		RemoveMetadata(PropertyName, "EditConditionHides");
	}

	IDetailGroup& CustomMetadataGroup = MetadataGroup.AddGroup("CustomMetadataGroup", LOCTEXT("CustomMetadataGroupName", "Custom Metadata"));
	CustomMetadataGroup.HeaderRow()
	.NameContent()
	[
		SNew(STextBlock)
		.ToolTip(MAKE_TOOLTIP("CustomMetadataTooltip", "List of Metadata entries."))
		.Text(LOCTEXT("CustomMetadataName", "Metadata List"))
		.Font(GetFont())
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::Printf(TEXT("%d Metadata Entries"), GetMetadataCount(PropertyName))))
			.Font(GetFont())
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.0f, 0.0f, 0.0f, 0.0f)
		[
			PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateLambda(OnAddMetadata), LOCTEXT("MetadataAddName", "Add Metadata"))
		]
	];

	TArray<FBPVariableMetaDataEntry> MetadataEntries = GetMetadataEntries(PropertyIndex);
	for (const FBPVariableMetaDataEntry Entry : MetadataEntries)
	{
		bool bNonEditable = NonEditableKeys.Contains(Entry.DataKey);

		CustomMetadataGroup.AddWidgetRow()
		.NameContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SEditableTextBox)
			.Font(GetFont())
			.IsReadOnly(bNonEditable)
			.ToolTip(MAKE_TOOLTIP("MetadataKeyName", "Metadata Key"))
			.Text(FText::FromName(Entry.DataKey))
			.OnTextCommitted(FOnTextCommitted::CreateLambda(OnMetaKeyChanged, Entry.DataKey))
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SEditableTextBox)
				.Font(GetFont())
				.IsReadOnly(bNonEditable)
				.ToolTip(MAKE_TOOLTIP("MetadataValueName", "Metadata Value"))
				.Text(FText::FromString(Entry.DataValue))
				.OnTextCommitted(FOnTextCommitted::CreateLambda(OnMetaValueChanged, Entry.DataKey))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f, 0.0f, 0.0f, 0.0f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			[
				PropertyCustomizationHelpers::MakeRemoveButton(FSimpleDelegate::CreateLambda(OnRemoveMetadata, Entry.DataKey), LOCTEXT("MetadataRemoveName", "Remove Metadata"), !bNonEditable)
			]
		];
	}
}

TArray<FBPVariableMetaDataEntry> FPropertyMetadataDetails::GetMetadataEntries(int PropertyIndex) const
{
	if (!BlueprintPtr->NewVariables.IsValidIndex(PropertyIndex)) return TArray<FBPVariableMetaDataEntry>();

	TArray<FBPVariableMetaDataEntry> A;
	TArray<FBPVariableMetaDataEntry> B;
	const TArray<FBPVariableMetaDataEntry> Unsorted = BlueprintPtr->NewVariables[PropertyIndex].MetaDataArray;
	for (const FBPVariableMetaDataEntry& Metadata : Unsorted)
	{
		if (NonEditableKeys.Contains(Metadata.DataKey))
		{
			A.Add(Metadata);
		}
		else
		{
			B.Add(Metadata);
		}
	}

	A.Append(B);
	return A;
}

FSlateFontInfo FPropertyMetadataDetails::GetFont(const int Size, const bool bBold)
{
	FSlateFontInfo Font = bBold ? IDetailLayoutBuilder::GetDetailFontBold() : IDetailLayoutBuilder::GetDetailFont();
	Font.Size = (Size > 0) ? Size : Font.Size;
	return Font;
}

FProperty* FPropertyMetadataDetails::GetInnermostProperty(const TWeakFieldPtr<FProperty>& Property)
{
	// Hopefully this never nulls since the editor will crash lmao
	if (!Property.IsValid()) return nullptr;

	if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
	{
		return ArrayProp->Inner;
	}
	
	if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
	{
		return SetProp->ElementProp;
	}

	return Property.Get();
}

bool FPropertyMetadataDetails::IsPropertyOfType(const TWeakFieldPtr<FProperty>& Property, const FFieldClass* FieldType)
{
	if (!Property.IsValid()) return false;

	// NOTE: Maps are a special case since the metadata seems to apply to both the Key and the Value
	// So we'll return true if either the Key or the Value matches
	if (const FMapProperty* MapProp = CastField<FMapProperty>(Property.Get()))
	{
		return MapProp->KeyProp->IsA(FieldType) || MapProp->ValueProp->IsA(FieldType);
	}

	return GetInnermostProperty(Property)->IsA(FieldType);
}

bool FPropertyMetadataDetails::IsPropertyOfStruct(const TWeakFieldPtr<FProperty>& Property, const UScriptStruct* Struct)
{
	if (!Property.IsValid()) return false;

	// NOTE: For some reason metadata adjustments for structs in container type properties don't apply, probably due to some details customization stuff
	// So we'll just look at the property itself and not check for arrays, sets and maps
	const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get());
	return StructProp ? StructProp->Struct == Struct : false;
}

bool FPropertyMetadataDetails::IsMapWithStructKey(const TWeakFieldPtr<FProperty>& Property)
{
	if (!Property.IsValid()) return false;
	if (const FMapProperty* MapProp = CastField<FMapProperty>(Property.Get()))
	{
		return MapProp->KeyProp->IsA(FStructProperty::StaticClass());
	}

	return false;
}

int FPropertyMetadataDetails::GetMetadataCount(FName Property) const
{
	if (Property.IsNone()) return 0;

	const int VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(BlueprintPtr.Get(), Property);
	if (VarIndex != INDEX_NONE) return BlueprintPtr->NewVariables[VarIndex].MetaDataArray.Num();
	return 0;
}

bool FPropertyMetadataDetails::DoesMetadataExist(FName Property, FName Key) const
{
	if (Property.IsNone() || Key.IsNone()) return false;

	FString Value;
	return FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), Property, nullptr, Key, Value);
}

void FPropertyMetadataDetails::RemoveMetadata(FName Property, FName Key) const
{
	if (!DoesMetadataExist(Property, Key)) return;
	FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BlueprintPtr.Get(), Property, nullptr, Key);
}

FString FPropertyMetadataDetails::GetMetadataValue(FName Property, FName Key) const
{
	if (!DoesMetadataExist(Property, Key)) return TEXT("");

	FString Value;
	FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), Property, nullptr, Key, Value);

	return Value;
}

void FPropertyMetadataDetails::SetMetadataValue(FName Property, FName Key, const FString& Value) const
{
	if (Property.IsNone() || Key.IsNone()) return;
	FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), Property, nullptr, Key, Value);
}

void FPropertyMetadataDetails::ChangeMetadataKey(FName Property, FName OldKey, FName NewKey) const
{
	if (Property.IsNone() || OldKey.IsNone()) return;
	if (NewKey.IsNone())
	{
		RemoveMetadata(Property, OldKey);
	}
	else
	{
		FString Value = GetMetadataValue(Property, NewKey);
		if (Value.IsEmpty()) Value = GetMetadataValue(Property, OldKey);

		RemoveMetadata(Property, OldKey);
		SetMetadataValue(Property, NewKey, Value);
	}
}

void FPropertyMetadataDetails::ChangeMetadataValue(FName Property, FName Key, const FString& Value, bool bDeleteEmpty) const
{
	if (Property.IsNone() || Key.IsNone()) return;
	if (Value.IsEmpty() && bDeleteEmpty)
	{
		RemoveMetadata(Property, Key);
	}
	else
	{
		SetMetadataValue(Property, Key, Value);
	}
}

void FPropertyMetadataDetails::VerifyMetadata(FName Property, FName Key) const
{
	if (Property.IsNone() || Key.IsNone()) return;
	if (DoesMetadataExist(Property, Key) && GetMetadataValue(Property, Key).IsEmpty())
	{
		RemoveMetadata(Property, Key);
	}
}

#undef MAKE_TOOLTIP
#undef LOCTEXT_NAMESPACE