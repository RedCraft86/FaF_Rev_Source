// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSectionData.h"
#include "Engine/StreamableManager.h"
#include "GameSection/Graph/GameSectionGraph.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameSectionManager.generated.h"

UCLASS()
class FAF_REV_API UGameSectionManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GameSectionManager")
		void Step(const int32 Index);
	
	UFUNCTION(BlueprintPure, Category = "GameSectionManager")
		bool IsBusy() const { return bLoading; }

private:
	
	bool bLoading;
	TArray<uint8> Sequence;
	uint8 LatentID, UnloadingLevels, LoadingLevels;
	UPROPERTY(Transient) TSet<UObject*> LoadedObjs;
	UPROPERTY(Transient) UGameSectionData* ThisData;
	UPROPERTY(Transient) UGameSectionData* LastData;
	UPROPERTY(Transient) UGameSectionGraph* SectionGraph;
	
	void BeginTransition();
	void UnloadLastData();
	void LoadCurrentData();
	void FinishLoading();
	void FinishTransition();

	bool UnloadLevel(const TSoftObjectPtr<UWorld>& InMap);
	bool LoadLevel(const TPair<TSoftObjectPtr<UWorld>, bool>& InMap);
	uint8 GetLatentID() { return LatentID++; }

	UFUNCTION() void OnLevelUnloaded();
	UFUNCTION() void OnLevelLoaded();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE ||
			WorldType == EWorldType::GamePreview || WorldType == EWorldType::GameRPC;
	}
};
