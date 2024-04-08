// Copyright (C) RedCraft86. All Rights Reserved.

#include "LevelDependencyTracker.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define GAME_ONLY 0

TArray<FAssetData> ULevelDependencyTracker::GetAssets()
{
	TArray<FAssetData> Result;
	for (const FName& Package : Dependencies)
	{
		TArray<FAssetData> Assets;
		GetAssetRegistry()->GetAssetsByPackageName(Package, Assets);
		if (!Assets.IsEmpty() && !Assets[0].IsAssetLoaded())
		{
			Result.AddUnique(Assets[0]);
		}
	}

	return Result;
}

IAssetRegistry* ULevelDependencyTracker::GetAssetRegistry()
{
	static IAssetRegistry* AssetRegistry;
	if (!AssetRegistry)
	{
		AssetRegistry = IAssetRegistry::Get();
	}

	return AssetRegistry;
}

#if WITH_EDITOR
void ULevelDependencyTracker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ULevelDependencyTracker, Level)
		|| bRefresh)
	{
		Dependencies.Empty();
		if (Level.IsNull()) return;

		TArray<FName> Packages;
		const FName LevelPackage = *Level.GetLongPackageName();
		GetAssetRegistry()->GetDependencies(LevelPackage, Packages);
#if GAME_ONLY
		Packages.Add(LevelPackage);
		Dependencies.Reserve(Packages.Num() - 10);
		for (const FName& Package : Packages)
		{
			if (Package.ToString().StartsWith(TEXT("/Game/")))
			{
				Dependencies.Add(Package);
			}
		}
#else
		Dependencies.Add(LevelPackage);
		Dependencies.Append(Packages);
#endif
	}
}
#endif
