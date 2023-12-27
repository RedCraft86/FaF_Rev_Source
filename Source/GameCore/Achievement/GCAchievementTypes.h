// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Core/CoreMacros.h"
#include "Engine/DataTable.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GCAchievementTypes.generated.h"

USTRUCT(BlueprintType, DisplayName = "Achievement ID")
struct GAMECORE_API FGCAchievementID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGCAchievementID);
};
SETUP_PULLDOWN_HASH(FGCAchievementID);

USTRUCT(BlueprintType, DisplayName = "Achievement Data")
struct GAMECORE_API FGCAchievementData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AchievementData")
		TSoftObjectPtr<UTexture2D> IconImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AchievementData")
		FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AchievementData", meta = (MultiLine = true))
		FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AchievementData", meta = (ClampMin = 1, UIMin = 1))
		uint8 MaxProgress;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AchievementData")
		bool bIsHidden;

	FGCAchievementData()
		: IconImage(nullptr)
		, DisplayName(FText::FromString("Invalid Achievement"))
		, Description(FText::FromString("This is an Invalid Achievement!"))
		, MaxProgress(1)
		, bIsHidden(false)
	{}

	FORCEINLINE bool IsValidData() const
	{
		return !DisplayName.IsEmptyOrWhitespace() && MaxProgress > 0;
	}
};
