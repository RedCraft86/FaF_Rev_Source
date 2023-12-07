// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"

class FGCMovableNPCCustomization final : public IDetailCustomization
{
public:

	const TSet<FName> VisibleCategories = {TEXT("NPC"), TEXT("Transform"), TEXT("TransformCommon"), TEXT("Actor"),
		TEXT("Input"), TEXT("Pawn"), TEXT("Tick")};
 
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FGCMovableNPCCustomization);
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
