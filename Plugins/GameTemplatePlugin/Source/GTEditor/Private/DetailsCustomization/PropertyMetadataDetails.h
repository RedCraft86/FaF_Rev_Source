// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "UObject/WeakFieldPtr.h"

class IDetailLayoutBuilder;
class IBlueprintEditor;
class UBlueprint;

class FPropertyMetadataDetails final : public IDetailCustomization
{
public:

	static TSharedPtr<IDetailCustomization> MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor);

	FPropertyMetadataDetails(TSharedPtr<IBlueprintEditor> InBlueprintEditor, UBlueprint* Blueprint)
		: BlueprintEditorPtr(InBlueprintEditor)
		, BlueprintPtr(Blueprint)
		, NonEditableKeys({
			"tooltip", "ExposeOnSpawn", "MakeEditWidget", "BlueprintPrivate", "Bitmask", "BitmaskEnum", "MultiLine", "DeprecationMessage",
			"ClampMin", "ClampMax", "UIMin", "UIMax", "GetOptions", "EditInline", "EditCondition", "EditConditionHides", "InlineEditConditionToggle",
			"InlineCategoryProperty", "ShowOnlyInnerProperties", "HideAlphaChannel", "ForceInlineRow"
		})
	{}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

private:

	TArray<FBPVariableMetaDataEntry> GetMetadataEntries(int PropertyIndex) const;

	static FSlateFontInfo GetFont(const int Size = 0, const bool bBold = false);
	static FProperty* GetInnermostProperty(const TWeakFieldPtr<FProperty>& Property);
	static bool IsPropertyOfType(const TWeakFieldPtr<FProperty>& Property, const FFieldClass* FieldType);
	static bool IsPropertyOfStruct(const TWeakFieldPtr<FProperty>& Property, const UScriptStruct* Struct);
	static bool IsMapWithStructKey(const TWeakFieldPtr<FProperty>& Property);

	int GetMetadataCount(FName Property) const;
	bool DoesMetadataExist(FName Property, FName Key) const;
	void RemoveMetadata(FName Property, FName Key) const;
	FString GetMetadataValue(FName Property, FName Key) const;
	void SetMetadataValue(FName Property, FName Key, const FString& Value) const;
	void ChangeMetadataKey(FName Property, FName OldKey, FName NewKey) const;
	void ChangeMetadataValue(FName Property, FName Key, const FString& Value, bool bDeleteEmpty = false) const;
	void VerifyMetadata(FName Property, FName Key) const;

	TWeakPtr<IBlueprintEditor> BlueprintEditorPtr;
	TWeakObjectPtr<UBlueprint> BlueprintPtr;
	const TSet<FName> NonEditableKeys;
};