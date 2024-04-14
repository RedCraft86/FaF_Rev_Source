// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRPlayer.h"
#include "UltraDynamicSky.h"
#include "GameMusic/GameMusicData.h"
#include "GameSection/GameSectionDataBase.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "GameSectionData.generated.h"

UCLASS()
class FAF_REV_API UGameSectionData : public UGameSectionDataBase
{
	GENERATED_BODY()

public:

	UGameSectionData();

	UPROPERTY(EditAnywhere, Category = "SectionData")
		FText DisplayLabel;
	
	UPROPERTY(EditAnywhere, Category = "SectionData")
		TMap<TSoftObjectPtr<UWorld>, bool> Levels;
	
	UPROPERTY(EditAnywhere, Category = "SectionData")
		TSet<TSoftObjectPtr<UObject>> PreloadObjects;

	UPROPERTY(EditAnywhere, Category = "LoadingScreen")
		TSet<TSoftObjectPtr<UTexture2D>> Backgrounds;
	
	UPROPERTY(EditAnywhere, Category = "LoadingScreen", meta = (NoElementDuplicate))
		TMap<FString, FText> LoadingTips;

	UPROPERTY(EditAnywhere, Category = "PlayerData")
		TSoftObjectPtr<AActor> Teleporter;

	UPROPERTY(EditAnywhere, Category = "PlayerData", meta = (ShowOnlyInnerProperties))
		FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		TSoftClassPtr<UObject> Quest;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		TSoftObjectPtr<UGameMusicData> Theme;

	UPROPERTY(EditAnywhere, Category = "WorldData", DisplayName = "Weather")
		FUDSSettings WeatherSettings;
	
	UPROPERTY(EditAnywhere, Category = "Dependencies", meta = (ClampMin = 1, UIMin = 1, ClampMax = 8, UIMax = 8, DisplayName = "Search Depth"))
		uint8 DependencyDepth;

	UPROPERTY(VisibleAnywhere, Category = "Dependencies", meta = (NoResetToDefault))
		TSet<FName> Dependencies;

	TSet<FAssetData> GetDependencies();
	
private:
#if WITH_EDITORONLY_DATA
	UFUNCTION(CallInEditor, Category = "SectionData")
		void QueryDependencies() { FindAllDependencies(); }
#endif
#if WITH_EDITOR
	uint32 Checksum = 0;
	uint32 CalcChecksum();
	void CheckDisplayName();
	void FindAllDependencies();
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FText GetTitle() const override { return DisplayLabel; }
#endif
	static IAssetRegistry* GetAssetRegistry()
	{
		static IAssetRegistry* AssetRegistry;
		if (!AssetRegistry) AssetRegistry = IAssetRegistry::Get();
		return AssetRegistry;
	}
};
