// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Quest.h"
#include "PlayerData.h"
#include "UltraDynamicSky.h"
#include "PlayerTeleporter.h"
#include "GameMusic/MusicDataBase.h"
#include "GameSection/GameSectionNode.h"
#include "Inventory/InventoryComponentBase.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "GameSectionData.generated.h"

UCLASS()
class FAF_REV_API UGameSectionDataNode final : public UGameSectionNode
{
	GENERATED_BODY()

public:

	UGameSectionDataNode() : bMinimalLoadingScreen(false)
	{
		LoadingTips = {
			{TEXT("Endings"), INVTEXT("This game has multiple endings but only one is canon. You will get access to a Phase Map to help you reach all the endings once you've achieved at least one ending.")},
			{TEXT("Game History 1"), INVTEXT("This game is actually a spritual successor to my cancelled game \"Fazbear Entertainment: Chapter 2.\"")},
			{TEXT("Game History 2"), INVTEXT("This game went through at least 4 versions before getting to this one. I kept rewriting the code and story to be better than the last.")}
		};
	}

	UPROPERTY(EditAnywhere, Category = "SectionData")
		TMap<TSoftObjectPtr<UWorld>, bool> Levels;
	
	UPROPERTY(EditAnywhere, Category = "SectionData")
		TSet<TSoftObjectPtr<UObject>> PreloadObjects;

	UPROPERTY(EditAnywhere, Category = "LoadingScreen")
		bool bMinimalLoadingScreen;
	
	UPROPERTY(EditAnywhere, Category = "LoadingScreen", meta = (EditCondition = "!bMinimalLoadingScreen"))
		TMap<FString, FText> LoadingTips;
	
	UPROPERTY(EditAnywhere, Category = "LoadingScreen", meta = (EditCondition = "!bMinimalLoadingScreen", NoElementDuplicate))
		TArray<TSoftObjectPtr<UTexture2D>> Backgrounds;
	
	UPROPERTY(EditAnywhere, Category = "PlayerData")
		TSoftObjectPtr<APlayerTeleporter> Teleporter;

	UPROPERTY(EditAnywhere, Category = "PlayerData")
		FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, Category = "PlayerData")
		TArray<FInventorySlotData> Inventory;
	
	UPROPERTY(EditAnywhere, Category = "WorldData")
		FGameplayTag UnlockMenu;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		TSoftClassPtr<UQuest> Quest;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		TSoftObjectPtr<UMusicDataBase> MusicData;

	UPROPERTY(EditAnywhere, Category = "WorldData")
		FUDSSettings SkyWeatherSettings;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Dependencies", meta = (ClampMin = 1, UIMin = 1, ClampMax = 10, UIMax = 10, DisplayName = "Search Depth"))
		uint8 DependencyDepth = 4;

	UPROPERTY(EditAnywhere, Category = "Dependencies")
		TSet<FName> ExcludedObjects = {TEXT("BP_")};
#endif
	
	UPROPERTY(VisibleAnywhere, Category = "Dependencies", meta = (NoResetToDefault))
		TSet<FName> Dependencies;

	TSet<FAssetData> GetDependencies();
	TSoftObjectPtr<UTexture2D> GetBackground();
	TPair<FString, FText> GetTip() const;
	
private:
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "SectionData")
		void QueryDependencies() { FindAllDependencies(); }

	void CheckDisplayName();
	void FindAllDependencies();
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	static IAssetRegistry* GetAssetRegistry()
	{
		static IAssetRegistry* AssetRegistry;
		if (!AssetRegistry) AssetRegistry = IAssetRegistry::Get();
		return AssetRegistry;
	}
};
