// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionData.h"
#include "Algo/RandomShuffle.h"

TSet<FAssetData> UGameSectionDataNode::GetDependencies()
{
	TSet<FAssetData> Result;
	if (bMinimalLoadingScreen) return Result;
	
	for (const FName& Package : Dependencies)
	{
		TArray<FAssetData> Assets;
		GetAssetRegistry()->GetAssetsByPackageName(Package, Assets);
		for (FAssetData& Asset : Assets)
		{
			if (Asset.IsValid()) Result.Add(Asset);
		}
	}

	return Result;
}

TSoftObjectPtr<UTexture2D> UGameSectionDataNode::GetBackground()
{
	if (Backgrounds.IsEmpty() || bMinimalLoadingScreen) return nullptr;
	Backgrounds.RemoveAll([](const TSoftObjectPtr<UTexture2D>& Element) -> bool { return Element.IsNull(); });
	Algo::RandomShuffle(Backgrounds); Algo::RandomShuffle(Backgrounds); // Twice
	return Backgrounds.IsEmpty() ? nullptr : Backgrounds[0];
}

TPair<FString, FText> UGameSectionDataNode::GetTip() const
{
	if (LoadingTips.IsEmpty() || bMinimalLoadingScreen) return{};
	
	TArray<FString> TipKeys;
	LoadingTips.GenerateKeyArray(TipKeys);
	Algo::RandomShuffle(TipKeys); Algo::RandomShuffle(TipKeys); // Twice
	return {TipKeys[0], LoadingTips.FindRef(TipKeys[0])};
}

#if WITH_EDITOR
void UGameSectionDataNode::CheckDisplayName()
{
	if (DisplayLabel.IsEmptyOrWhitespace())
	{
		DisplayLabel = FText::FromString(FName::NameToDisplayString(GetName(), false));
	}
}

void UGameSectionDataNode::FindAllDependencies()
{
#if WITH_EDITORONLY_DATA
	TArray<FName> Packages;
	for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : Levels)
	{
		if (!Pair.Key.IsNull()) Packages.AddUnique(*Pair.Key.GetLongPackageName());
	}
	Packages.RemoveAll([](const FName& InPackage) -> bool
	{
		return InPackage.IsNone() || !InPackage.ToString().StartsWith(TEXT("/Game/"));
	});
	Dependencies = TSet(Packages);
	
	uint8 CurrentDepth = DependencyDepth;
	while (CurrentDepth)
	{
		TArray<FName> SubPackages;
		for (const FName& Package : Packages)
		{
			TArray<FName> TempPackages;
			GetAssetRegistry()->GetDependencies(Package, TempPackages);
			TempPackages.RemoveAll([this](const FName& InPackage) -> bool
			{
				if (!InPackage.ToString().StartsWith(TEXT("/Game/"))) return true;
				for (const FName& Object : ExcludedObjects)
				{
					if (InPackage.ToString().Contains(Object.ToString()))
						return true;
				}

				return false;
			});
			SubPackages.Append(TempPackages);
		}

		Packages = SubPackages;
		Dependencies.Append(SubPackages);
		CurrentDepth--;
	}
#endif
}

void UGameSectionDataNode::PostInitProperties()
{
	Super::PostInitProperties();
	CheckDisplayName();
}

void UGameSectionDataNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, Levels)
#if WITH_EDITORONLY_DATA
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, DependencyDepth)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, ExcludedObjects)
#endif
		)
	{
		FindAllDependencies();
	}

	CheckDisplayName();
}
#endif
