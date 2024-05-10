// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSectionData.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameSectionManager.generated.h"

UCLASS(DisplayName = "Section Manager")
class FAF_REV_API UGameSectionManager : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	UGameSectionManager()
		: bLoading(false), PlayTime(0.0f), Sequence({0})
		, LatentID(0), UnloadingLevels(0), LoadingLevels(0)
		, PlayerChar(nullptr), ThisData(nullptr), LastData(nullptr)
		, SectionGraph(nullptr), LoadingWidget(nullptr)
	{}

	UFUNCTION(BlueprintCallable, Category = "GameSectionManager")
		void LoadSequence();
	
	UFUNCTION(BlueprintCallable, Category = "GameSectionManager")
		void SetSequence(const TArray<uint8>& InSequence);
	
	UFUNCTION(BlueprintCallable, Category = "GameSectionManager")
		void Step(const uint8 Index);
	
	UFUNCTION(BlueprintPure, Category = "GameSectionManager")
		bool IsBusy() const { return bLoading; }

private:
	
	bool bLoading;
	float PlayTime;
	TArray<uint8> Sequence;
	uint8 LatentID, UnloadingLevels, LoadingLevels;
	UPROPERTY(Transient) AFRPlayerBase* PlayerChar;
	UPROPERTY(Transient) TSet<UObject*> LoadedObjs;
	UPROPERTY(Transient) UGameSectionDataNode* ThisData;
	UPROPERTY(Transient) UGameSectionDataNode* LastData;
	UPROPERTY(Transient) class UGameSectionGraph* SectionGraph;
	UPROPERTY(Transient) class ULoadingWidgetBase* LoadingWidget;
	
	void BeginTransition();
	void UnloadLastData();
	void LoadCurrentData();
	void FinishLoading();
	void FinishTransition();

	bool UnloadLevel(const TSoftObjectPtr<UWorld>& InMap);
	bool LoadLevel(const TPair<TSoftObjectPtr<UWorld>, bool>& InMap);
	uint8 GetLatentID() { return LatentID++; }

	void HideLoadingWidget(const TFunction<void()>& OnFinished);
	void ShowLoadingWidget();

	UFUNCTION() void OnLevelUnloaded();
	UFUNCTION() void OnLevelLoaded();

	ULoadingWidgetBase* GetLoadingWidget();
	virtual void Tick(float DeltaTime) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE ||
			WorldType == EWorldType::GamePreview || WorldType == EWorldType::GameRPC;
	}
	virtual TStatId GetStatId() const override { return UObject::GetStatID(); }
};
