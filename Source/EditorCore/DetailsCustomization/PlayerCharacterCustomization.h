// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"

class FGCPlayerCharacterCustomization final : public IDetailCustomization
{
public:

	const TSet<FName> VisibleCategories = {TEXT("Settings"), TEXT("References"), TEXT("Transform"), TEXT("TransformCommon"),
		TEXT("Actor"), TEXT("Input"), TEXT("Pawn"), TEXT("Tick")};
 
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FGCPlayerCharacterCustomization);
	}

private:
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override
	{
		TArray<FName> CategoryNames;
		DetailBuilder.GetCategoryNames(CategoryNames);
		for (const FName& CategoryName : CategoryNames)
		{
			if (!VisibleCategories.Contains(CategoryName))
			{
				DetailBuilder.HideCategory(CategoryName);
			}
		}
	}
};

// Prop->NotifyPreChange();
// Prop->NotifyPostChange(EPropertyChangeType::ValueSet);
// Prop->NotifyFinishedChangingProperties();