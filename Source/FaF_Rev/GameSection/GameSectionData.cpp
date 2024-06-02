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
			if (Asset.IsValid() && !Asset.IsAssetLoaded())
			{
				Result.Add(Asset);
			}
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
uint32 UGameSectionDataNode::CalcChecksum()
{
#if WITH_EDITORONLY_DATA
	uint32 Hash = GetTypeHash(DependencyDepth);
	for (const FName& Object : ExcludedObjects)
	{
		Hash = HashCombine(Hash, GetTypeHash(Object));
	}
#else
	uint32 Hash = 0;
#endif
	for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : Levels)
	{
		if (!Pair.Key.IsNull()) Hash = HashCombine(Hash, GetTypeHash(Pair));
	}
	return Hash;
}

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
	const uint32 TempChecksum = CalcChecksum();
	if (Checksum != TempChecksum)
	{
		Checksum = TempChecksum;
		FindAllDependencies();
	}

	CheckDisplayName();
}
#endif
