// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Quest.h"
#include "Core/CoreMacros.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Inventory/GCInventoryTypes.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GCSequenceTypes.generated.h"

UENUM(BlueprintInternalUseOnly)
enum class EGCCustomBoolKeys : uint8
{
	CanRun,
	CanPause,
};

UENUM(BlueprintInternalUseOnly)
enum class EGCCustomFloatKeys : uint8
{
	WalkMultiplier,
	RunningSpeed,
	LightIntensity,
};

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
		bool bCanRun;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		float WalkMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (Bitmask, BitmaskEnum = "/Script/GameCore.EGCPlayerAbilityFlags"))
		int32 AbilityFlags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (GetOptions = "CustomBooleanKeys"))
		TMap<EGCCustomBoolKeys, bool> CustomBooleanData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (GetOptions = "CustomNumberKeys"))
		TMap<EGCCustomFloatKeys, float> CustomNumberData;
	
	FGCSequenceData()
		: Label(TEXT(""))
		, Backgrounds({})
		, Level(nullptr)
		, TeleporterTag({})
		, QuestClass(NULL)
		, MainMusic(nullptr)
		, ChaseMusic(nullptr)
		, EnsureItems({})
		, AbilityFlags(0)
	{}

	FORCEINLINE FName GetLevelName() const
	{
		return *FPackageName::ObjectPathToPackageName(Level.ToString());
	}
	
	FORCEINLINE bool IsValidData() const
	{
		return !Level.IsNull() && TeleporterTag.IsValid();
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