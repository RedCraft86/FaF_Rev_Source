// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PlayerData.h"
#include "GameMusicData.h"
#include "UltraDynamicSky.h"
#include "PlayerTeleporter.h"
#include "GameSection/GameSectionDataBase.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "GameSectionData.generated.h"

UCLASS()
class FAF_REV_API UGameSectionData : public UGameSectionDataBase
{
	GENERATED_BODY()

public:

	UGameSectionData() : DependencyDepth(4) {}

	UPROPERTY(EditAnywhere, Category = "SectionData")
		FText DisplayLabel;
	
	UPROPERTY(EditAnywhere, Category = "SectionData")
		TMap<TSoftObjectPtr<UWorld>, bool> Levels;
	
	UPROPERTY(EditAnywhere, Category = "SectionData")
		TSet<TSoftObjectPtr<UObject>> PreloadObjects;
	
	UPROPERTY(EditAnywhere, Category = "LoadingScreen")
		TMap<FString, FText> LoadingTips;
	
	UPROPERTY(EditAnywhere, Category = "LoadingScreen", meta = (NoElementDuplicate))
		TArray<TSoftObjectPtr<UTexture2D>> Backgrounds;
	
	UPROPERTY(EditAnywhere, Category = "PlayerData")
		TSoftObjectPtr<APlayerTeleporter> Teleporter;

	UPROPERTY(EditAnywhere, Category = "PlayerData", meta = (ShowOnlyInnerProperties))
		FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		TSoftClassPtr<UObject> Quest;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		FGameMusicID MusicID;

	UPROPERTY(EditAnywhere, Category = "WorldData", DisplayName = "Weather")
		FUDSSettings WeatherSettings;
	
	UPROPERTY(EditAnywhere, Category = "Dependencies", meta = (ClampMin = 1, UIMin = 1, ClampMax = 8, UIMax = 8, DisplayName = "Search Depth"))
		uint8 DependencyDepth;

	UPROPERTY(VisibleAnywhere, Category = "Dependencies", meta = (NoResetToDefault))
		TSet<FName> Dependencies;

	TSet<FAssetData> GetDependencies();
	TSoftObjectPtr<UTexture2D> GetBackground();
	TPair<FString, FText> GetTip() const;
	
private:
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "SectionData")
		void QueryDependencies() { FindAllDependencies(); }

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
