// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "DependencyTracker.generated.h"

struct FAssetData;
class IAssetRegistry;

UCLASS(BlueprintType)
class FAF_REV_API UDependencyTracker final : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Tracker")
		TSoftObjectPtr<UObject> BaseObject;

	UPROPERTY(EditAnywhere, Category = "Tracker")
		TSet<FName> Dependencies;

	UFUNCTION(BlueprintPure, Category = "DependencyTracker", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "AsType"))
		TSoftObjectPtr<UObject> GetBaseObject(const TSubclassOf<UObject> AsType);
	
	UFUNCTION(BlueprintPure, Category = "DependencyTracker")
		TSet<FAssetData> GetDependencies();
	
	static IAssetRegistry* GetAssetRegistry();
	static TSet<FName> GetAllDependencies(const FName& BasePackage);

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditAnywhere, Category = "Tracker", meta = (DisplayPriority = -1))
		bool bRefresh = false;
#endif
#if WITH_EDITOR
private:
	TSoftObjectPtr<UObject> CachedObject = nullptr;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
