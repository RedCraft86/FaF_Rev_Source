// Copyright (C) RedCraft86. All Rights Reserved.

#include "DependencyTracker.h"
#include "AssetRegistry/IAssetRegistry.h"

TSoftObjectPtr<UObject> UDependencyTracker::GetBaseObject(const TSubclassOf<UObject> AsType)
{
	return BaseObject;
}

TSet<FAssetData> UDependencyTracker::GetDependencies()
{
	TSet<FAssetData> Result;
	for (const FName& Package : Dependencies)
	{
		TArray<FAssetData> Assets;
		GetAssetRegistry()->GetAssetsByPackageName(Package, Assets);
		if (!Assets.IsEmpty() && Assets[0].IsValid() && !Assets[0].IsAssetLoaded())
		{
			Result.Add(Assets[0]);
		}
	}

	return Result;
}

IAssetRegistry* UDependencyTracker::GetAssetRegistry()
{
	static IAssetRegistry* AssetRegistry;
	if (!AssetRegistry) AssetRegistry = IAssetRegistry::Get();
	return AssetRegistry;
}

TSet<FName> UDependencyTracker::GetAllDependencies(const FName& BasePackage, const uint8 Depth)
{
	TSet<FName> Result;
	TArray<FName> BasePackages;
	GetAssetRegistry()->GetDependencies(BasePackage, BasePackages);
	BasePackages.RemoveAll([](const FName& Package) -> bool
	{
		return !Package.ToString().StartsWith(TEXT("/Game/"));
	});

	uint8 CurrentDepth = Depth;
	TSet<FName> Packages = TSet(BasePackages);
	while (CurrentDepth)
	{
		CurrentDepth--;
		TSet<FName> SubPackages;
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
		Result.Append(SubPackages);
	}

	Result.Add(BasePackage);
	return Result;
}
#if WITH_EDITOR
void UDependencyTracker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDependencyTracker, MaxDepth)
		|| bRefresh || CachedObject != BaseObject)
	{
		bRefresh = false;
		CachedObject = BaseObject;
		Dependencies = GetAllDependencies(*BaseObject.GetLongPackageName(), MaxDepth);
	}
#endif
}
#endif
