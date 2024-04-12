// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSectionData.h"

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

uint64 UGameSectionData::CalcChecksum()
{
	uint64 Hash = GetTypeHash(DependencyDepth);
	for (const TSoftObjectPtr<UWorld>& Obj : Levels)
	{
		if (!Obj.IsNull()) Hash = HashCombine(Hash, GetTypeHash(Obj));
	}
	for (const TSoftObjectPtr<UTexture2D>& Obj : Backgrounds)
	{
		if (!Obj.IsNull()) Hash = HashCombine(Hash, GetTypeHash(Obj));
	}
	for (const TSoftObjectPtr<UObject>& Obj : PreloadObjects)
	{
		if (!Obj.IsNull()) Hash = HashCombine(Hash, GetTypeHash(Obj));
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
	for (const TSoftObjectPtr<UWorld>& Obj : Levels)
	{
		Packages.AddUnique(*Obj.GetLongPackageName());
	}
	for (const TSoftObjectPtr<UTexture2D>& Obj : Backgrounds)
	{
		Packages.AddUnique(*Obj.GetLongPackageName());
	}
	for (const TSoftObjectPtr<UObject>& Obj : PreloadObjects)
	{
		Packages.AddUnique(*Obj.GetLongPackageName());
	}
	Packages.RemoveAll([](const FName& InPackage) -> bool
	{
		return !InPackage.ToString().StartsWith(TEXT("/Game/"));
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
	const uint64 TempChecksum = CalcChecksum();
	if (Checksum != TempChecksum)
	{
		Checksum = TempChecksum;
		FindAllDependencies();
	}

	CheckDisplayName();
}
#endif
