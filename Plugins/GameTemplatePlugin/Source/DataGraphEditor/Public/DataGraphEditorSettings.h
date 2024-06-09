// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "DataGraphEditorSettings.generated.h"

UENUM(BlueprintType)
enum class EDataGraphLayoutStrategy : uint8
{
	Tree,
	ForceDirected,
};

UCLASS(Config = Editor, DefaultConfig, DisplayName = "Data Graph Settings")
class DATAGRAPHEDITOR_API UDataGraphEditorSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	static UDataGraphEditorSettings* Get() { return GetMutableDefault<UDataGraphEditorSettings>(); }

	UDataGraphEditorSettings()
	{
		CategoryName = TEXT("GameTemplate");
		SectionName = TEXT("DataGraphSettings");
		
		AutoLayoutStrategy = EDataGraphLayoutStrategy::Tree;
		bFirstPassOnly = false;
		bRandomInit = false;
		OptimalDistance = 100.f;
		MaxIteration = 50;
		InitTemperature = 10.f;
		CoolDownRate = 10.f;
	}

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		float OptimalDistance;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		EDataGraphLayoutStrategy AutoLayoutStrategy;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		uint8 MaxIteration;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		bool bFirstPassOnly;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		bool bRandomInit;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		float InitTemperature;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
		float CoolDownRate;
};
