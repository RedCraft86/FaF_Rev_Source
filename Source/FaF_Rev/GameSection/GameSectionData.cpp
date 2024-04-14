// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionData.h"

#include "Algo/RandomShuffle.h"

UGameSectionData::UGameSectionData()
{
	DependencyDepth = 4;
}

TSet<FAssetData> UGameSectionData::GetDependencies()
{
	TSet<FAssetData> Result;
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

TSoftObjectPtr<UTexture2D> UGameSectionData::GetBackground()
{
	Backgrounds.RemoveAll([](const TSoftObjectPtr<UTexture2D>& Element) -> bool { return Element.IsNull(); });
	Algo::RandomShuffle(Backgrounds); Algo::RandomShuffle(Backgrounds); // Twice
	return Backgrounds.IsEmpty() ? nullptr : Backgrounds[0];
}

TPair<FString, FText> UGameSectionData::GetTip() const
{
	if (LoadingTips.IsEmpty()) return{};
	
	TArray<FString> TipKeys;
	LoadingTips.GenerateKeyArray(TipKeys);
	Algo::RandomShuffle(TipKeys); Algo::RandomShuffle(TipKeys); // Twice
	return {TipKeys[0], LoadingTips.FindRef(TipKeys[0])};
}

uint32 UGameSectionData::CalcChecksum()
{
	uint32 Hash = GetTypeHash(DependencyDepth);
	for (const TPair<TSoftObjectPtr<UWorld>, bool>& Pair : Levels)
	{
		if (!Pair.Key.IsNull()) Hash = HashCombineFast(Hash, GetTypeHash(Pair));
	}
	return Hash;
}

#if WITH_EDITOR
void UGameSectionData::CheckDisplayName()
{
	if (DisplayLabel.IsEmptyOrWhitespace())
	{
		DisplayLabel = FText::FromString(FName::NameToDisplayString(GetName(), false));
	}
}

void UGameSectionData::FindAllDependencies()
{
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
			TempPackages.RemoveAll([](const FName& InPackage) -> bool
			{
				return !InPackage.ToString().StartsWith(TEXT("/Game/"));
			});
			SubPackages.Append(TempPackages);
		}

		Packages = SubPackages;
		Dependencies.Append(SubPackages);
		CurrentDepth--;
	}
}

void UGameSectionData::PostInitProperties()
{
	Super::PostInitProperties();
	CheckDisplayName();
}

void UGameSectionData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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
