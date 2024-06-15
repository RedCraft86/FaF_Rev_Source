// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"

class GTEDITOR_API FGTActorDetails : public IDetailCustomization
{
public:

	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShared<FGTActorDetails>();
	}

private:

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override
	{
		TArray<TWeakObjectPtr<UObject>> Objs;
		DetailBuilder.GetObjectsBeingCustomized(Objs);
		
		TArray<FString> Allowed = {TEXT("Transform"), TEXT("TransformCommon"), TEXT("Settings"),
			TEXT("Actor"), TEXT("Collision"), TEXT("BrushSettings"), TEXT("Tick")};
		
		for (TWeakObjectPtr<UObject> Obj : Objs)
		{
			if (!Obj.IsValid() || Obj->IsTemplate()) continue;
			if (FString Meta = Obj->GetClass()->GetMetaData(TEXT("AllowedCategories")); !Meta.IsEmpty())
			{
				TArray<FString> Temp;
				Meta = Meta.Replace(TEXT(", "), TEXT("")).Replace(TEXT(" ,"), TEXT(""));
				while (Meta.Contains(TEXT(",,"))) Meta.ReplaceInline(TEXT(",,"), TEXT(","));
				Meta.ParseIntoArray(Temp, TEXT(","));
				if (Temp.IsEmpty()) Temp.Add(Meta);
				Allowed.Append(Temp);
			}

			if (FString Meta = Obj->GetClass()->GetMetaData(TEXT("HiddenCategories")); !Meta.IsEmpty())
			{
				TArray<FString> Temp;
				Meta = Meta.Replace(TEXT(", "), TEXT("")).Replace(TEXT(" ,"), TEXT(""));
				while (Meta.Contains(TEXT(",,"))) Meta.ReplaceInline(TEXT(",,"), TEXT(","));
				Meta.ParseIntoArray(Temp, TEXT(","));
				if (Temp.IsEmpty()) Temp.Add(Meta);
				Allowed.RemoveAll([Temp](const FString& Element) -> bool { return Temp.Contains(Element); });
			}
		}

		TArray<FName> CategoryNames;
		DetailBuilder.GetCategoryNames(CategoryNames);
		for (const FName& Name : CategoryNames)
		{
			if (!Allowed.ContainsByPredicate([Name](const FString& Elem)->bool{ return Name.ToString().StartsWith(Elem); }))
			{
				DetailBuilder.HideCategory(Name);
			}
		}
	}
};
