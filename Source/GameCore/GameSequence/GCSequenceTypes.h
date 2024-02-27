// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Quest.h"
#include "Core/CoreMacros.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Inventory/GCInventoryTypes.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GCSequenceTypes.generated.h"

namespace ValueIDs
{
	static FString None(TEXT("None"));
	
	static FString CanRun(TEXT("CanRun"));
	static FString CanPause(TEXT("CanPause"));

	static FString MaxStamina(TEXT("MaxStamina"));
	static FString WalkMulti(TEXT("WalkMulti"));
	static FString RunSpeed(TEXT("RunSpeed"));
	static FString LightIntensity(TEXT("LightIntensity"));
	static FString LightRange(TEXT("LightRange"));
	
	namespace Defaults
	{
		static bool CanRun(true);
		static bool CanPause(true);

		static float MaxStamina(100.0f);
		static float WalkMulti(1.0f);
		static float RunSpeed(650.0f);
		static float LightIntensity(0.15f);
		static float LightRange(400);
	}
}

USTRUCT(BlueprintType, DisplayName = "Game Sequence ID")
struct GAMECORE_API FGCSequenceID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGCSequenceID);
};
SETUP_PULLDOWN_HASH(FGCSequenceID);

USTRUCT(BlueprintType, DisplayName = "Game Sequence Data")
struct GAMECORE_API FGCSequenceData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData", meta = (MultiLine = true))
		FString Label;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TArray<TSoftObjectPtr<UTexture2D>> Backgrounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSet<TSoftObjectPtr<UWorld>> PreloadLevels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSet<TSoftObjectPtr<UObject>> PreloadObjects;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		FGameplayTag TeleporterTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSoftClassPtr<UQuest> QuestClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSoftObjectPtr<USoundBase> MainMusic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSoftObjectPtr<USoundBase> ChaseMusic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		FGameplayTag InventoryKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceData")
		TSet<FGCInvItemID> EnsureItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TMap<FString, bool> CustomBooleanData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		TMap<FString, float> CustomNumberData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Abilities")
		bool bCanCloseEyes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Abilities")
		TSubclassOf<UUserWidget> MapWidget;
	
	FGCSequenceData()
		: Label(TEXT(""))
		, Backgrounds({})
		, Level(nullptr)
		, TeleporterTag({})
		, QuestClass(NULL)
		, MainMusic(nullptr)
		, ChaseMusic(nullptr)
		, EnsureItems({})
		, bCanCloseEyes(false)
		, MapWidget(NULL)
	{
		CustomBooleanData = {
			{ValueIDs::CanRun, ValueIDs::Defaults::CanRun},
			{ValueIDs::CanPause, ValueIDs::Defaults::CanPause}
		};
		
		CustomNumberData = {
			{ValueIDs::MaxStamina, ValueIDs::Defaults::MaxStamina},
			{ValueIDs::WalkMulti, ValueIDs::Defaults::WalkMulti},
			{ValueIDs::RunSpeed, ValueIDs::Defaults::RunSpeed},
			{ValueIDs::LightIntensity, ValueIDs::Defaults::LightIntensity},
			{ValueIDs::LightRange, ValueIDs::Defaults::LightRange}
		};
	}

	FORCEINLINE FName GetLevelName() const
	{
		return *FPackageName::ObjectPathToPackageName(Level.ToString());
	}
	
	FORCEINLINE bool IsValidData() const
	{
		return !Level.IsNull();
	}

	TSoftObjectPtr<UTexture2D> GetRandomBackground()
	{
		if (Backgrounds.IsEmpty()) return nullptr;
		Backgrounds.RemoveAll([](const TSoftObjectPtr<UTexture2D>& Element) -> bool { return Element.IsNull(); });
		return Backgrounds[FMath::RandRange(0, Backgrounds.Num() - 1)];
	}

#if WITH_EDITOR
private:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);
		if (QuestClass == UQuest::StaticClass())
		{
			QuestClass = nullptr;
		}

		if (Label.IsEmpty())
		{
			Label = InRowName.ToString();
			Label.ReplaceInline(TEXT("Ch"), TEXT("Chapter "));
			Label.ReplaceInline(TEXT("Seq"), TEXT("Sequence "));
			Label.ReplaceInline(TEXT("_"), TEXT(" - "));
		}
	}
#endif
};