// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "LevelDependencyTracker.generated.h"

struct FAssetData;
class IAssetRegistry;

UCLASS(BlueprintType)
class FAF_REV_API ULevelDependencyTracker final : public UDataAsset
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "DependencyTracker")
		bool bRefresh = false;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DependencyTracker")
		TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DependencyTracker")
		TSet<FName> Dependencies;
	
	UFUNCTION(BlueprintPure, Category = "DependencyTracker")
		TArray<FAssetData> GetAssets();
	
	static IAssetRegistry* GetAssetRegistry();

#if WITH_EDITOR
private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
