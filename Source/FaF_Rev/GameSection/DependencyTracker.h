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

	// How far to look down the reference chain
	UPROPERTY(EditAnywhere, Category = "Tracker", meta = (ClampMin = 1, UIMin = 1, ClampMax = 5, UIMax = 5))
		uint8 MaxDepth = 3;
	
	UPROPERTY(EditAnywhere, Category = "Tracker")
		TSoftObjectPtr<UObject> BaseObject;

	UPROPERTY(VisibleAnywhere, Category = "Tracker")
		TSet<FName> Dependencies;

	UFUNCTION(BlueprintPure, Category = "DependencyTracker", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "AsType"))
		TSoftObjectPtr<UObject> GetBaseObject(const TSubclassOf<UObject> AsType);
	
	UFUNCTION(BlueprintPure, Category = "DependencyTracker")
		TSet<FAssetData> GetDependencies();
	
	static IAssetRegistry* GetAssetRegistry();
	static TSet<FName> GetAllDependencies(const FName& BasePackage, const uint8 Depth);

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
