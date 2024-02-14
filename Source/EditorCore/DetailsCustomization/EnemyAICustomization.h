// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"

class FGCEnemyAICustomization final : public IDetailCustomization
{
public:

	const TSet<FName> VisibleCategories = {TEXT("Settings"), TEXT("AI_Logic"), TEXT("StateMachineComponents"),
		TEXT("Transform"), TEXT("TransformCommon"), TEXT("Actor"), TEXT("Pawn"), TEXT("Tick")};
 
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FGCEnemyAICustomization);
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
